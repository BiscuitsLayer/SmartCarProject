#pragma once

// STL
#include <string>
#include <vector>

// Sources
#include <mesh/mesh.hpp>
#include <model/loader/assimp_loader.hpp>
#include <model/loader/tinygltf_loader.hpp>

namespace App {

class Model {
public:
    Model(GL::Program program, std::string path) {
        auto loader = AssimpLoader{program, path};
        // auto loader = TinyGltfLoader(path);
        meshes_ = loader.GetMeshes();
    }

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


    GL::Mat4 GetModelMatrix() {
        return transform_;
    }

    void Draw(GL::Context &gl, GL::Program &program) {
        for (auto&& mesh : meshes_) {
            mesh.Draw(gl, program);
        }
    }

private:
    std::vector<Mesh> meshes_;
    Transform transform_;
};

} // namespace App