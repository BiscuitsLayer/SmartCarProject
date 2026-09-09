#pragma once

#include <string>
#include <vector>

#include <GL/OOGL.hpp>

namespace App {

class CinematicDirector {
public:
    explicit CinematicDirector(const std::string& config_path);

    void Update(float time, float delta_time);
    bool DrawCar() const;
    bool DrawEnvironment(const std::string& name) const;
    bool DrawObstacle(const std::string& name) const;

    int GetFps() const { return fps_; }
    float GetDuration() const { return duration_; }

private:
    struct Motion {
        std::string type = "fixed";
        GL::Vec3 position;
        GL::Vec3 center;
        GL::Vec3 target;
        float radius = 0.0f;
        float radius_x = 0.0f;
        float radius_z = 0.0f;
        float height = 0.0f;
        float start_angle = 0.0f;
        float angular_speed = 0.0f;
        float yaw = 0.0f;
        std::string yaw_mode = "fixed";
        float slip_angle = 0.0f;
        float wheel_speed = 0.0f;
    };

    struct Shot {
        float start = 0.0f;
        float end = 0.0f;
        bool draw_car = true;
        Motion car;
        Motion camera;
        std::vector<std::string> environments;
        std::vector<std::string> obstacles;
    };

    const Shot& CurrentShot() const;
    static bool Contains(const std::vector<std::string>& names, const std::string& name);

    int fps_ = 30;
    float duration_ = 0.0f;
    std::vector<Shot> shots_;
    size_t current_shot_ = 0;
};

} // namespace App
