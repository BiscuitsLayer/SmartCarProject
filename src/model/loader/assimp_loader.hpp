#pragma once

// STL
#include <map>
#include <exception>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace App {

class AssimpLoader {
public:
    AssimpLoader(GL::Program program, std::string& path) {
        directory_ = path.substr(0, path.find_last_of('/')) + '/';

        Assimp::Importer importer;
        // aiProcess_FlipUVs flips the texture coordinates on the y-axis <- necessary for OpenGL
        const aiScene *scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            throw std::runtime_error(importer.GetErrorString());
        }

        aiMatrix4x4 transformation;
        HandleNodeRecursive(program, scene->mRootNode, scene, transformation);
    }

    std::vector<Mesh> GetMeshes() const {
        return meshes_;
    }

private:
    std::vector<Texture> HandleMaterial(aiMaterial *mat, aiTextureType assimp_texture_type, Texture::Type app_texture_type) {
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

    void HandleMesh(GL::Program program, aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation) {
        std::vector<GL::Vertex> vertices;
        std::vector<int> indices;
        std::vector<Texture> textures;

        GL::Mat4 transform_to_model {
            transformation.a1, transformation.a2, transformation.a3, transformation.a4,
            transformation.b1, transformation.b2, transformation.b3, transformation.b4,
            transformation.c1, transformation.c2, transformation.c3, transformation.c4,
            transformation.d1, transformation.d2, transformation.d3, transformation.d4,
        };

        // Vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            GL::Vertex vertex;
            // process vertex positions, normals and texture coordinates
            vertex.Pos = GL::Vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};    

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
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Texture> diffuse_textures;
            diffuse_textures = HandleMaterial(material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
            textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());

            std::vector<Texture> specular_textures;
            specular_textures = HandleMaterial(material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
            textures.insert(textures.end(), specular_textures.begin(), specular_textures.end());
        }

        meshes_.push_back(Mesh{program, mesh->mName.C_Str(), Transform{transform_to_model}, vertices, indices, textures});
    }
    
    void HandleNodeRecursive(GL::Program program, aiNode* node, const aiScene* scene, aiMatrix4x4 transformation) {
        // accumulate from parent transformation
        transformation *= node->mTransformation;

        // process all the node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            HandleMesh(program, mesh, scene, transformation);			
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            HandleNodeRecursive(program, node->mChildren[i], scene, transformation);
        }
    }

    std::vector<Mesh> meshes_;
    std::map<std::string, Texture> paths_to_loaded_textures_;
    std::string directory_;
};

} // namespace App