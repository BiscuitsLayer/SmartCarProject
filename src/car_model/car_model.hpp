#pragma once

// STL
#include <algorithm>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <car_model/car_model_fwd.hpp>

// LibSmartCar
#include <camera/camera.hpp>
#include <model/model.hpp>
#include <transform/transform.hpp>
#include <accelerator/accelerator.hpp>

namespace App {

class CarModel: public Model {
public:
    CarModel(const std::string& name, const std::string& default_shader_name,
        const std::string& bbox_shader_name, const std::string& gltf,
        const std::vector<std::string>& wheel_meshes_names, const float move_max_speed,
        const float acceleration, const float rotate_max_speed, const float wheels_rotate_speed,
        const GL::Vec3& center_translation, const Transform& transform);
    CarModel(const Config::CarModelConfig& config);

    virtual const GL::Mat4 GetModelMatrix() const override;
    void Move(float delta_time);
    void SetDrawWheelsBBoxes(bool value);

private:
    void RotateWheels(float delta_time);
    void MoveForward(float delta_time);
    void RotateLeft(float delta_time, bool move_front);
    void RotateRight(float delta_time, bool move_front);

    // Should be also kept there to compute rotation speed
    float move_max_speed_;
    float rotate_max_speed_;
    float wheels_rotate_speed_;
    std::vector<size_t> wheel_meshes_indicies_;

    const GL::Mat4 center_translation_;
    GL::Mat4 movement_transform_;

    Accelerator accelerator_;

    friend class Gui;
};

} // namespace App