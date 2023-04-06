#include "mesh.hpp"

namespace App {

// Extern variables
extern const int APP_GL_VERTEX_BYTESIZE;
extern const int APP_GL_VERTEX_POS_OFFSET;
extern const int APP_GL_VERTEX_TEX_OFFSET;
extern const int APP_GL_VERTEX_NORMAL_OFFSET;
extern const int APP_GL_VERTEX_TANGENT_OFFSET;
extern const int APP_GL_VEC2_COMPONENTS_COUNT;
extern const int APP_GL_VEC2_BYTESIZE;
extern const int APP_GL_VEC3_COMPONENTS_COUNT;
extern const int APP_GL_VEC3_BYTESIZE;

Mesh::Mesh(std::string default_shader_name, std::string bbox_shader_name, std::string name, Transform transform_to_model, std::vector<GL::Vertex> vertices, std::vector<int> indices, Material material, BBox bbox)
    : default_shader_name_(default_shader_name), bbox_shader_name_(bbox_shader_name), name_(name), transform_to_model_(transform_to_model), vertices_(vertices), indices_(indices), material_(material), bbox_(bbox),
    is_instanced_(false) {
    vbo_ = GL::VertexBuffer(vertices.data(), vertices.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);
    ebo_ = GL::VertexBuffer(indices.data(), indices.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);

    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto program = shader_handler.at(default_shader_name_);
    gl.UseProgram(*program);

    vao_.BindAttribute(program->GetAttribute("aPos"), vbo_, GL::Type::Float, APP_GL_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);
    vao_.BindAttribute(program->GetAttribute("aTexCoord"), vbo_, GL::Type::Float, APP_GL_VEC2_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_TEX_OFFSET);
    vao_.BindAttribute(program->GetAttribute("aNormal"), vbo_, GL::Type::Float, APP_GL_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_NORMAL_OFFSET);
    vao_.BindAttribute(program->GetAttribute("aTangent"), vbo_, GL::Type::Float, APP_GL_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_TANGENT_OFFSET);
    vao_.BindElements(ebo_);
}

void Mesh::SetDrawBBox(bool value) {
    value ? bbox_.Enable() : bbox_.Disable();
}

void Mesh::Draw() const {
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto program = shader_handler.at(default_shader_name_);
    gl.UseProgram(*program);

    material_.BindTextures(program);

    program->SetUniform(program->GetUniform("aMatrices.meshTransformToModelMatrix"), transform_to_model_);
    program->SetUniform(program->GetUniform("aMatrices.meshSelfTransformMatrix"), self_transform_);

    if (is_instanced_) {
        gl.DrawElementsInstanced(vao_, GL::Primitive::Triangles, 0, indices_.size(), GL::Type::UnsignedInt, instance_transforms_->size());
    } else {
        gl.DrawElements(vao_, GL::Primitive::Triangles, 0, indices_.size(), GL::Type::UnsignedInt);
    }

    if (bbox_.IsEnabled()) {
        auto bbox_program = shader_handler.at(bbox_shader_name_);
        gl.UseProgram(*bbox_program);

        bbox_program->SetUniform(bbox_program->GetUniform("aMatrices.meshTransformToModelMatrix"), transform_to_model_);
        bbox_.Draw(true);
    }
}

MemoryAlignedBBox Mesh::GetMABB() const {
    auto mabb = bbox_.GetMABB();
    mabb.mesh_to_model = static_cast<GL::Mat4>(transform_to_model_);
    return mabb;
}

void Mesh::DrawBBoxOnCollision() const {
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto bbox_program = shader_handler.at(bbox_shader_name_);
    gl.UseProgram(*bbox_program);

    bbox_program->SetUniform(bbox_program->GetUniform("aMatrices.meshTransformToModelMatrix"), transform_to_model_);
    bbox_.Draw(false);
}

void Mesh::MakeInstanced(const std::vector<Transform>& instance_transforms) {
    is_instanced_ = true;
    instance_transforms_ = instance_transforms;

    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto program = shader_handler.at(default_shader_name_);
    gl.UseProgram(*program);

    std::vector<GL::Mat4> raw_transforms;
    for (auto transform : instance_transforms) {
        raw_transforms.push_back(transform);
    }
    instance_transforms_vbo_ = GL::VertexBuffer(raw_transforms.data(), raw_transforms.size() * APP_GL_MAT4_BYTESIZE, GL::BufferUsage::StaticDraw);

    auto pos = program->GetAttribute("aInstanceMatrix");
    auto pos1 = pos + 0;
    auto pos2 = pos + 1;
    auto pos3 = pos + 2;
    auto pos4 = pos + 3;

    vao_.BindAttribute(pos1, vbo_, GL::Type::Float, APP_GL_VEC4_COMPONENTS_COUNT, APP_GL_MAT4_BYTESIZE, APP_GL_VEC4_BYTESIZE * 0);
    vao_.BindAttribute(pos2, vbo_, GL::Type::Float, APP_GL_VEC4_COMPONENTS_COUNT, APP_GL_MAT4_BYTESIZE, APP_GL_VEC4_BYTESIZE * 1);
    vao_.BindAttribute(pos3, vbo_, GL::Type::Float, APP_GL_VEC4_COMPONENTS_COUNT, APP_GL_MAT4_BYTESIZE, APP_GL_VEC4_BYTESIZE * 2);
    vao_.BindAttribute(pos4, vbo_, GL::Type::Float, APP_GL_VEC4_COMPONENTS_COUNT, APP_GL_MAT4_BYTESIZE, APP_GL_VEC4_BYTESIZE * 3);

    vao_.BindAttributeDivisor(pos1, vbo_, 1);
    vao_.BindAttributeDivisor(pos2, vbo_, 1);
    vao_.BindAttributeDivisor(pos3, vbo_, 1);
    vao_.BindAttributeDivisor(pos4, vbo_, 1);
}

// TODO: update instanced <- update transform (NOW CANNOT BE UPDATED)

} // namespace App
