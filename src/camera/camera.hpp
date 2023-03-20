#pragma once

// OpenGL Wrapper
#include <GL/Math/Mat4.hpp>

// Sources
#include <constants/constants.hpp>
#include <transform/transform.hpp>
#include <config/config_handler.hpp>

namespace App {

class Camera {
public:
    Camera(GL::Vec3 camera_position, GL::Vec3 camera_target,
        float camera_move_speed, float camera_rotate_speed,
        float camera_min_length_to_target, float camera_max_length_to_target,
        bool camera_target_fixed_on_car, bool camera_position_fixed_behind_car, GL::Vec3 camera_translation_from_car);
    Camera(Config::CameraConfig config);

    // To make the class polymorphic, so we are able
    // to use down-casting with shared_ptr
    virtual ~Camera() {}
    GL::Mat4 GetViewMatrix() const;

    void MoveFront(float delta_time);
    void MoveBack(float delta_time);
    void MoveLeft(float delta_time);
    void MoveRight(float delta_time);

    void UpdateWithModel(GL::Mat4 target_model_matrix);

protected:
    void UpdateMatrix();
    void AlignStepFront(float delta_coord);

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

    Transform camera_translation_from_car_;

    bool camera_target_fixed_on_car_;
    bool camera_position_fixed_behind_car_;

    friend class Gui;
};

} // namespace App
