#include "constants.hpp"

namespace App {

// Keyboard
const char* keyboard_modes[static_cast<size_t>(KeyboardMode::SIZE)] = { "ORBIT CAMERA", "CAR MOVEMENT", "NN_LEARNING" };

// Vectors constants
const float APP_VECTOR_LENGTH_EPS = 1e-3f;

const int APP_GL_VEC2_COMPONENTS_COUNT = 2;
const int APP_GL_VEC2_BYTESIZE = sizeof(GL::Vec2);

const int APP_GL_VEC3_COMPONENTS_COUNT = 3;
const int APP_GL_VEC3_BYTESIZE = sizeof(GL::Vec3);

const int APP_GL_VEC4_COMPONENTS_COUNT = 4;
const int APP_GL_VEC4_BYTESIZE = sizeof(GL::Vec4);

const int APP_GL_MAT4_COMPONENTS_COUNT = 16;
const int APP_GL_MAT4_BYTESIZE = sizeof(GL::Mat4);

// Camera
const GL::Vec3 APP_CAMERA_WORLD_SPACE_UP = GL::Vec3(0.0f, 1.0f, 0.0f);
const GL::Vec3 APP_CAMERA_RESERVE_WORLD_SPACE_UP = GL::Vec3(1.0f, 0.0f, 0.0f);

// GL constants
const int APP_WINDOW_COLOR_BITS = 23;
const int APP_WINDOW_DEPTH_BITS= 24;
const int APP_WINDOW_STENCIL_BITS = 0;
const int APP_WINDOW_MULTISAMPLE_BITS = 4;

const int APP_GL_VERTEX_BYTESIZE = sizeof(GL::Vertex);

const int APP_GL_VERTEX_POS_OFFSET = offsetof(GL::Vertex, Pos);
const int APP_GL_VERTEX_TEX_OFFSET = offsetof(GL::Vertex, Tex);
const int APP_GL_VERTEX_NORMAL_OFFSET = offsetof(GL::Vertex, Normal);
const int APP_GL_VERTEX_TANGENT_OFFSET = offsetof(GL::Vertex, Tangent);

const int APP_ZERO_OFFSET = 0;

// Base color
// WARNING: in GLTF2.0 often referred to as "pbrMetallicRoughness"
// if texture is not provided one should use only baseColorFactor value everywhere
const AssimpMaterialTextureParameters APP_ASSIMP_BASE_COLOR_TEXTURE_PARAMETERS{AI_MATKEY_BASE_COLOR_TEXTURE};
const AssimpMaterialColorParameters APP_ASSIMP_BASE_COLOR_FACTOR_PARAMETERS{AI_MATKEY_BASE_COLOR};

// Metallic roughness
// WARNING: texture has metallic value in "R" channel, roughness value in "G" channel
// if texture is not provided one should use only metallicFactor and roughnessFactor everywhere
// WARNING: in this project we will set:
//      metallicFactor = metallicRoughnessFactor.r;
//      roughnessFactor = metallicRoughnessFactor.g;
const AssimpMaterialTextureParameters APP_ASSIMP_METALLIC_ROUGHNESS_TEXTURE_PARAMETERS{AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE};
const AssimpMaterialFloatParameters APP_ASSIMP_METALLIC_FACTOR_PARAMETERS{AI_MATKEY_METALLIC_FACTOR};
const AssimpMaterialFloatParameters APP_ASSIMP_ROUGHNESS_FACTOR_PARAMETERS{AI_MATKEY_ROUGHNESS_FACTOR};

// Normal map
const int APP_ASSIMP_NORMAL_TEXTURE_INDEX = 0;
const AssimpMaterialTextureParameters APP_ASSIMP_NORMAL_TEXTURE_PARAMETERS{aiTextureType_NORMALS, APP_ASSIMP_NORMAL_TEXTURE_INDEX};

// Textures
const int APP_CUBEMAP_TEXTURE_UNIT = 0;

const int APP_BASE_COLOR_TEXTURE_UNIT = 4;
const int APP_METALLIC_ROUGHNESS_TEXTURE_UNIT = 2;
const int APP_NORMAL_TEXTURE_UNIT = 3;

// Car
const GL::Vec3 APP_CAR_WHEELS_ROTATION_AXIS = GL::Vec3(1.0f, 0.0f, 0.0f);
const float APP_CAR_SPEED_EPS = 0.05f;

// Intersector
const int APP_INTERSECTOR_NUM_GROUPS_Z_COUNT = 1;
const float APP_INTERSECTOR_INTERSECTION_FOUND = 1.0f;
const float APP_INTERSECTOR_INTERSECTION_NOT_FOUND = 0.0f;

}