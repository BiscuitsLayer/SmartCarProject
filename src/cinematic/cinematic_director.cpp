#include "cinematic_director.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include <car_model/car_model.hpp>
#include <helpers/helpers.hpp>

namespace App {
namespace {

using json = nlohmann::json;
constexpr float PI = 3.14159265358979323846f;

GL::Vec3 ReadVec3(const json& value, const GL::Vec3& fallback = {}) {
    if (!value.is_array() || value.size() != 3) {
        return fallback;
    }
    return {value[0].get<float>(), value[1].get<float>(), value[2].get<float>()};
}

std::vector<std::string> ReadNames(const json& value) {
    std::vector<std::string> names;
    if (value.is_array()) {
        for (const auto& name : value) {
            names.push_back(name.get<std::string>());
        }
    }
    return names;
}

} // namespace

CinematicDirector::CinematicDirector(const std::string& config_path) {
    std::ifstream input(config_path);
    if (!input) {
        throw std::runtime_error("Cannot open cinematic config: " + config_path);
    }

    json config;
    input >> config;
    const auto& export_config = config.at("export");
    fps_ = export_config.value("fps", 30);
    duration_ = export_config.at("duration").get<float>();

    const auto read_motion = [](const json& value) {
        Motion motion;
        motion.type = value.value("type", "fixed");
        motion.position = ReadVec3(value.value("position", json::array()));
        motion.center = ReadVec3(value.value("center", json::array()));
        motion.target = ReadVec3(value.value("target", json::array()));
        motion.radius = value.value("radius", 0.0f);
        motion.radius_x = value.value("radius_x", motion.radius);
        motion.radius_z = value.value("radius_z", motion.radius);
        motion.height = value.value("height", 0.0f);
        motion.start_angle = value.value("start_angle", 0.0f);
        motion.angular_speed = value.value("angular_speed", 0.0f);
        motion.yaw = value.value("yaw", 0.0f);
        motion.yaw_mode = value.value("yaw_mode", "fixed");
        motion.slip_angle = value.value("slip_angle", 0.0f);
        motion.wheel_speed = value.value("wheel_speed", 0.0f);
        return motion;
    };

    for (const auto& value : config.at("shots")) {
        Shot shot;
        shot.start = value.at("start").get<float>();
        shot.end = value.at("end").get<float>();
        shot.draw_car = value.value("draw_car", true);
        shot.car = read_motion(value.value("car", json::object()));
        shot.camera = read_motion(value.at("camera"));
        const auto visibility = value.value("visible", json::object());
        shot.environments = ReadNames(visibility.value("environment", json::array()));
        shot.obstacles = ReadNames(visibility.value("obstacles", json::array()));
        shots_.push_back(std::move(shot));
    }

    if (fps_ <= 0 || duration_ <= 0.0f || shots_.empty()) {
        throw std::runtime_error("Cinematic config needs positive fps/duration and at least one shot");
    }
}

void CinematicDirector::Update(float time, float delta_time) {
    for (size_t i = 0; i < shots_.size(); ++i) {
        if (time >= shots_[i].start && time < shots_[i].end) {
            current_shot_ = i;
            break;
        }
    }

    const auto& shot = CurrentShot();
    const float local_time = time - shot.start;
    auto& context = Context::Get();

    if (shot.draw_car) {
        GL::Vec3 car_position = shot.car.position;
        float yaw = shot.car.yaw;
        if (shot.car.type == "orbit") {
            const float angle = shot.car.start_angle + local_time * shot.car.angular_speed;
            car_position = shot.car.center + GL::Vec3{
                std::cos(angle) * shot.car.radius_x,
                shot.car.height,
                std::sin(angle) * shot.car.radius_z
            };
            if (shot.car.yaw_mode == "tangent_with_slip") {
                const float tangent_x = -std::sin(angle) * shot.car.radius_x * shot.car.angular_speed;
                const float tangent_z = std::cos(angle) * shot.car.radius_z * shot.car.angular_speed;
                yaw = std::atan2(tangent_x, tangent_z) * 180.0f / PI + shot.car.slip_angle;
            }
        }
        context.car_model->SetCinematicPose(car_position, yaw);
        context.car_model->SpinWheels(delta_time * shot.car.wheel_speed);
    }

    GL::Vec3 camera_position = shot.camera.position;
    if (shot.camera.type == "orbit") {
        const float angle = shot.camera.start_angle + local_time * shot.camera.angular_speed;
        camera_position = shot.camera.center + GL::Vec3{
            std::cos(angle) * shot.camera.radius_x,
            shot.camera.height,
            std::sin(angle) * shot.camera.radius_z
        };
    }
    context.camera->SetPose(camera_position, shot.camera.target);
}

bool CinematicDirector::DrawCar() const {
    return CurrentShot().draw_car;
}

bool CinematicDirector::DrawEnvironment(const std::string& name) const {
    return Contains(CurrentShot().environments, name);
}

bool CinematicDirector::DrawObstacle(const std::string& name) const {
    return Contains(CurrentShot().obstacles, name);
}

const CinematicDirector::Shot& CinematicDirector::CurrentShot() const {
    return shots_.at(current_shot_);
}

bool CinematicDirector::Contains(const std::vector<std::string>& names, const std::string& name) {
    return std::find(names.begin(), names.end(), name) != names.end();
}

} // namespace App
