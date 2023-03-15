#pragma once

// STL
#include <string>
#include <vector>

// Sources
#include <mesh/mesh.hpp>
#include <config/config_handler.hpp>
#include <model/loader/assimp_loader.hpp>

namespace App {

class Model {
public:
    Model(std::string default_shader_name, std::string bbox_shader_name, std::string gltf, Transform transform)
        : default_shader_name_(default_shader_name), bbox_shader_name_(bbox_shader_name),
        transform_(transform) {
        if (!gltf.empty()) {
            auto loader = AssimpLoader{ default_shader_name, bbox_shader_name, gltf };
            meshes_ = loader.GetMeshes();
        }
    }

    Model(Config::CommonModelConfig config)
        : Model(config.shader.default_shader_name, config.shader.bbox_shader_name, config.gltf, config.transform) {}

    // To make the class polymorphic, so we are able
    // to use down-casting with shared_ptr
    virtual ~Model() {}

    void SetScale(GL::Vec3 scale) {
        transform_.SetScale(scale);
    }

    void UpdateScale(GL::Vec3 additional_scale) {
        transform_.UpdateScale(additional_scale);
    }

    void SetRotation(float rotate_degrees, GL::Vec3 rotate_axis) {
        transform_.SetRotation(rotate_degrees, rotate_axis);
    }

    void UpdateRotation(float additional_rotate_degrees, GL::Vec3 additional_rotate_axis) {
        transform_.UpdateRotation(additional_rotate_degrees, additional_rotate_axis);
    }

    void SetTranslation(GL::Vec3 translation) {
        transform_.SetTranslation(translation);
    }

    void UpdateTranslation(GL::Vec3 additional_translation) {
        transform_.UpdateTranslation(additional_translation);
    }

    virtual GL::Mat4 GetModelMatrix() {
        return transform_;
    }

    virtual void Draw() {
        auto& context = App::Context::Get();
        auto& gl = context.gl.value().get();
        auto shader_handler = context.shader_handler.value();

        GL::Mat4 mvp = App::GetModelViewProjectionMatrix(GetModelMatrix(), context.camera.value()->GetViewMatrix(), context.projection_matrix.value());

        auto program = shader_handler.at(default_shader_name_);
        gl.UseProgram(*program);
        program->SetUniform(program->GetUniform("MVP"), mvp);

        auto bbox_program = shader_handler.at(bbox_shader_name_);
        gl.UseProgram(*bbox_program);
        bbox_program->SetUniform(bbox_program->GetUniform("MVP"), mvp);

        for (auto&& mesh : meshes_) {
            mesh.Draw();
        }
    }

protected:
    std::vector<Mesh> meshes_;
    Transform transform_;

    std::string default_shader_name_;
    std::string bbox_shader_name_;
};

} // namespace App