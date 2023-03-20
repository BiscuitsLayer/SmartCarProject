#include "bbox.hpp"

namespace App {

BBox::BBox(std::string bbox_shader_name, GL::Vec3 min, GL::Vec3 max)
    : init_min_(min), init_max_(max), bbox_shader_name_(bbox_shader_name),
    init_vertices_(
        {
            GL::Vertex{GL::Vec3{init_min_.X, init_min_.Y, init_min_.Z}, GL::Vec2{}, GL::Vec3{}},
                GL::Vertex{GL::Vec3{init_max_.X, init_min_.Y, init_min_.Z}, GL::Vec2{}, GL::Vec3{}},
                GL::Vertex{GL::Vec3{init_max_.X, init_max_.Y, init_min_.Z}, GL::Vec2{}, GL::Vec3{}},
                GL::Vertex{GL::Vec3{init_min_.X, init_max_.Y, init_min_.Z}, GL::Vec2{}, GL::Vec3{}},
                GL::Vertex{GL::Vec3{init_min_.X, init_min_.Y, init_max_.Z}, GL::Vec2{}, GL::Vec3{}},
                GL::Vertex{GL::Vec3{init_max_.X, init_min_.Y, init_max_.Z}, GL::Vec2{}, GL::Vec3{}},
                GL::Vertex{GL::Vec3{init_max_.X, init_max_.Y, init_max_.Z}, GL::Vec2{}, GL::Vec3{}},
                GL::Vertex{GL::Vec3{init_min_.X, init_max_.Y, init_max_.Z}, GL::Vec2{}, GL::Vec3{}},
        }
        ),
    indices_(
        {
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            4, 5, 1, 1, 0, 4,
            3, 2, 6, 6, 7, 3,
        }
        ) {


    ebo_ = GL::VertexBuffer(indices_.data(), indices_.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);
    vao_.BindElements(ebo_);

    UpdateVertices(Transform{});
}

void BBox::UpdateVertices(Transform mesh_self_transform) {
    GL::Vec3 new_bbox_min;
    GL::Vec3 new_bbox_max;

    for (auto&& vertex : vertices_) {
        GL::Vec3 new_vertex = static_cast<GL::Mat4>(mesh_self_transform) * vertex.Pos;

        new_bbox_min = GL::Vec3{ std::min(new_bbox_min.X, new_vertex.X), std::min(new_bbox_min.Y, new_vertex.Y), std::min(new_bbox_min.Z, new_vertex.Z) };
        new_bbox_max = GL::Vec3{ std::max(new_bbox_max.X, new_vertex.X), std::max(new_bbox_max.Y, new_vertex.Y), std::max(new_bbox_max.Z, new_vertex.Z) };
    }

    cur_min_ = new_bbox_min;
    cur_max_ = new_bbox_max;

    std::vector<GL::Vertex> new_vertices_ = {
        GL::Vertex{GL::Vec3{new_bbox_min.X, new_bbox_min.Y, new_bbox_min.Z}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{new_bbox_max.X, new_bbox_min.Y, new_bbox_min.Z}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{new_bbox_max.X, new_bbox_max.Y, new_bbox_min.Z}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{new_bbox_min.X, new_bbox_max.Y, new_bbox_min.Z}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{new_bbox_min.X, new_bbox_min.Y, new_bbox_max.Z}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{new_bbox_max.X, new_bbox_min.Y, new_bbox_max.Z}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{new_bbox_max.X, new_bbox_max.Y, new_bbox_max.Z}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{new_bbox_min.X, new_bbox_max.Y, new_bbox_max.Z}, GL::Vec2{}, GL::Vec3{}},
    };

    vbo_ = GL::VertexBuffer(new_vertices_.data(), new_vertices_.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);

    auto& context = App::Context::Get();
    auto& gl = context.gl.value().get();
    auto shader_handler = context.shader_handler.value();

    auto bbox_program = shader_handler.at(bbox_shader_name_);
    gl.UseProgram(*bbox_program);

    vao_.BindAttribute(bbox_program->GetAttribute("aPos"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);
}

void BBox::Draw() {
    auto& context = App::Context::Get();
    auto& gl = context.gl.value().get();
    auto shader_handler = context.shader_handler.value();

    auto bbox_program = shader_handler.at(bbox_shader_name_);
    gl.UseProgram(*bbox_program);

    gl.DrawElements(vao_, GL::Primitive::Lines, 0, indices_.size(), GL::Type::UnsignedInt);
}

MemoryAlignedBBox BBox::GetMABB() {
    return MemoryAlignedBBox{ cur_min_, cur_max_, GL::Mat4{}, GL::Mat4{} };
}

} // namespace App