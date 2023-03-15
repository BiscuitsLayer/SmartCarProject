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
    Skybox(std::string skybox_shader_name, std::string folder, std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames)
        : Model({}, {}, {}, App::Transform{}), skybox_shader_name_(skybox_shader_name), filenames_(filenames) {
        vbo_ = GL::VertexBuffer(vertices_.data(), vertices_.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);

        auto& context = App::Context::Get();
        auto& gl = context.gl.value().get();
        auto shader_handler = context.shader_handler.value();

        auto skybox_program = shader_handler.at(skybox_shader_name_);
        gl.UseProgram(*skybox_program);

        vao_.BindAttribute(skybox_program->GetAttribute("aPos"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);

        Texture cubemap = Texture::Cubemap(folder, filenames_);
        textures_.push_back(cubemap);
    }

    Skybox(Config::SkyboxModelConfig config)
        : Skybox(config.shader.skybox_shader_name, config.folder, config.filenames) {}

    virtual void Draw() override {
        auto& context = App::Context::Get();
        auto& gl = context.gl.value().get();
        auto shader_handler = context.shader_handler.value();

        GL::Mat4 vp = App::GetViewNoTranslationProjectionMatrix(context.camera.value()->GetViewMatrix(), context.projection_matrix.value());

        auto skybox_program = shader_handler.at(skybox_shader_name_);
        gl.UseProgram(*skybox_program);
        skybox_program->SetUniform(skybox_program->GetUniform("VP"), vp);

        gl.DepthFunc(GL::TestFunction::LessEqual);

        for (unsigned int i = 0; i < textures_.size(); ++i) {
            skybox_program->SetUniform(skybox_program->GetUniform("cubemap"), textures_[i].texture_unit_);
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

    std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames_;

    std::string skybox_shader_name_;
};

} // namespace App