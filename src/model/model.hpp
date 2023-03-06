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

    void Draw(GL::Context &gl, GL::Program &program) {
        for (auto&& mesh : meshes_) {
            mesh.Draw(gl, program);
        }
    }

private:
    std::vector<Mesh> meshes_;
};

} // namespace App