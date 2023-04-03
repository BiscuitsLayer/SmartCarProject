#include "skybox.hpp"

namespace App {

// Extern variables
extern const int APP_GL_VEC3_COMPONENTS_COUNT;
extern const int APP_GL_VEC3_BYTESIZE;
extern const int APP_ZERO_OFFSET;
extern const int APP_CUBEMAP_TEXTURE_UNIT;

Skybox::Skybox(std::string model_name, std::string shader_name, std::string folder, std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames)
    : Model(model_name, {}, {}, {}, App::Transform{}), shader_name_(shader_name), filenames_(filenames) {
    vbo_ = GL::VertexBuffer(vertices_.data(), vertices_.size() * APP_GL_VEC3_BYTESIZE, GL::BufferUsage::StaticDraw);

    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto skybox_program = shader_handler.at(shader_name_);
    gl.UseProgram(*skybox_program);

    vao_.BindAttribute(skybox_program->GetAttribute("aPos"), vbo_, GL::Type::Float, APP_GL_VEC3_COMPONENTS_COUNT, APP_GL_VEC3_BYTESIZE, APP_ZERO_OFFSET);

    texture_ = Texture::Cubemap(folder, filenames_);
}

Skybox::Skybox(Config::SkyboxModelConfig config)
    : Skybox(config.name, config.shader.default_shader_name, config.folder, config.filenames) {}

void Skybox::Draw() const {
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto skybox_program = shader_handler.at(shader_name_);
    gl.UseProgram(*skybox_program);
    skybox_program->SetUniform(skybox_program->GetUniform("aMatrices.viewMatrix"), context.camera->GetViewMatrix());
    skybox_program->SetUniform(skybox_program->GetUniform("aMatrices.projectionMatrix"), context.projection_matrix.value());

    gl.DepthFunc(GL::TestFunction::LessEqual);

    if (texture_.texture_.has_value()) {
        skybox_program->SetUniform(skybox_program->GetUniform("cubemap"), APP_CUBEMAP_TEXTURE_UNIT);
        gl.BindCubemap(texture_.texture_.value(), APP_CUBEMAP_TEXTURE_UNIT);
    }

    gl.DrawArrays(vao_, GL::Primitive::Triangles, 0, vertices_.size());
    gl.DepthFunc(GL::TestFunction::Less);
}

} // namespace App