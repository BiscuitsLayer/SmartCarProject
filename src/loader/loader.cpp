#include "loader.hpp"

namespace App {

AssimpLoader::AssimpLoader(std::string default_shader_name, std::string bbox_shader_name, std::string& path)
    : default_shader_name_(default_shader_name), bbox_shader_name_(bbox_shader_name) {
    directory_ = path.substr(0, path.find_last_of('/')) + '/';

    Assimp::Importer importer;
    // aiProcess_FlipUVs flips the texture coordinates on the y-axis <- necessary for OpenGL
    const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error(importer.GetErrorString());
    }

    aiMatrix4x4 transformation;
    HandleNodeRecursive(scene->mRootNode, scene, transformation);
}

std::vector<Mesh> AssimpLoader::GetMeshes() const {
    return meshes_;
}

std::vector<Texture> AssimpLoader::HandleMaterial(aiMaterial* mat, aiTextureType assimp_texture_type, Texture::Type app_texture_type) {
    std::vector<Texture> ans;
    for (unsigned int i = 0; i < mat->GetTextureCount(assimp_texture_type); ++i) {
        aiString texture_filename;
        mat->GetTexture(assimp_texture_type, i, &texture_filename);
        std::string full_path{directory_ + texture_filename.C_Str()};

        auto found = paths_to_loaded_textures_.find(full_path);
        if (found != paths_to_loaded_textures_.end()) {
            ans.push_back(found->second);
        } else {
            Texture new_texture{full_path, app_texture_type};
            paths_to_loaded_textures_.insert(std::make_pair(full_path, new_texture));
            ans.push_back(new_texture);
        }
    }
    return ans;
}

void AssimpLoader::HandleMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation) {
    std::vector<GL::Vertex> vertices;
    std::vector<int> indices;
    std::vector<Texture> textures;

    GL::Mat4 transform_to_model {
        transformation.a1, transformation.a2, transformation.a3, transformation.a4,
            transformation.b1, transformation.b2, transformation.b3, transformation.b4,
            transformation.c1, transformation.c2, transformation.c3, transformation.c4,
            transformation.d1, transformation.d2, transformation.d3, transformation.d4,
    };

    GL::Vec3 bbox_min;
    GL::Vec3 bbox_max;

    // Vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        GL::Vertex vertex;
        // process vertex positions, normals and texture coordinates
        vertex.Pos = GL::Vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        bbox_min = GL::Vec3{(std::min)(bbox_min.X, vertex.Pos.X), (std::min)(bbox_min.Y, vertex.Pos.Y), (std::min)(bbox_min.Z, vertex.Pos.Z)};
        bbox_max = GL::Vec3{(std::max)(bbox_max.X, vertex.Pos.X), (std::max)(bbox_max.Y, vertex.Pos.Y), (std::max)(bbox_max.Z, vertex.Pos.Z)};

        if (mesh->HasTextureCoords(0)) {
            vertex.Tex = GL::Vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.Tex = GL::Vec2{0.0f, 0.0f};
        }
        vertex.Normal = GL::Vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        vertices.push_back(vertex);
    }

    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Materials
    // WARNING: don't compare like mesh->mMaterialIndex > -1, because of unsigned comparison
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuse_textures;
        diffuse_textures = HandleMaterial(material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
        textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());

        std::vector<Texture> specular_textures;
        specular_textures = HandleMaterial(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
        textures.insert(textures.end(), specular_textures.begin(), specular_textures.end());
    }

    BBox bbox{bbox_shader_name_, bbox_min, bbox_max};
    Mesh new_mesh{default_shader_name_, bbox_shader_name_, mesh->mName.C_Str(), Transform{transform_to_model}, vertices, indices, textures, bbox};
    meshes_.push_back(new_mesh);
}

void AssimpLoader::HandleNodeRecursive(aiNode* node, const aiScene* scene, aiMatrix4x4 transformation) {
    // accumulate from parent transformation
    transformation *= node->mTransformation;

    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        HandleMesh(mesh, scene, transformation);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        HandleNodeRecursive(node->mChildren[i], scene, transformation);
    }
}

} // namespace App