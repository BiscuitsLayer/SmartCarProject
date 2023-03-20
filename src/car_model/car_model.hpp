#pragma once

// STL
#include <algorithm>

// Sources
#include <camera/camera.hpp>
#include <model/model.hpp>
#include <transform/transform.hpp>
#include <accelerator/accelerator.hpp>

namespace App {

class CarModel: public Model {
public:
    CarModel(std::string car_name, std::string default_shader_name, std::string bbox_shader_name, std::string gltf, std::vector<std::string> car_wheel_mesh_names,
        float car_move_max_speed, float car_rotate_max_speed, float car_wheels_rotate_speed, GL::Vec3 car_center_translation, Transform transform);
    CarModel(Config::CarModelConfig config);

    virtual GL::Mat4 GetModelMatrix() override;
    void Move(float delta_time);
    void SetDrawWheelsBBoxes(bool value);

private:
    void RotateWheels(float delta_time);
    void MoveForward(float delta_time);
    void RotateLeft(float delta_time, bool move_front);
    void RotateRight(float delta_time, bool move_front);

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