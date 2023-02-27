#pragma once

// OpenGL Wrapper
#include <GL/Math/Mat4.hpp>

// Sources
#include <transform/transform.hpp>

namespace App {

const float APP_VECTOR_LENGTH_EPS = 1e-3;

class Camera {
public:
    Camera(GL::Vec3 camera_position, GL::Vec3 camera_target, float camera_move_speed, float camera_rotate_speed) : camera_position_(camera_position), 
    camera_target_(camera_target), camera_move_speed_(camera_move_speed), camera_rotate_speed_(camera_rotate_speed)
    {
        UpdateMatrix();
    }

    Camera() : Camera(GL::Vec3(0.0f, 0.0f, 3.0f), GL::Vec3(0.0f, 0.0f, 0.0f), 0.5f, 1.0f) {
    }

    GL::Mat4 GetViewMatrix() const {
        return view_matrix_;
    }

    void MoveFront(float delta_time) {
        float delta_coord = camera_move_speed_ * delta_time;
        camera_position_ += (-1.0f * reversed_camera_direction_) * delta_coord;
        UpdateMatrix();
    }

    void MoveBack(float delta_time) {
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
        reversed_camera_direction_ = (camera_position_ - camera_target_).Normal();

        world_space_up_ = GL::Vec3(0.0f, 1.0f, 0.0f);
        
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
        float length_to_target = (camera_position_ - camera_target_).Length();
        if (length_to_target > APP_VECTOR_LENGTH_EPS) {
            float align_step_front = (delta_coord * delta_coord) / (2 * length_to_target);
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
};

} // namespace App
