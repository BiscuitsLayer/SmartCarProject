#include "loader.hpp"

namespace App {

// Extern variables
extern const AssimpMaterialTextureParameters APP_ASSIMP_BASE_COLOR_TEXTURE_PARAMETERS;
extern const AssimpMaterialColorParameters APP_ASSIMP_BASE_COLOR_FACTOR_PARAMETERS;
extern const AssimpMaterialTextureParameters APP_ASSIMP_METALLIC_ROUGHNESS_TEXTURE_PARAMETERS;
extern const AssimpMaterialFloatParameters APP_ASSIMP_METALLIC_FACTOR_PARAMETERS;
extern const AssimpMaterialFloatParameters APP_ASSIMP_ROUGHNESS_FACTOR_PARAMETERS;
extern const AssimpMaterialTextureParameters APP_ASSIMP_NORMAL_TEXTURE_PARAMETERS;

AssimpLoader::AssimpLoader(std::string default_shader_name, std::string bbox_shader_name, std::string& path)
    : default_shader_name_(default_shader_name), bbox_shader_name_(bbox_shader_name) {
    directory_ = GetFolderFromPath(path);

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

AssimpMaterialTextureParameters AssimpLoader::GetAssimpTextureParameters(Material::ParameterType parameter_type) {
    switch (parameter_type) {
        case Material::ParameterType::BASE_COLOR: {
            return APP_ASSIMP_BASE_COLOR_TEXTURE_PARAMETERS;
            break;
        }
        case Material::ParameterType::METALLIC_ROUGHNESS: {
            return APP_ASSIMP_METALLIC_ROUGHNESS_TEXTURE_PARAMETERS;
            break;
        }
        case Material::ParameterType::NORMAL: {
            return APP_ASSIMP_NORMAL_TEXTURE_PARAMETERS;
            break;
        }
        default: {
            throw std::runtime_error("AssimpLoader: texture parameters not set for given type!");
        }
    }
}

GL::Vec4 AssimpLoader::GetMaterialFactor(aiMaterial* assimp_material, Material::ParameterType parameter_type) {
    GL::Vec4 ans{};

    switch (parameter_type) {
        case Material::ParameterType::BASE_COLOR: {
            aiColor4D factor;
            assimp_material->Get(APP_ASSIMP_BASE_COLOR_FACTOR_PARAMETERS.pKey, APP_ASSIMP_BASE_COLOR_FACTOR_PARAMETERS.type,
                APP_ASSIMP_BASE_COLOR_FACTOR_PARAMETERS.index, factor);
            ans = GL::Vec4{factor.r, factor.g, factor.b, factor.a};
            return ans;
        }
        case Material::ParameterType::METALLIC_ROUGHNESS: {
            float metallicFactor = 0.0f;
            assimp_material->Get(APP_ASSIMP_METALLIC_FACTOR_PARAMETERS.pKey, APP_ASSIMP_METALLIC_FACTOR_PARAMETERS.type,
                APP_ASSIMP_METALLIC_FACTOR_PARAMETERS.index, metallicFactor);
            ans.X = metallicFactor;

            float roughnessFactor = 0.0f;
            assimp_material->Get(APP_ASSIMP_ROUGHNESS_FACTOR_PARAMETERS.pKey, APP_ASSIMP_ROUGHNESS_FACTOR_PARAMETERS.type,
                APP_ASSIMP_ROUGHNESS_FACTOR_PARAMETERS.index, roughnessFactor);
            ans.Y = roughnessFactor;
            return ans;
        }
        case Material::ParameterType::NORMAL: {
            return ans;
        }
        default: {
            throw std::runtime_error("AssimpLoader: factor parameters not set for given type!");
        }
    }
}

Material AssimpLoader::HandleMaterial(aiMaterial* assimp_material) {
    Material ans{assimp_material->GetName().C_Str()};

    for (int type = Material::ParameterType::BASE_COLOR; type < Material::ParameterType::SIZE; ++type) {
        Material::ParameterType parameter_type{type};
        auto assimp_texture_parameters = GetAssimpTextureParameters(parameter_type);
        auto texture_count = assimp_material->GetTextureCount(assimp_texture_parameters.type);

        if (texture_count > 1) {
            std::cout << "Warning: there are more that 1 texture of certain type given for the material: " << ans.GetName() << std::endl;
        }

        // TODO: now getting just the first texture in the list
        if (texture_count > 0) { // Getting both texture and factor
            aiString texture_filename;
            assimp_material->GetTexture(assimp_texture_parameters.type, assimp_texture_parameters.index, &texture_filename);
            std::string full_path{directory_ + texture_filename.C_Str()};

            auto found = paths_to_loaded_textures_.find(full_path);
            if (found != paths_to_loaded_textures_.end()) {
                ans.SetTexture(found->second, parameter_type);
            } else {
                GL::Vec4 factor = GetMaterialFactor(assimp_material, parameter_type);

                Timer texture_loading_timer{};
                texture_loading_timer.Start();

                Texture new_texture = Texture{full_path, factor};

                paths_to_loaded_textures_.insert(std::make_pair(full_path, new_texture));
                ans.SetTexture(new_texture, parameter_type);
                
                std::cout << "Texture (" << full_path << ") loaded successfully in " << texture_loading_timer.Stop<App::Timer::Milliseconds>() << " milliseconds" << std::endl;
            }
        } else { // Getting only factor
            GL::Vec4 factor = GetMaterialFactor(assimp_material, parameter_type);
            Texture new_texture = Texture{factor};

            ans.SetTexture(new_texture, parameter_type);
        }
    }
    return ans;
}

void AssimpLoader::HandleMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 transformation) {
    std::vector<GL::Vertex> vertices;
    std::vector<int> indices;

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
        // Positions
        if (mesh->HasPositions()) {
            vertex.Pos = GL::Vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

            // Set bbox borders
            bbox_min = GL::Vec3{(std::min)(bbox_min.X, vertex.Pos.X), (std::min)(bbox_min.Y, vertex.Pos.Y), (std::min)(bbox_min.Z, vertex.Pos.Z)};
            bbox_max = GL::Vec3{(std::max)(bbox_max.X, vertex.Pos.X), (std::max)(bbox_max.Y, vertex.Pos.Y), (std::max)(bbox_max.Z, vertex.Pos.Z)};
        }

        // Texture coordinates
        if (mesh->HasTextureCoords(0)) {
            vertex.Tex = GL::Vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.Tex = GL::Vec2{0.0f, 0.0f};
        }

        // Normals
        if (mesh->HasNormals()) {
            vertex.Normal = GL::Vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        }

        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent = GL::Vec3{mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
        }

        vertices.push_back(vertex);
    }

    // Indices
    if (mesh->HasFaces()) {
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    // Materials
    Material material;

    // WARNING: don't compare like mesh->mMaterialIndex > -1, because of unsigned comparison
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* assimp_material = scene->mMaterials[mesh->mMaterialIndex];
        material = HandleMaterial(assimp_material);
    }

    BBox bbox{bbox_shader_name_, bbox_min, bbox_max};
    Mesh new_mesh{default_shader_name_, bbox_shader_name_, mesh->mName.C_Str(), Transform{transform_to_model}, vertices, indices, material, bbox};
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