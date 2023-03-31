#pragma once

// STL
#include <string>

// Assimp
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/GltfMaterial.h>

// Configured by CMake
#include <config_out.hpp>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

namespace App {

// Assimp helpers
struct AssimpMaterialTextureParameters {
    aiTextureType type;
    unsigned int index;
};

struct AssimpMaterialColorParameters {
    const char *pKey;
    unsigned int type;
    unsigned int index;
};

struct AssimpMaterialFloatParameters {
    const char *pKey;
    unsigned int type;
    unsigned int index;
};

/* ===== CONSTEXPR VARIABLES (FOR ARRAY SIZE) ===== */
// Application constants
constexpr int APP_KEYBOARD_KEYS_COUNT = GL::Key::key_t::Divide + 1;

// Textures
constexpr int APP_CUBEMAP_TEXTURES_COUNT = 6;

/* ===== EXTERN VARIABLES ===== */
extern const float APP_VECTOR_LENGTH_EPS;

// Vectors constants
extern const float APP_VECTOR_LENGTH_EPS;

extern const int APP_GL_VEC2_COMPONENTS_COUNT;
extern const int APP_GL_VEC2_BYTESIZE;

extern const int APP_GL_VEC3_COMPONENTS_COUNT;
extern const int APP_GL_VEC3_BYTESIZE;

// Camera
extern const GL::Vec3 APP_CAMERA_WORLD_SPACE_UP;
extern const GL::Vec3 APP_CAMERA_RESERVE_WORLD_SPACE_UP;

// GL constants
extern const int APP_GL_VERTEX_BYTESIZE;

extern const int APP_GL_VERTEX_POS_OFFSET;
extern const int APP_GL_VERTEX_TEX_OFFSET;
extern const int APP_GL_VERTEX_NORMAL_OFFSET;
extern const int APP_GL_VERTEX_TANGENT_OFFSET;

extern const int APP_ZERO_OFFSET;

// Base color
// WARNING: in GLTF2.0 often referred to as "pbrMetallicRoughness"
// if texture is not provided one should use only baseColorFactor value everywhere
extern const AssimpMaterialTextureParameters APP_ASSIMP_BASE_COLOR_TEXTURE_PARAMETERS;
extern const AssimpMaterialColorParameters APP_ASSIMP_BASE_COLOR_FACTOR_PARAMETERS;

// Metallic roughness
// WARNING: texture has metallic value in "R" channel, roughness value in "G" channel
// if texture is not provided one should use only metallicFactor and roughnessFactor everywhere
// WARNING: in this project we will set:
//      metallicFactor = metallicRoughnessFactor.r;
//      roughnessFactor = metallicRoughnessFactor.g;
extern const AssimpMaterialTextureParameters APP_ASSIMP_METALLIC_ROUGHNESS_TEXTURE_PARAMETERS;
extern const AssimpMaterialFloatParameters APP_ASSIMP_METALLIC_FACTOR_PARAMETERS;
extern const AssimpMaterialFloatParameters APP_ASSIMP_ROUGHNESS_FACTOR_PARAMETERS;

// Normal map
extern const int APP_ASSIMP_NORMAL_TEXTURE_INDEX;
extern const AssimpMaterialTextureParameters APP_ASSIMP_NORMAL_TEXTURE_PARAMETERS;

// Textures
extern const int APP_CUBEMAP_TEXTURE_UNIT;

extern const int APP_BASE_COLOR_TEXTURE_UNIT;
extern const int APP_METALLIC_ROUGHNESS_TEXTURE_UNIT;
extern const int APP_NORMAL_TEXTURE_UNIT;

// Car
extern const GL::Vec3 APP_CAR_WHEELS_ROTATION_AXIS;
extern const float APP_CAR_SPEED_EPS;

// Intersector
extern const int APP_INTERSECTOR_NUM_GROUPS_Z_COUNT;
extern const float APP_INTERSECTOR_INTERSECTION_FOUND;
extern const float APP_INTERSECTOR_INTERSECTION_NOT_FOUND;

} // namespace App