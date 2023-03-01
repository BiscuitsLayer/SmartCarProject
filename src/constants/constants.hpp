#pragma once

// STL
#include <string>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

const int APP_INIT_WINDOW_WIDTH = 800;
const int APP_INIT_WINDOW_HEIGHT = 600;
const std::string APP_INIT_WINDOW_TITLE = "DemoApp";
const bool APP_INIT_IS_FULLSCREEN = false;

const float APP_INIT_FOV_IN_DEGREES = 45.0f;
const float APP_INIT_NEAR_PLANE = 0.1f;
const float APP_INIT_FAR_PLANE = 100.0f;

const float APP_INIT_MAX_FPS = 90.0f;
const bool APP_MAX_FPS_MODE_ON = true;

const int APP_VEC2_COMPONENTS_COUNT = 2;
const int APP_VEC2_BYTESIZE = 2 * sizeof(float);

const int APP_VEC3_COMPONENTS_COUNT = 3;
const int APP_VEC3_BYTESIZE = 3 * sizeof(float);

const int APP_GL_VERTEX_BYTESIZE = sizeof(GL::Vertex);