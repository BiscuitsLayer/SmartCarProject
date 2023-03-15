#pragma once

// STL
#include <algorithm>

// Sources
#include <model/model.hpp>
#include <transform/transform.hpp>

namespace App {

class CarModel: public Model {
public:
    CarModel(std::string default_shader_name, std::string bbox_shader_name, std::string gltf, std::vector<std::string> car_wheel_mesh_names,
        float car_move_speed, float car_rotate_speed, float car_wheels_rotate_speed, GL::Vec3 car_center_translation, Transform transform)
        : Model(default_shader_name, bbox_shader_name, gltf, transform),
        car_move_speed_(car_move_speed), car_rotate_speed_(car_rotate_speed),
        car_wheels_rotate_speed_(car_wheels_rotate_speed),
        car_center_translation_(GL::Mat4{}.Translate(car_center_translation)) {
        for (auto&& car_wheel_mesh_name : car_wheel_mesh_names) {
            auto found = std::find_if(meshes_.begin(), meshes_.end(), [car_wheel_mesh_name](App::Mesh mesh) {
                return mesh.name_ == car_wheel_mesh_name;
                });
            if (found != meshes_.end()) {
                car_wheel_meshes_indicies_.push_back(found - meshes_.begin());
            }
        }
    }

    CarModel(Config::CarModelConfig config)
        : CarModel(config.shader.default_shader_name, config.shader.bbox_shader_name,
            config.gltf, config.wheels.mesh_names, config.speed.move, config.speed.rotate,
            config.wheels.speed.rotate, config.rotation_center, config.transform) {}

    void MoveFront(float delta_time) {
        movement_transform_.Translate(GL::Vec3(0.0f, 0.0f, car_move_speed_ * delta_time));
        RotateWheels(delta_time);
    }

    void MoveFrontLeft(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), car_rotate_speed_ * delta_time);
        MoveFront(delta_time);
    }

    void MoveFrontRight(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), -1.0f * car_rotate_speed_ * delta_time);
        MoveFront(delta_time);
    }

    void MoveBack(float delta_time) {
        movement_transform_.Translate(GL::Vec3(0.0f, 0.0f, -1.0f * car_move_speed_ * delta_time));
        RotateWheels(-1.0f * delta_time);
    }

    void MoveBackLeft(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), -1.0f * car_rotate_speed_ * delta_time);
        MoveBack(delta_time);
    }

    void MoveBackRight(float delta_time) {
        movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), car_rotate_speed_ * delta_time);
        MoveBack(delta_time);
    }

    virtual GL::Mat4 GetModelMatrix() override {
        return static_cast<GL::Mat4>(transform_) * movement_transform_ * car_center_translation_;
    }

private:
    void RotateWheels(float delta_time) {
        for (auto index : car_wheel_meshes_indicies_) {
            meshes_[index].self_transform_.UpdateRotation(car_wheels_rotate_speed_ * delta_time, GL::Vec3(1.0f, 0.0f, 0.0f));
        }
    }

    float car_move_speed_;
    float car_rotate_speed_;
    float car_wheels_rotate_speed_;

    GL::Mat4 car_center_translation_;
    GL::Mat4 movement_transform_;

    std::vector<size_t> car_wheel_meshes_indicies_;

    friend class Gui;
};

} // namespace App