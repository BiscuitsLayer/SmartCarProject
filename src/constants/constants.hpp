#pragma once

// STL
#include <string>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Application constants
const int APP_KEYBOARD_KEYS_COUNT = GL::Key::key_t::Divide + 1;

const float APP_INIT_MAX_FPS = 90.0f;
const bool APP_MAX_FPS_MODE_ON = true;

const int APP_INIT_WINDOW_WIDTH = 800;
const int APP_INIT_WINDOW_HEIGHT = 600;
const std::string APP_INIT_WINDOW_TITLE = "DemoApp";
const bool APP_INIT_IS_FULLSCREEN = false;

// Projection matrix constants
const float APP_INIT_FOV_IN_DEGREES = 45.0f;
const float APP_INIT_NEAR_PLANE = 0.1f;
const float APP_INIT_FAR_PLANE = 10000.0f;

// Vectors constants
const float APP_VECTOR_LENGTH_EPS = 1e-3;

const int APP_VEC2_COMPONENTS_COUNT = 2;
const int APP_VEC2_BYTESIZE = 2 * sizeof(float);

const int APP_VEC3_COMPONENTS_COUNT = 3;
const int APP_VEC3_BYTESIZE = 3 * sizeof(float);

// Camera
const float APP_CAMERA_MIN_LENGTH_TO_TARGET = 1.0f;
const float APP_CAMERA_MAX_LENGTH_TO_TARGET = 1000.0f;

const GL::Vec3 APP_CAMERA_POSITION = GL::Vec3(0.0f, 2.0f, -10.0f);
const GL::Vec3 APP_CAMERA_TARGET = GL::Vec3(0.0f, 0.0f, 0.0f);
const float APP_CAMERA_MOVE_SPEED = 5.0f;
const float APP_CAMERA_ROTATE_SPEED = 10.0f;

const GL::Vec3 APP_CAMERA_WORLD_SPACE_UP = GL::Vec3(0.0f, 1.0f, 0.0f);

// GL constants
const int APP_GL_VERTEX_BYTESIZE = sizeof(GL::Vertex);

const int APP_GL_VERTEX_POS_OFFSET = offsetof(GL::Vertex, Pos);
const int APP_GL_VERTEX_TEX_OFFSET = offsetof(GL::Vertex, Tex);
const int APP_GL_VERTEX_NORMAL_OFFSET = offsetof(GL::Vertex, Normal);

// Textures naming (before atlas is implemented)
const std::string APP_TEXTURE_DIFFUSE_BASE_NAME = "textureDiffuse";
const std::string APP_TEXTURE_SPECULAR_BASE_NAME = "textureSpecular";

// Texture indexing
static int APP_TEXTURE_NEXT_FREE_UNIT = 0;

// Car parameters
const float APP_CAR_MOVE_SPEED = 2.0f;
const float APP_CAR_ROTATE_SPEED = 4.0f;
const GL::Vec3 APP_CAR_CENTER_TRANSLATION = GL::Vec3(0.0f, 0.0f, 1.5f);