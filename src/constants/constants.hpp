#pragma once

// STL
#include <string>
#include <cmath>

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

/* ===== MATH CONSTANTS ===== */
inline constexpr double APP_MATH_PI = 3.14159265358979323846;

/* ===== CONSTEXPR VARIABLES (FOR ARRAY SIZE) ===== */
// Application constants
constexpr int APP_KEYBOARD_KEYS_COUNT = GL::Key::key_t::Divide + 1;

// Textures
constexpr int APP_CUBEMAP_TEXTURES_COUNT = 6;

// Ray intersector
constexpr int APP_RAY_INTERSECTOR_RAYS_COUNT = 121;

// For DQN algorithm
constexpr int APP_CAR_STATE_PARAMETERS_COUNT = APP_RAY_INTERSECTOR_RAYS_COUNT + 4;
constexpr int APP_CAR_ACTIONS_COUNT = 4;

/* ===== EXTERN VARIABLES ===== */
// Keyboard
enum class KeyboardMode: int {
    ORBIT_CAMERA = 0,
    CAR_MOVEMENT,
    NN_LEARNING,
    SIZE
};
extern const char* keyboard_modes[static_cast<size_t>(KeyboardMode::SIZE)];

// Vectors constants
extern const float APP_VECTOR_LENGTH_EPS;

extern const int APP_GL_VEC2_COMPONENTS_COUNT;
extern const int APP_GL_VEC2_BYTESIZE;

extern const int APP_GL_VEC3_COMPONENTS_COUNT;
extern const int APP_GL_VEC3_BYTESIZE;

extern const int APP_GL_VEC4_COMPONENTS_COUNT;
extern const int APP_GL_VEC4_BYTESIZE;

extern const int APP_GL_MAT4_COMPONENTS_COUNT;
extern const int APP_GL_MAT4_BYTESIZE;

// Camera
extern const GL::Vec3 APP_CAMERA_WORLD_SPACE_UP;
extern const GL::Vec3 APP_CAMERA_RESERVE_WORLD_SPACE_UP;

// GL constants
extern const int APP_WINDOW_COLOR_BITS;
extern const int APP_WINDOW_DEPTH_BITS;
extern const int APP_WINDOW_STENCIL_BITS;
extern const int APP_WINDOW_MULTISAMPLE_BITS;

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