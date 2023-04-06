#pragma once

// STL
#include <algorithm>
#include <memory>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <car_model/car_model_fwd.hpp>

// LibSmartCar
#include <camera/camera.hpp>
#include <model/model.hpp>
#include <transform/transform.hpp>
#include <accelerator/accelerator.hpp>
#include <intersector/intersector.hpp>

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

    // For collision check
    void SetIntersector(Config::IntersectorConfig intersector_config) { intersector_ = std::make_shared<Intersector>(intersector_config); }
    std::shared_ptr<Intersector> GetIntersector() { return intersector_; }
    
    void Move(float delta_time);
    void SetDrawWheelsBBoxes(bool value);
    virtual std::vector<MemoryAlignedBBox> CollectMABB() const override;

private:
    void RotateWheels(float delta_time);
    void MoveForward(float delta_time);
    void RotateLeft(float delta_time, bool move_front);
    void RotateRight(float delta_time, bool move_front);

    void ClearPrecomputedMovementTransform();
    void UpdateMovementTransform();

    // For collision check
    const GL::Mat4 GetPrecomputedModelMatrix() const;
    std::shared_ptr<Intersector> intersector_;

    // Should be also kept there to compute rotation speed
    float move_max_speed_;
    float rotate_max_speed_;
    float wheels_rotate_speed_;
    std::vector<size_t> wheel_meshes_indicies_;

    const GL::Mat4 center_translation_;
    GL::Mat4 movement_transform_;

    // If there won't be any collisions after check, set it as the resulting movement
    GL::Mat4 precomputed_movement_transform_;

    Accelerator accelerator_;

    friend class Gui; // access to private variables
};

} // namespace App