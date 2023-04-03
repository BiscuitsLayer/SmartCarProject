#pragma once

// STL
#include <vector>
#include <algorithm>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <bbox/bbox_fwd.hpp>

// LibSmartCar
#include <transform/transform.hpp>

// Incomplete type resolve
#include <helpers/helpers_fwd.hpp>

namespace App {

class BBox {
public:
    BBox(const std::string& bbox_shader_name, const GL::Vec3& min, const GL::Vec3& max);

    void UpdateVertices(const Transform& mesh_self_transform);
    void Draw() const;
    const MemoryAlignedBBox GetMABB() const;
    void DrawOnCollision() const;

private:
    const GL::Vec3 init_min_;
    const GL::Vec3 init_max_;

    const std::vector<GL::Vec3> init_vertices_;
    const std::vector<unsigned int> wireframe_indices_;
    const std::vector<unsigned int> solid_indices_;


    const std::string bbox_shader_name_;

    GL::Vec3 cur_min_;
    GL::Vec3 cur_max_;

    GL::VertexBuffer vbo_;  // vertex buffer object

    GL::VertexArray  wireframe_vao_;  // vertex array object
    GL::VertexBuffer wireframe_ebo_;  // element buffer object

    GL::VertexArray  solid_vao_;  // vertex array object
    GL::VertexBuffer solid_ebo_;  // element buffer object    
};

} // namespace App