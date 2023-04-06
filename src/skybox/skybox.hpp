#pragma once

// STL
#include <array>
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <skybox/skybox_fwd.hpp>

// LibSmartCar
#include <model/model.hpp>
#include <texture/texture.hpp>

namespace App {

class Skybox {
    /*
     *  loads a cubemap texture from individual texture faces
     *  order:
     *  +X (right)
     *  -X (left)
     *  +Y (top)
     *  -Y (bottom)
     *  +Z (front)
     *  -Z (back)
     */
public:
    Skybox(std::string model_name, std::string shader_name, std::string folder,
        std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames);
    Skybox(Config::SkyboxModelConfig config);

    void Draw() const;

private:
    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object

    // Cubemap only has a single texture
    Texture texture_;

    std::vector<GL::Vec3> vertices_ = {
        GL::Vec3{-1.0f,  1.0f, -1.0f},
        GL::Vec3{-1.0f, -1.0f, -1.0f},
        GL::Vec3{ 1.0f, -1.0f, -1.0f},
        GL::Vec3{ 1.0f, -1.0f, -1.0f},
        GL::Vec3{ 1.0f,  1.0f, -1.0f},
        GL::Vec3{-1.0f,  1.0f, -1.0f},

        GL::Vec3{-1.0f, -1.0f,  1.0f},
        GL::Vec3{-1.0f, -1.0f, -1.0f},
        GL::Vec3{-1.0f,  1.0f, -1.0f},
        GL::Vec3{-1.0f,  1.0f, -1.0f},
        GL::Vec3{-1.0f,  1.0f,  1.0f},
        GL::Vec3{-1.0f, -1.0f,  1.0f},

        GL::Vec3{ 1.0f, -1.0f, -1.0f},
        GL::Vec3{ 1.0f, -1.0f,  1.0f},
        GL::Vec3{ 1.0f,  1.0f,  1.0f},
        GL::Vec3{ 1.0f,  1.0f,  1.0f},
        GL::Vec3{ 1.0f,  1.0f, -1.0f},
        GL::Vec3{ 1.0f, -1.0f, -1.0f},

        GL::Vec3{-1.0f, -1.0f,  1.0f},
        GL::Vec3{-1.0f,  1.0f,  1.0f},
        GL::Vec3{ 1.0f,  1.0f,  1.0f},
        GL::Vec3{ 1.0f,  1.0f,  1.0f},
        GL::Vec3{ 1.0f, -1.0f,  1.0f},
        GL::Vec3{-1.0f, -1.0f,  1.0f},

        GL::Vec3{-1.0f,  1.0f, -1.0f},
        GL::Vec3{ 1.0f,  1.0f, -1.0f},
        GL::Vec3{ 1.0f,  1.0f,  1.0f},
        GL::Vec3{ 1.0f,  1.0f,  1.0f},
        GL::Vec3{-1.0f,  1.0f,  1.0f},
        GL::Vec3{-1.0f,  1.0f, -1.0f},

        GL::Vec3{-1.0f, -1.0f, -1.0f},
        GL::Vec3{-1.0f, -1.0f,  1.0f},
        GL::Vec3{ 1.0f, -1.0f, -1.0f},
        GL::Vec3{ 1.0f, -1.0f, -1.0f},
        GL::Vec3{-1.0f, -1.0f,  1.0f},
        GL::Vec3{ 1.0f, -1.0f,  1.0f},
    };

    std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames_;

    std::string shader_name_;
};

} // namespace App