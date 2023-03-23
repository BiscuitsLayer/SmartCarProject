#include "skybox.hpp"

namespace App {

Skybox::Skybox(std::string model_name, std::string shader_name, std::string folder, std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames)
    : Model(model_name, {}, {}, {}, App::Transform{}), shader_name_(shader_name), filenames_(filenames) {
    vbo_ = GL::VertexBuffer(vertices_.data(), vertices_.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);

    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    auto skybox_program = shader_handler.at(shader_name_);
    gl.UseProgram(*skybox_program);

    vao_.BindAttribute(skybox_program->GetAttribute("aPos"), vbo_, GL::Type::Float, APP_GL_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);

    Texture cubemap = Texture::Cubemap(folder, filenames_);
    textures_.push_back(cubemap);
}

Skybox::Skybox(Config::SkyboxModelConfig config)
    : Skybox(config.name, config.shader.default_shader_name, config.folder, config.filenames) {}

void Skybox::Draw() {
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();
    auto shader_handler = context.shader_handler.value();

    GL::Mat4 vp = App::GetViewNoTranslationProjectionMatrix(context.camera->GetViewMatrix(), context.projection_matrix.value());

    auto skybox_program = shader_handler.at(shader_name_);
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

} // namespace App