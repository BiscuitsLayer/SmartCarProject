#include "camera.hpp"

// Incomplete type resolve
#include <config/config_handler.hpp>

namespace App {

Camera::Camera(const GL::Vec3& position, const GL::Vec3& target, const float move_speed, const float rotate_speed,
    const float min_length_to_target, const float max_length_to_target, const bool is_target_fixed_on_car,
    const bool is_position_fixed_behind_car, const GL::Vec3& translation_from_car)
    : position_(position), target_(target), move_speed_(move_speed), rotate_speed_(rotate_speed),
    min_length_to_target_(min_length_to_target), max_length_to_target_(max_length_to_target),
    is_target_fixed_on_car_(is_target_fixed_on_car), is_position_fixed_behind_car_(is_position_fixed_behind_car),
    world_space_up_(APP_CAMERA_WORLD_SPACE_UP) {
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

GL::Mat4 Camera::GetViewMatrix() const {
    return view_matrix_;
}

void Camera::MoveFront(float delta_time) {
    cur_length_to_target_ = (position_ - target_).Length();

    if (cur_length_to_target_ < min_length_to_target_) {
        return;
    }

    float delta_coord = move_speed_ * delta_time;
    position_ += (-1.0f * reversed_direction_) * delta_coord;
    UpdateMatrix();
}

void Camera::MoveBack(float delta_time) {
    cur_length_to_target_ = (position_ - target_).Length();

    if (cur_length_to_target_ > max_length_to_target_) {
        return;
    }

    float delta_coord = move_speed_ * delta_time;
    position_ -= (-1.0f * reversed_direction_) * delta_coord;
    UpdateMatrix();
}

void Camera::MoveLeft(float delta_time) {
    float delta_coord = rotate_speed_ * delta_time;
    position_ -= right_ * delta_coord;
    UpdateMatrix();
    AlignStepFront(delta_coord);
}

void Camera::MoveRight(float delta_time) {
    float delta_coord = rotate_speed_ * delta_time;
    position_ += right_ * delta_coord;
    UpdateMatrix();
    AlignStepFront(delta_coord);
}

void Camera::UpdateWithModel(const GL::Mat4& target_model_matrix) {
    if (is_target_fixed_on_car_) {
        GL::Vec3 new_target = GetTranslation(target_model_matrix);
        target_ = new_target;
    }

    if (is_position_fixed_behind_car_) {
        GL::Vec3 new_position = GetTranslation(target_model_matrix * translation_from_car_);
        position_ = new_position;
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

void Camera::AlignStepFront(float delta_coord) {
    cur_length_to_target_ = (position_ - target_).Length();

    if (cur_length_to_target_ > APP_VECTOR_LENGTH_EPS) {
        // Taylor's series approximation
        float align_step_front = (delta_coord * delta_coord) / (2 * cur_length_to_target_);
        position_ += (-1.0f * reversed_direction_) * align_step_front;
        UpdateMatrix();
    }
}

} // namespace App