#pragma once

// STL
#include <vector>

// Sources
#include <constants/constants.hpp>

const int APP_WOODEN_TEXTURE_UNIT = 0;
const int APP_AWESOME_TEXTURE_UNIT = 1;

namespace App {

class CubeEbo {
public:
    CubeEbo(GL::Program& program) {
        LoadGeometry(program);
        LoadTextures();
    }

    void LoadGeometry(GL::Program& program) {
        vbo_ = GL::VertexBuffer(vertices_.data(), vertices_.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);
        cbo_ = GL::VertexBuffer(colors_.data(), colors_.size() * APP_VEC3_BYTESIZE, GL::BufferUsage::StaticDraw);
        ebo_ = GL::VertexBuffer(indices_.data(), indices_.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);

        vao_.BindAttribute(program.GetAttribute("aPos"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);
        vao_.BindAttribute(program.GetAttribute("aColor"), cbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_VEC3_BYTESIZE, 0);
        vao_.BindElements(ebo_);
    }

    void LoadTextures() { /* empty */ }

    void Draw(GL::Context& gl, GL::Program& program) {
        gl.DrawElements(vao_, GL::Primitive::Triangles, 0, indices_.size(), GL::Type::UnsignedInt);
    }

private:
    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object
    GL::VertexBuffer ebo_;  // element buffer object

    GL::VertexBuffer cbo_;  // color buffer object

    std::vector<GL::Vec3> colors_ = {
        GL::Vec3{1.0, 0.0, 0.0},
        GL::Vec3{0.0, 1.0, 0.0},
        GL::Vec3{0.0, 0.0, 1.0},
        GL::Vec3{1.0, 1.0, 1.0},
        GL::Vec3{1.0, 0.0, 0.0},
        GL::Vec3{0.0, 1.0, 0.0},
        GL::Vec3{0.0, 0.0, 1.0},
        GL::Vec3{1.0, 1.0, 1.0},
    };

    std::vector<GL::Vertex> vertices_ = {
        GL::Vertex{GL::Vec3{-0.5f, -0.5f, -0.5f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f, -0.5f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f, -0.5f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f,  0.5f, -0.5f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f, -0.5f,  0.5f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f,  0.5f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f,  0.5f}, GL::Vec2{}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f,  0.5f,  0.5f}, GL::Vec2{}, GL::Vec3{}},
    };

    std::vector<unsigned int> indices_ = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        7, 6, 5, 5, 4, 7,
        4, 0, 3, 3, 7, 4,
        4, 5, 1, 1, 0, 4,
        3, 2, 6, 6, 7, 3,
    };
};

} // namespace App