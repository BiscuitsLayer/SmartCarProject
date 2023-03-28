#pragma once

// STL
#include <string>

// Assimp
#include <assimp/scene.h>
#include <assimp/GltfMaterial.h>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Application constants
const int APP_KEYBOARD_KEYS_COUNT = GL::Key::key_t::Divide + 1;

// Vectors constants
const float APP_VECTOR_LENGTH_EPS = 1e-3f;

const int APP_GL_VEC2_COMPONENTS_COUNT = 2;
const int APP_GL_VEC2_BYTESIZE = sizeof(GL::Vec2);

const int APP_GL_VEC3_COMPONENTS_COUNT = 3;
const int APP_GL_VEC3_BYTESIZE = sizeof(GL::Vec3);

// Camera
const GL::Vec3 APP_CAMERA_WORLD_SPACE_UP = GL::Vec3(0.0f, 1.0f, 0.0f);
const GL::Vec3 APP_CAMERA_RESERVE_WORLD_SPACE_UP = GL::Vec3(1.0f, 0.0f, 0.0f);

// GL constants
const int APP_GL_VERTEX_BYTESIZE = sizeof(GL::Vertex);

const int APP_GL_VERTEX_POS_OFFSET = offsetof(GL::Vertex, Pos);
const int APP_GL_VERTEX_TEX_OFFSET = offsetof(GL::Vertex, Tex);
const int APP_GL_VERTEX_NORMAL_OFFSET = offsetof(GL::Vertex, Normal);

const int APP_ZERO_OFFSET = 0;

// Assimp
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

// Base color
const int APP_ASSIMP_DIFFUSE_TEXTURE_INDEX = 0;
const AssimpMaterialTextureParameters APP_ASSIMP_BASE_COLOR_TEXTURE_PARAMETERS{AI_MATKEY_BASE_COLOR_TEXTURE};
const AssimpMaterialColorParameters APP_ASSIMP_BASE_COLOR_FACTOR_PARAMETERS{AI_MATKEY_BASE_COLOR};

const AssimpMaterialFloatParameters APP_ASSIMP_METALLIC_FACTOR_PARAMETERS{AI_MATKEY_METALLIC_FACTOR};
const AssimpMaterialFloatParameters APP_ASSIMP_ROUGHNESS_FACTOR_PARAMETERS{AI_MATKEY_ROUGHNESS_FACTOR};

// Textures
const int APP_CUBEMAP_TEXTURES_COUNT = 6;

const int APP_CUBEMAP_TEXTURE_UNIT = 0;

// const int APP_IBL_IRRADIANCE_TEXTURE_UNIT = 0;
// const int APP_IBL_PREFILTER_TEXTURE_UNIT = 1;
// const int APP_IBL_BRDF_LUT_TEXTURE_UNIT = 2;

const int APP_BASE_COLOR_TEXTURE_UNIT = 1;
// const int APP_PBR_NORMAL_TEXTURE_UNIT = 4;
// const int APP_PBR_ROUGHNESS_TEXTURE_UNIT = 6;
// const int APP_PBR_AO_TEXTURE_UNIT = 7;

// const int APP_SHADOW_TEXTURE_UNIT = 7;

// Car
const GL::Vec3 APP_CAR_WHEELS_ROTATION_AXIS = GL::Vec3(1.0f, 0.0f, 0.0f);
const float APP_CAR_SPEED_EPS = 0.05f;

// Intersector
const int APP_INTERSECTOR_NUM_GROUPS_Z_COUNT = 1;
const float APP_INTERSECTOR_INTERSECTION_FOUND = 1.0f;
const float APP_INTERSECTOR_INTERSECTION_NOT_FOUND = 0.0f;
