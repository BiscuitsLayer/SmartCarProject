#include "bbox.hpp"

// Incomplete type resolve
#include <helpers/helpers.hpp>

namespace App {

// Extern variables
extern const int APP_GL_VEC3_BYTESIZE;
extern const int APP_ZERO_OFFSET;
extern const int APP_GL_VEC3_BYTESIZE;
extern const int APP_GL_VEC3_COMPONENTS_COUNT;

BBox::BBox(const std::string& bbox_shader_name, const GL::Vec3& min, const GL::Vec3& max)
    : is_enabled_(false), init_min_(min), init_max_(max),
    init_vertices_(
        {
            GL::Vec3{init_min_.X, init_min_.Y, init_min_.Z},
            GL::Vec3{init_max_.X, init_min_.Y, init_min_.Z},
            GL::Vec3{init_max_.X, init_max_.Y, init_min_.Z},
            GL::Vec3{init_min_.X, init_max_.Y, init_min_.Z},
            GL::Vec3{init_min_.X, init_min_.Y, init_max_.Z},
            GL::Vec3{init_max_.X, init_min_.Y, init_max_.Z},
            GL::Vec3{init_max_.X, init_max_.Y, init_max_.Z},
            GL::Vec3{init_min_.X, init_max_.Y, init_max_.Z},
        }
        ),
    wireframe_indices_(
        {
            /*
                  4 -------- 7
                 /|         /|
                5 -------- 6 |
                | |        | |
                | 0 -------| 3
                |/         |/
                1 -------- 2
            */
            0, 1, 1, 2, 2, 3, 3, 0, // bottom edges
            4, 5, 5, 6, 6, 7, 7, 4, // top edges
            0, 4, 1, 5, 2, 6, 3, 7, // side edges
        }
        ),
    solid_indices_(
        {
            1, 0, 3, 3, 2, 1,
            5, 1, 2, 2, 6, 5,
            6, 2, 3, 3, 7, 6,
            4, 0, 1, 1, 5, 4,
            7, 3, 0, 0, 4, 7,
            4, 5, 6, 6, 7, 4
        }
        ),
    bbox_shader_name_(bbox_shader_name), cur_min_(min), cur_max_(max) {
    // Load vertices data
    vbo_ = GL::VertexBuffer(init_vertices_.data(), init_vertices_.size() * APP_GL_VEC3_BYTESIZE, GL::BufferUsage::DynamicDraw);

    // To draw in usual mode
    wireframe_ebo_ = GL::VertexBuffer(wireframe_indices_.data(), wireframe_indices_.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);
    wireframe_vao_.BindElements(wireframe_ebo_);

    // To draw on collision
    solid_ebo_ = GL::VertexBuffer(solid_indices_.data(), solid_indices_.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);
    solid_vao_.BindElements(solid_ebo_);

    // Set cur_min_ and cur_max_ variables
    UpdateVertices(Transform{});
}

void BBox::UpdateVertices(const Transform& mesh_self_transform) {
    // Initialize new min and max by default with 0-th vertex
    GL::Vec3 new_bbox_min = static_cast<GL::Mat4>(mesh_self_transform) * init_vertices_[0];
    GL::Vec3 new_bbox_max = static_cast<GL::Mat4>(mesh_self_transform) * init_vertices_[0];

    for (auto&& vertex : init_vertices_) {
        GL::Vec3 new_vertex = static_cast<GL::Mat4>(mesh_self_transform) * vertex;

        new_bbox_min = GL::Vec3{(std::min)(new_bbox_min.X, new_vertex.X), (std::min)(new_bbox_min.Y, new_vertex.Y), (std::min)(new_bbox_min.Z, new_vertex.Z)};
        new_bbox_max = GL::Vec3{(std::max)(new_bbox_max.X, new_vertex.X), (std::max)(new_bbox_max.Y, new_vertex.Y), (std::max)(new_bbox_max.Z, new_vertex.Z)};
    }

    // Set new BBox bounds
    cur_min_ = new_bbox_min;
    cur_max_ = new_bbox_max;

    // Set all new BBox vertices
    std::vector<GL::Vec3> new_vertices_ = {
        GL::Vec3{new_bbox_min.X, new_bbox_min.Y, new_bbox_min.Z},
        GL::Vec3{new_bbox_max.X, new_bbox_min.Y, new_bbox_min.Z},
        GL::Vec3{new_bbox_max.X, new_bbox_max.Y, new_bbox_min.Z},
        GL::Vec3{new_bbox_min.X, new_bbox_max.Y, new_bbox_min.Z},
        GL::Vec3{new_bbox_min.X, new_bbox_min.Y, new_bbox_max.Z},
        GL::Vec3{new_bbox_max.X, new_bbox_min.Y, new_bbox_max.Z},
        GL::Vec3{new_bbox_max.X, new_bbox_max.Y, new_bbox_max.Z},
        GL::Vec3{new_bbox_min.X, new_bbox_max.Y, new_bbox_max.Z},
    };

    // Update the buffer
    vbo_.SubData(new_vertices_.data(), APP_ZERO_OFFSET, new_vertices_.size() * APP_GL_VEC3_BYTESIZE);

    // Get program from shader handler
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();
    auto bbox_program = shader_handler.at(bbox_shader_name_);

    // Turn on
    gl.UseProgram(*bbox_program);

    // Set attributes for shader program
    wireframe_vao_.BindAttribute(bbox_program->GetAttribute("aWireframePos"), vbo_, GL::Type::Float, APP_GL_VEC3_COMPONENTS_COUNT, APP_GL_VEC3_BYTESIZE, APP_ZERO_OFFSET);
    solid_vao_.BindAttribute(bbox_program->GetAttribute("aSolidPos"), vbo_, GL::Type::Float, APP_GL_VEC3_COMPONENTS_COUNT, APP_GL_VEC3_BYTESIZE, APP_ZERO_OFFSET);
}

void BBox::Draw(const bool is_wireframe) const {
    // Get program from shader handler
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();
    auto bbox_program = shader_handler.at(bbox_shader_name_);

    // Turn on
    gl.UseProgram(*bbox_program);

    // Set drawing mode and draw
    bbox_program->SetUniform(bbox_program->GetUniform("isWireframe"), is_wireframe);
    if (is_wireframe) {
        gl.DrawElements(wireframe_vao_, GL::Primitive::Lines, APP_ZERO_OFFSET, wireframe_indices_.size(), GL::Type::UnsignedInt);
    } else {
        gl.DrawElements(solid_vao_, GL::Primitive::Triangles, APP_ZERO_OFFSET, solid_indices_.size(), GL::Type::UnsignedInt);
    }
}

const MemoryAlignedBBox BBox::GetMABB() const {
    return MemoryAlignedBBox{cur_min_, cur_max_, GL::Mat4{}, GL::Mat4{}};
}

} // namespace App