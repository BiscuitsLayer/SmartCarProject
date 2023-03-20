#pragma once

// STL
#include <array>
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <model/model.hpp>
#include <texture/texture.hpp>

namespace App {

class Skybox: public Model {
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

    virtual void Draw() override;

private:
    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object

    std::vector<Texture> textures_;

    std::vector<GL::Vertex> vertices_ = {
        GL::Vertex{GL::Vec3{-1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{-1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{ 1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{-1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{-1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f,  1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f,  1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{-1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f, -1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{ 1.0f, -1.0f,  1.0f}, GL::Vec2{}, GL::Vec3{}},
    };

    std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames_;

    std::string shader_name_;
};

} // namespace App