#pragma once

// STL
#include <algorithm>

// Sources
#include <model/model.hpp>
#include <transform/transform.hpp>
#include <accelerator/accelerator.hpp>

namespace App {

class CarModel: public Model {
public:
    CarModel(std::string car_name, std::string default_shader_name, std::string bbox_shader_name, std::string gltf, std::vector<std::string> car_wheel_mesh_names,
        float car_move_max_speed, float car_rotate_max_speed, float car_wheels_rotate_speed, GL::Vec3 car_center_translation, Transform transform)
        : Model(car_name, default_shader_name, bbox_shader_name, gltf, transform),
        car_move_max_speed_(car_move_max_speed),
        car_rotate_max_speed_(car_rotate_max_speed),
        car_wheels_rotate_speed_(car_wheels_rotate_speed),
        car_center_translation_(GL::Mat4{}.Translate(car_center_translation)),
        accelerator_(Accelerator{car_move_max_speed, APP_CAR_ACCEL}) {
        for (auto&& car_wheel_mesh_name : car_wheel_mesh_names) {
            auto found = std::find_if(meshes_.begin(), meshes_.end(), [car_wheel_mesh_name](App::Mesh mesh) {
                return mesh.name_ == car_wheel_mesh_name;
                });
            if (found != meshes_.end()) {
                car_wheel_meshes_indicies_.push_back(found - meshes_.begin());
                // found->SetDrawBBox(true);
            }
        }
    }

    CarModel(Config::CarModelConfig config)
        : CarModel(config.name, config.shader.default_shader_name, config.shader.bbox_shader_name,
            config.gltf, config.wheels.mesh_names, config.speed.move, config.speed.rotate,
            config.wheels.speed.rotate, config.rotation_center, config.transform) {}

    virtual GL::Mat4 GetModelMatrix() override {
        return static_cast<GL::Mat4>(transform_) * movement_transform_ * car_center_translation_;
    }

    void Move(float delta_time) {
        auto& context = App::Context::Get();
        if (context.keyboard_status.value()[GL::Key::W]) {
            switch (context.keyboard_mode.value()) {
                case App::KeyboardMode::ORBIT_CAMERA: {
                    context.camera->MoveFront(delta_time);
                    break;
                }
                case App::KeyboardMode::CAR_MOVEMENT: {
                    accelerator_.IncreaseSpeed(delta_time, true);
                    break;
                }
            }
        }

        if (context.keyboard_status.value()[GL::Key::S]) {
            switch (context.keyboard_mode.value()) {
                case App::KeyboardMode::ORBIT_CAMERA: {
                    context.camera->MoveBack(delta_time);
                    break;
                }
                case App::KeyboardMode::CAR_MOVEMENT: {
                    accelerator_.IncreaseSpeed(delta_time, false);
                    break;
                }
            }
        }

        if (context.keyboard_status.value()[GL::Key::A]) {
            switch (context.keyboard_mode.value()) {
                case App::KeyboardMode::ORBIT_CAMERA: {
                    context.camera->MoveLeft(delta_time);
                    break;
                }
                case App::KeyboardMode::CAR_MOVEMENT: {
                    RotateLeft(delta_time, accelerator_.GetSpeed() > 0.0);
                    break;
                }
            }
        }

        if (context.keyboard_status.value()[GL::Key::D]) {
            switch (context.keyboard_mode.value()) {
                case App::KeyboardMode::ORBIT_CAMERA: {
                    context.camera->MoveRight(delta_time);
                    break;
                }
                case App::KeyboardMode::CAR_MOVEMENT: {
                    RotateRight(delta_time, accelerator_.GetSpeed() > 0.0);
                    break;
                }
            }
        }

        MoveForward(delta_time);

        if (!context.keyboard_status.value()[GL::Key::W] && !context.keyboard_status.value()[GL::Key::S]) {
            accelerator_.DecreaseSpeed(delta_time);
        }
    }

    void SetDrawWheelsBBoxes(bool value) {
        for (auto index : car_wheel_meshes_indicies_) {
            meshes_[index].SetDrawBBox(value);
        }
    }

private:
    void RotateWheels(float delta_time) {
        for (auto index : car_wheel_meshes_indicies_) {
            meshes_[index].self_transform_.UpdateRotation(car_wheels_rotate_speed_ * delta_time, GL::Vec3(1.0f, 0.0f, 0.0f));
            meshes_[index].bbox_.UpdateVertices(meshes_[index].self_transform_);
        }
    }

    void MoveForward(float delta_time) {
        movement_transform_.Translate(GL::Vec3(0.0f, 0.0f, accelerator_.GetSpeed() * delta_time));
        RotateWheels(accelerator_.GetSpeed() * delta_time);
    }

    void RotateLeft(float delta_time, bool move_front) {
        if (accelerator_.GetSpeed() != 0.0) {
            float car_rotate_speed = car_rotate_max_speed_ * accelerator_.GetSpeed() / car_move_max_speed_;
            movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), car_rotate_speed * delta_time);
        }
    }

    void RotateRight(float delta_time, bool move_front) {
        if (accelerator_.GetSpeed() != 0.0) {
            float car_rotate_speed = car_rotate_max_speed_ * accelerator_.GetSpeed() / car_move_max_speed_;
            movement_transform_.Rotate(GL::Vec3(0.0f, 1.0f, 0.0f), -1.0f * car_rotate_speed * delta_time);
        }
    }

    float car_move_max_speed_;
    float car_rotate_max_speed_;
    float car_wheels_rotate_speed_;

    GL::Mat4 car_center_translation_;
    GL::Mat4 movement_transform_;

    std::vector<size_t> car_wheel_meshes_indicies_;

    Accelerator accelerator_;

    friend class Gui;
};

} // namespace App