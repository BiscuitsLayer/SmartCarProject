#pragma once

// STL
#include <string>
#include <vector>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <model/model_fwd.hpp>

// LibSmartCar
#include <camera/camera.hpp>
#include <mesh/mesh.hpp>
#include <config/config_handler.hpp>
#include <loader/loader.hpp>

namespace App {

class Model {
public:
    Model(std::string model_name, std::string default_shader_name,
        std::string bbox_shader_name, std::string gltf, Transform transform);
    Model(Config::CommonModelConfig config);

    // To make the class polymorphic, so we are able
    // to use down-casting with shared_ptr
    virtual ~Model() {}

    void SetScale(GL::Vec3 scale);
    void UpdateScale(GL::Vec3 additional_scale);

    void SetRotation(float rotate_degrees, GL::Vec3 rotate_axis);
    void UpdateRotation(float additional_rotate_degrees, GL::Vec3 additional_rotate_axis);

    void SetTranslation(GL::Vec3 translation);
    void UpdateTranslation(GL::Vec3 additional_translation);

    virtual const GL::Mat4 GetModelMatrix() const;

    void SetDrawBBoxes(bool value);
    virtual void Draw();
    virtual std::vector<MemoryAlignedBBox> CollectMABB();

protected:
    std::string name_;
    std::vector<Mesh> meshes_;
    Transform transform_;

    std::string default_shader_name_;
    std::string bbox_shader_name_;

    friend class Gui;
};

} // namespace App