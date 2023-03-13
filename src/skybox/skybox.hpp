#pragma once

// STL
#include <array>
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <texture/texture.hpp>

namespace App {

class Skybox {
/*
 *  loads a cubemap texture from 6 individual texture faces
 *  order:
 *  +X (right)
 *  -X (left)
 *  +Y (top)
 *  -Y (bottom)
 *  +Z (front) 
 *  -Z (back)
 */
public:
    Skybox(GL::Program &program, std::string path, std::array<std::string, 6> filenames) : filenames_(filenames) {
        LoadGeometry(program);
        LoadTextures(path);
    }

    void LoadGeometry(GL::Program &program) {
        vbo_ = GL::VertexBuffer(vertices_.data(), vertices_.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);
  
        vao_.BindAttribute(program.GetAttribute("aPos"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);
    }

    void LoadTextures(std::string path) {
        Texture cubemap = Texture::Cubemap(path, filenames_);
        textures_.push_back(cubemap);
	}

    void Draw(GL::Context &gl, GL::Program &program) {
        gl.UseProgram(program);

        gl.DepthFunc(GL::TestFunction::LessEqual);

        for (unsigned int i = 0; i < textures_.size(); ++i) {
            program.SetUniform(program.GetUniform("cubemap"), textures_[i].texture_unit_);
            gl.BindCubemap(textures_[i].texture_, textures_[i].texture_unit_);
        }

		gl.DrawArrays(vao_, GL::Primitive::Triangles, 0, vertices_.size());
        gl.DepthFunc(GL::TestFunction::Less);
    }

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

    std::array<std::string, 6> filenames_;
};

} // namespace App