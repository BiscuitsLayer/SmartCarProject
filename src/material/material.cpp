#include "material.hpp"

// Incomplete type resolve
#include <helpers/helpers.hpp>

namespace App {

// Extern variables
extern const int APP_BASE_COLOR_TEXTURE_UNIT;

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

    // Base color
    program->SetUniform(program->GetUniform("baseColorHasTexture"), textures_[ParameterType::BASE_COLOR].texture_.has_value());
    if (textures_[ParameterType::BASE_COLOR].texture_.has_value()) {
        program->SetUniform(program->GetUniform("baseColorTexture"), APP_BASE_COLOR_TEXTURE_UNIT);
        gl.BindTexture(textures_[ParameterType::BASE_COLOR].texture_.value(), APP_BASE_COLOR_TEXTURE_UNIT);
    }
    program->SetUniform(program->GetUniform("baseColorFactor"), textures_[ParameterType::BASE_COLOR].factor_);

    // Metallic roughness
    program->SetUniform(program->GetUniform("metallicRoughnessHasTexture"), textures_[ParameterType::METALLIC_ROUGHNESS].texture_.has_value());
    if (textures_[ParameterType::METALLIC_ROUGHNESS].texture_.has_value()) {
        program->SetUniform(program->GetUniform("metallicRoughnessTexture"), APP_METALLIC_ROUGHNESS_TEXTURE_UNIT);
        gl.BindTexture(textures_[ParameterType::METALLIC_ROUGHNESS].texture_.value(), APP_METALLIC_ROUGHNESS_TEXTURE_UNIT);
    }
    program->SetUniform(program->GetUniform("metallicRoughnessFactor"), textures_[ParameterType::METALLIC_ROUGHNESS].factor_);

    // Normal map
    program->SetUniform(program->GetUniform("normalHasTexture"), textures_[ParameterType::NORMAL].texture_.has_value());
    if (textures_[ParameterType::NORMAL].texture_.has_value()) {
        program->SetUniform(program->GetUniform("normalTexture"), APP_NORMAL_TEXTURE_UNIT);
        gl.BindTexture(textures_[ParameterType::NORMAL].texture_.value(), APP_NORMAL_TEXTURE_UNIT);
    }
}


} // namespace App