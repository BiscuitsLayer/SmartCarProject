#include "model.hpp"

namespace App {

// Extern variables
/* empty */

Model::Model(std::string model_name, std::string default_shader_name, std::string bbox_shader_name, std::string gltf, Transform transform)
    : name_(model_name), default_shader_name_(default_shader_name), bbox_shader_name_(bbox_shader_name),
    transform_(transform) {
    if (!gltf.empty()) {
        auto loader = AssimpLoader{default_shader_name, bbox_shader_name, gltf};
        meshes_ = loader.GetMeshes();
    }
}

Model::Model(Config::CommonModelConfig config)
    : Model(config.name, config.shader.default_shader_name, config.shader.bbox_shader_name, config.gltf, config.transform) {}

void Model::SetScale(GL::Vec3 scale) {
    transform_.SetScale(scale);
}

void Model::UpdateScale(GL::Vec3 additional_scale) {
    transform_.UpdateScale(additional_scale);
}

void Model::SetRotation(float rotate_degrees, GL::Vec3 rotate_axis) {
    transform_.SetRotation(rotate_degrees, rotate_axis);
}

void Model::UpdateRotation(float additional_rotate_degrees, GL::Vec3 additional_rotate_axis) {
    transform_.UpdateRotation(additional_rotate_degrees, additional_rotate_axis);
}

void Model::SetTranslation(GL::Vec3 translation) {
    transform_.SetTranslation(translation);
}

void Model::UpdateTranslation(GL::Vec3 additional_translation) {
    transform_.UpdateTranslation(additional_translation);
}

const GL::Mat4 Model::GetModelMatrix() const {
    return transform_;
}

void Model::SetDrawBBoxes(bool value) {
    for (auto&& mesh : meshes_) {
        mesh.SetDrawBBox(value);
    }
}

void Model::Draw() {
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto program = shader_handler.at(default_shader_name_);
    gl.UseProgram(*program);
    program->SetUniform(program->GetUniform("aMatrices.modelMatrix"), GetModelMatrix());
    program->SetUniform(program->GetUniform("aMatrices.viewMatrix"), context.camera->GetViewMatrix());
    program->SetUniform(program->GetUniform("aMatrices.projectionMatrix"), context.projection_matrix.value());

    program->SetUniform(program->GetUniform("cameraPosition"), context.camera->GetPosition());

    auto bbox_program = shader_handler.at(bbox_shader_name_);
    gl.UseProgram(*bbox_program);
    program->SetUniform(program->GetUniform("aMatrices.modelMatrix"), GetModelMatrix());
    program->SetUniform(program->GetUniform("aMatrices.viewMatrix"), context.camera->GetViewMatrix());
    program->SetUniform(program->GetUniform("aMatrices.projectionMatrix"), context.projection_matrix.value());

    for (auto&& mesh : meshes_) {
        mesh.Draw();
    }
}

std::vector<MemoryAlignedBBox> Model::CollectMABB() const {
    std::vector<MemoryAlignedBBox> result;
    result.reserve(meshes_.size());

    for (auto&& mesh : meshes_) {
        auto mabb = mesh.GetMABB();
        mabb.model = GetModelMatrix();
        result.push_back(mabb);
    }
    return result;
}

} // namespace App