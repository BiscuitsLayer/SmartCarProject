#pragma once

// OpenGL Wrapper
#include <GL/Math/Mat4.hpp>

// Sources
#include <constants/constants.hpp>
#include <transform/transform.hpp>

namespace App {

class Camera {
public:
    Camera(GL::Vec3 camera_position, GL::Vec3 camera_target,
    float camera_move_speed, float camera_rotate_speed,
    float camera_min_length_to_target, float camera_max_length_to_target) 
    : camera_position_(camera_position), camera_target_(camera_target), 
    camera_move_speed_(camera_move_speed), camera_rotate_speed_(camera_rotate_speed),
    camera_min_length_to_target_(camera_min_length_to_target), camera_max_length_to_target_(camera_max_length_to_target) {
        cur_length_to_target_ = (camera_position_ - camera_target_).Length();
        if ((cur_length_to_target_ < camera_min_length_to_target_) || (cur_length_to_target_ > camera_max_length_to_target_)) {
            throw std::runtime_error("Camera error: current length to target is not between MIN and MAX values");
        }
        UpdateMatrix();
    }

    Camera()
    : Camera(APP_CAMERA_POSITION, APP_CAMERA_TARGET, 
    APP_CAMERA_MOVE_SPEED, APP_CAMERA_ROTATE_SPEED,
    APP_CAMERA_MIN_LENGTH_TO_TARGET, APP_CAMERA_MAX_LENGTH_TO_TARGET) {}

    GL::Mat4 GetViewMatrix() const {
        return view_matrix_;
    }

    void MoveFront(float delta_time) {
        cur_length_to_target_ = (camera_position_ - camera_target_).Length();
        if (cur_length_to_target_ < camera_min_length_to_target_) {
            return;
        }
        float delta_coord = camera_move_speed_ * delta_time;
        camera_position_ += (-1.0f * reversed_camera_direction_) * delta_coord;
        UpdateMatrix();
    }

    void MoveBack(float delta_time) {
        cur_length_to_target_ = (camera_position_ - camera_target_).Length();
        if (cur_length_to_target_ > camera_max_length_to_target_) {
            return;
        }
        float delta_coord = camera_move_speed_ * delta_time;
        camera_position_ -= (-1.0f * reversed_camera_direction_) * delta_coord;
        UpdateMatrix();
    }

    void MoveLeft(float delta_time) {
        float delta_coord = camera_rotate_speed_ * delta_time;
        camera_position_ -= camera_right_ * delta_coord;
        UpdateMatrix();
        AlignStepFront(delta_coord);
    }

    void MoveRight(float delta_time) {
        float delta_coord = camera_rotate_speed_ * delta_time;
        camera_position_ += camera_right_ * delta_coord;
        UpdateMatrix();
        AlignStepFront(delta_coord);
    }

private:
    void UpdateMatrix() {
        cur_length_to_target_ = (camera_position_ - camera_target_).Length();
        reversed_camera_direction_ = (camera_position_ - camera_target_).Normal();

        world_space_up_ = APP_CAMERA_WORLD_SPACE_UP;
        camera_right_ = world_space_up_.Cross(reversed_camera_direction_);

        if (camera_right_.Length() < APP_VECTOR_LENGTH_EPS) {
            world_space_up_ = GL::Vec3(1.0f, 0.0f, 0.0f);
            camera_right_ = world_space_up_.Cross(reversed_camera_direction_);
        }

        camera_right_ = camera_right_.Normal();
        camera_up_ = reversed_camera_direction_.Cross(camera_right_);
        view_matrix_ = GL::Mat4::LookAt(camera_position_, camera_target_, world_space_up_);
    }

    void AlignStepFront(float delta_coord) {
        cur_length_to_target_ = (camera_position_ - camera_target_).Length();
        if (cur_length_to_target_ > APP_VECTOR_LENGTH_EPS) {
            float align_step_front = (delta_coord * delta_coord) / (2 * cur_length_to_target_);
            camera_position_ += (-1.0f * reversed_camera_direction_) * align_step_front;
            UpdateMatrix();
        }
    }

    // the positive z-axis is going through your screen towards you
    GL::Vec3 camera_position_;
    GL::Vec3 camera_target_;
    GL::Vec3 reversed_camera_direction_;

    float camera_move_speed_;
    float camera_rotate_speed_;
    
    GL::Vec3 world_space_up_;

    GL::Vec3 camera_right_;
    GL::Vec3 camera_up_;

    GL::Mat4 view_matrix_;

    float cur_length_to_target_;
    float camera_min_length_to_target_;
    float camera_max_length_to_target_;

    friend class Gui;
};

} // namespace App
