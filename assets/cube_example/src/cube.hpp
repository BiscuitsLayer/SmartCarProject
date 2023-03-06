#pragma once

// STL
#include <vector>

// Sources
#include <constants/constants.hpp>
#include <texture/texture.hpp>

const int APP_WOODEN_TEXTURE_UNIT = 0;
const int APP_AWESOME_TEXTURE_UNIT = 1;

namespace App {

class Cube {
public:
    Cube(GL::Program &program) {
        LoadGeometry(program);
        LoadTextures();
    }

    void LoadGeometry(GL::Program &program) {
        vbo_ = GL::VertexBuffer(vertices_.data(), vertices_.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);
  
        vao_.BindAttribute(program.GetAttribute("aPos"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);
        vao_.BindAttribute(program.GetAttribute("aTexCoord"), vbo_, GL::Type::Float, APP_VEC2_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_TEX_OFFSET);
    }

    void LoadTextures() {
	    textures_.push_back(Texture{"../assets/cube_example/textures/container.jpg", Texture::Type::DIFFUSE, APP_WOODEN_TEXTURE_UNIT});
	    textures_.push_back(Texture{"../assets/cube_example/textures/awesomeface.jpg", Texture::Type::DIFFUSE, APP_AWESOME_TEXTURE_UNIT});
    }

    void Draw(GL::Context &gl, GL::Program &program) {
        // to be changed
        unsigned int diffuseNumber = 1;
        unsigned int specularNumber = 1;

        for (unsigned int i = 0; i < textures_.size(); ++i) {
            std::string texture_name;

            switch (textures_[i].type_) {
                case Texture::Type::DIFFUSE: {
                    texture_name = APP_TEXTURE_DIFFUSE_BASE_NAME + std::to_string(diffuseNumber);
                    ++diffuseNumber;
                    break;
                }
                case Texture::Type::SPECULAR: {
                    texture_name = APP_TEXTURE_SPECULAR_BASE_NAME + std::to_string(specularNumber);
                    ++specularNumber;
                    break;
                }
            }

            program.SetUniform(program.GetUniform(texture_name), textures_[i].texture_unit_);
            gl.BindTexture(textures_[i].texture_, textures_[i].texture_unit_);
        }

		gl.DrawArrays(vao_, GL::Primitive::Triangles, 0, vertices_.size());
    }

private:
    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object

    std::vector<Texture> textures_;

    std::vector<GL::Vertex> vertices_ = {
        GL::Vertex{GL::Vec3{-0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f, -0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},  
        GL::Vertex{GL::Vec3{0.5f,  0.5f, -0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},  
        GL::Vertex{GL::Vec3{0.5f,  0.5f, -0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},  
        GL::Vertex{GL::Vec3{-0.5f,  0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},  
        GL::Vertex{GL::Vec3{-0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},  

        GL::Vertex{GL::Vec3{-0.5f, -0.5f,  0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f,  0.5f,  0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f, -0.5f,  0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{-0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f,  0.5f, -0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f, -0.5f,  0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f, -0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f,  0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{-0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f, -0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f, -0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f, -0.5f,  0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f, -0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},

        GL::Vertex{GL::Vec3{-0.5f,  0.5f, -0.5f}, GL::Vec2{0.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f, -0.5f}, GL::Vec2{1.0f, 1.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{0.5f,  0.5f,  0.5f}, GL::Vec2{1.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f,  0.5f,  0.5f}, GL::Vec2{0.0f, 0.0f}, GL::Vec3{}},
        GL::Vertex{GL::Vec3{-0.5f,  0.5f, -0.5f}, GL::Vec2{0.0f, 1.0}, GL::Vec3{}},
    };
};

} // namespace App