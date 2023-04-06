#pragma once

// STL
#include <iostream>
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <mesh/mesh_fwd.hpp>

// LibSmartCar
#include <helpers/helpers.hpp>
#include <texture/texture.hpp>
#include <bbox/bbox.hpp>
#include <material/material.hpp>

namespace App {

class Mesh {
public:
    Mesh(std::string default_shader_name, std::string bbox_shader_name, std::string name, Transform transform_to_model,
        std::vector<GL::Vertex> vertices, std::vector<int> indices, Material material, BBox bbox);

    void SetDrawBBox(bool value);
    void Draw() const;
    MemoryAlignedBBox GetMABB() const;
    void DrawBBoxOnCollision() const;

    void MakeInstanced(const std::vector<Transform>& instance_transforms);

private:
    bool is_instanced_;
    std::optional<std::vector<Transform>> instance_transforms_;
    GL::VertexBuffer instance_transforms_vbo_;  // vertex buffer object

    std::string name_;
    Transform transform_to_model_;
    Transform self_transform_;

    BBox bbox_;

    std::vector<GL::Vertex> vertices_;
    std::vector<int> indices_;
    Material material_;

    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object
    GL::VertexBuffer ebo_;  // element buffer object

    std::string default_shader_name_;
    std::string bbox_shader_name_;

    friend class CarModel;
    friend class Gui;
};

} // namespace App