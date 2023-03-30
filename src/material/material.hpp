#pragma once

// STL
#include <array>

// Constants
#include <constants/constants.hpp>

// LibSmartCar
#include <texture/texture.hpp>

// Incomplete type resolve
#include <helpers/helpers_fwd.hpp>

namespace App {

class Material {
public:
    enum ParameterType : int {
        BASE_COLOR = 0,
        METALLIC_ROUGHNESS,
        NORMAL,
        SIZE
    };

    Material(const std::string& name);
    Material();

    std::string GetName() const;

    void SetTexture(const Texture& texture, ParameterType type);
    Texture GetTexture(ParameterType type) const;

private:
    void BindTextures(std::shared_ptr<GL::Program> program) const;

    std::string name_;
    std::array<Texture, ParameterType::SIZE> textures_;

    friend class Mesh;
};

} // namespace App