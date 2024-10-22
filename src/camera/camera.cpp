#include "camera.hpp"

// Incomplete type resolve
#include <config/config_handler.hpp>

namespace App {

// Extern variables
extern const GL::Vec3 APP_CAMERA_WORLD_SPACE_UP;
extern const float APP_VECTOR_LENGTH_EPS;
extern const GL::Vec3 APP_CAMERA_RESERVE_WORLD_SPACE_UP;

Camera::Camera(const GL::Vec3& position, const GL::Vec3& target, const float move_speed, const float rotate_speed,
    const float min_length_to_target, const float max_length_to_target, const bool is_target_fixed_on_car,
    const bool is_position_fixed_behind_car, const GL::Vec3& translation_from_car)
    : position_(position), target_(target), reversed_direction_({}), 
    move_speed_(move_speed), rotate_speed_(rotate_speed),
    world_space_up_(APP_CAMERA_WORLD_SPACE_UP), right_({}), up_({}),
    view_matrix_({}),
    cur_length_to_target_(0.0f), min_length_to_target_(min_length_to_target), max_length_to_target_(max_length_to_target),
    translation_from_car_(App::Transform{}),
    is_target_fixed_on_car_(is_target_fixed_on_car), is_position_fixed_behind_car_(is_position_fixed_behind_car), reached_final_position_(false) {
    translation_from_car_.SetTranslation(translation_from_car);
    cur_length_to_target_ = (position_ - target_).Length();

    if ((cur_length_to_target_ < min_length_to_target_) || (cur_length_to_target_ > max_length_to_target_)) {
        throw std::runtime_error("Camera error: current length to target is not between MIN and MAX values");
    }

    UpdateMatrix();
}

Camera::Camera(const Config::CameraConfig& config)
    : Camera(config.position.value, config.target.value, config.speed.move, config.speed.rotate,
        config.length_to_target.min, config.length_to_target.max, config.target.fixed_on_car, config.position.fixed_behind_car,
        config.position.translation_from_car) {}

void Camera::Move(const float delta_time) {
    auto& context = App::Context::Get();
    if (context.keyboard_mode.value() == App::KeyboardMode::ORBIT_CAMERA) {
        if (context.keyboard_status.value()[GL::Key::W]) {
            context.camera->MoveFront(delta_time);
        }
        if (context.keyboard_status.value()[GL::Key::S]) {
            context.camera->MoveBack(delta_time);
        }
        if (context.keyboard_status.value()[GL::Key::A]) {
            context.camera->MoveLeft(delta_time);
        }
        if (context.keyboard_status.value()[GL::Key::D]) {
            context.camera->MoveRight(delta_time);
        }
    }
}

void Camera::UpdateWithModel(const GL::Mat4& model_matrix) {
    reached_final_position_ = true;

    if (is_target_fixed_on_car_) {
        GL::Vec3 new_target = GetTranslation(model_matrix);

        GL::Vec3 delta_target = target_ - new_target;
        if (delta_target.Dot(delta_target) > APP_VECTOR_LENGTH_EPS) {
            // Smooth movement
            target_ = (9 * target_ + new_target) / 10;
            reached_final_position_ = false;
        }
    }

    if (is_position_fixed_behind_car_) {
        GL::Vec3 new_position = GetTranslation(model_matrix * translation_from_car_);

        GL::Vec3 delta_position = position_- new_position;
        if (delta_position.Dot(delta_position) > APP_VECTOR_LENGTH_EPS) {
            // Smooth movement
            position_ = (9 * position_ + new_position) / 10;
            reached_final_position_ = false;
        }
    }
}

void Camera::UpdateMatrix() {
    cur_length_to_target_ = (position_ - target_).Length();
    reversed_direction_ = (position_ - target_).Normal();

    right_ = world_space_up_.Cross(reversed_direction_);

    if (right_.Length() < APP_VECTOR_LENGTH_EPS) {
        world_space_up_ = APP_CAMERA_RESERVE_WORLD_SPACE_UP;
        right_ = world_space_up_.Cross(reversed_direction_);
    }

    right_ = right_.Normal();
    up_ = reversed_direction_.Cross(right_);
    view_matrix_ = GL::Mat4::LookAt(position_, target_, world_space_up_);
}

void Camera::MoveFront(const float delta_time) {
    cur_length_to_target_ = (position_ - target_).Length();

    if (cur_length_to_target_ < min_length_to_target_) {
        return;
    }

    float delta_coord = move_speed_ * delta_time;
    position_ += (-1.0f * reversed_direction_) * delta_coord;
    UpdateMatrix();
}

void Camera::MoveBack(const float delta_time) {
    cur_length_to_target_ = (position_ - target_).Length();

    if (cur_length_to_target_ > max_length_to_target_) {
        return;
    }

    float delta_coord = move_speed_ * delta_time;
    position_ -= (-1.0f * reversed_direction_) * delta_coord;
    UpdateMatrix();
}

void Camera::MoveLeft(const float delta_time) {
    float delta_coord = rotate_speed_ * delta_time;
    position_ -= right_ * delta_coord;
    UpdateMatrix();
    AlignStepFront(delta_coord);
}

void Camera::MoveRight(const float delta_time) {
    float delta_coord = rotate_speed_ * delta_time;
    position_ += right_ * delta_coord;
    UpdateMatrix();
    AlignStepFront(delta_coord);
}

void Camera::AlignStepFront(const float delta_coord) {
    cur_length_to_target_ = (position_ - target_).Length();

    if (cur_length_to_target_ > APP_VECTOR_LENGTH_EPS) {
        // Taylor's series approximation
        float align_step_front = (delta_coord * delta_coord) / (2 * cur_length_to_target_);
        position_ += (-1.0f * reversed_direction_) * align_step_front;
        UpdateMatrix();
    }
}

} // namespace App