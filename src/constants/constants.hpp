#pragma once

// STL
#include <string>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Application constants
const int APP_KEYBOARD_KEYS_COUNT = GL::Key::key_t::Divide + 1;

// Vectors constants
const float APP_VECTOR_LENGTH_EPS = 1e-3;

const int APP_VEC2_COMPONENTS_COUNT = 2;
const int APP_VEC2_BYTESIZE = 2 * sizeof(float);

const int APP_VEC3_COMPONENTS_COUNT = 3;
const int APP_VEC3_BYTESIZE = 3 * sizeof(float);

// Camera
const GL::Vec3 APP_CAMERA_WORLD_SPACE_UP = GL::Vec3(0.0f, 1.0f, 0.0f);

// GL constants
const int APP_GL_VERTEX_BYTESIZE = sizeof(GL::Vertex);

const int APP_GL_VERTEX_POS_OFFSET = offsetof(GL::Vertex, Pos);
const int APP_GL_VERTEX_TEX_OFFSET = offsetof(GL::Vertex, Tex);
const int APP_GL_VERTEX_NORMAL_OFFSET = offsetof(GL::Vertex, Normal);

const int APP_CUBEMAP_TEXTURES_COUNT = 6;

// Textures naming (before atlas is implemented)
const std::string APP_TEXTURE_DIFFUSE_BASE_NAME = "textureDiffuse";
const std::string APP_TEXTURE_SPECULAR_BASE_NAME = "textureSpecular";

// Texture indexing
static int APP_TEXTURE_NEXT_FREE_UNIT = 0;