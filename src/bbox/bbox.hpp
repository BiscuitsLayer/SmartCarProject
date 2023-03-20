#pragma once

// STL
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <constants/constants.hpp>
#include <helpers/helpers.hpp>
#include <transform/transform.hpp>

namespace App {

class BBox {
public:
    BBox(const std::string& bbox_shader_name, const GL::Vec3& min, const GL::Vec3& max);

    void UpdateVertices(const Transform& mesh_self_transform);
    void Draw();
    const MemoryAlignedBBox& GetMABB();

private:
    const GL::Vec3 init_min_;
    const GL::Vec3 init_max_;

    const std::vector<GL::Vertex> init_vertices_;
    const std::vector<unsigned int> indices_;

    const std::string bbox_shader_name_;

    GL::Vec3 cur_min_;
    GL::Vec3 cur_max_;

    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object
    GL::VertexBuffer ebo_;  // element buffer object
};

} // namespace App