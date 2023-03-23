#pragma once

// STL
#include <map>
#include <exception>
#include <algorithm>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <loader/loader_fwd.hpp>

// LibSmartCar
#include <bbox/bbox.hpp>
#include <mesh/mesh.hpp>
#include <texture/texture.hpp>

namespace App {

class AssimpLoader {
public:
    AssimpLoader(std::string default_shader_name, std::string bbox_shader_name, std::string& path);

    std::vector<Mesh> GetMeshes() const;

private:
    std::vector<Texture> HandleMaterial(aiMaterial* mat, aiTextureType assimp_texture_type, Texture::Type app_texture_type);
    void HandleMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation);
    void HandleNodeRecursive(aiNode* node, const aiScene* scene, aiMatrix4x4 transformation);

    std::vector<Mesh> meshes_;
    std::map<std::string, Texture> paths_to_loaded_textures_;
    std::string directory_;

    std::string default_shader_name_;
    std::string bbox_shader_name_;
};

} // namespace App