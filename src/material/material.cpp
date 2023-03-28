#include "material.hpp"

// Incomplete type resolve
#include <helpers/helpers.hpp>

namespace App {

Material::Material(const std::string& name)
    : name_(name) {}

Material::Material()
    : Material(std::string{}) {}

std::string Material::GetName() const {
    return name_;
}

void Material::SetTexture(const Texture& texture, ParameterType type) {
    textures_[type] = texture;
}

Texture Material::GetTexture(ParameterType type) const {
    return textures_[type];
}

void Material::BindTextures(std::shared_ptr<GL::Program> program) const {
    auto& context = App::Context::Get();
    auto& gl = context.gl->get();

    program->SetUniform(program->GetUniform("baseColorHasTexture"), textures_[ParameterType::BASE_COLOR].texture_.has_value());
    if (textures_[ParameterType::BASE_COLOR].texture_.has_value()) {
        program->SetUniform(program->GetUniform("baseColorTexture"), APP_BASE_COLOR_TEXTURE_UNIT);
        gl.BindTexture(textures_[ParameterType::BASE_COLOR].texture_.value(), APP_BASE_COLOR_TEXTURE_UNIT);
    }
    program->SetUniform(program->GetUniform("baseColorFactor"), textures_[ParameterType::BASE_COLOR].color_);
}


} // namespace App