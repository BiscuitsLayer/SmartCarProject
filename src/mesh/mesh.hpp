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

namespace App {

class Mesh {
public:
    Mesh(std::string default_shader_name, std::string bbox_shader_name, std::string name, Transform transform_to_model,
        std::vector<GL::Vertex> vertices, std::vector<int> indices, std::vector<Texture> textures, BBox bbox);

    void SetDrawBBox(bool value);
    void Draw();
    MemoryAlignedBBox GetMABB();

private:
    std::string name_;
    Transform transform_to_model_;
    Transform self_transform_;

    bool draw_bbox_;
    BBox bbox_;

    std::vector<GL::Vertex> vertices_;
    std::vector<int> indices_;
    std::vector<Texture> textures_;

    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object
    GL::VertexBuffer ebo_;  // element buffer object

    std::string default_shader_name_;
    std::string bbox_shader_name_;

    friend class CarModel;
    friend class Gui;
};

} // namespace App