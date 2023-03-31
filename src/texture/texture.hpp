#pragma once

// STL
#include <string>
#include <optional>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <texture/texture_fwd.hpp>

// LibSmartCar
#include <timer/timer.hpp>

// Incomplete type resolve
#include <helpers/helpers_fwd.hpp>

namespace App {

class Texture {
public:
    // Texture + color
    Texture(GL::Texture texture, GL::Vec4 factor);
    Texture(std::string path, GL::Vec4 factor);

    // Only texture
    Texture(GL::Texture texture);
    Texture(std::string path);

    // Only color
    Texture(GL::Vec4 factor);

    // Empty
    Texture();

    static Texture Cubemap(std::string path, std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames);

private:
    static std::shared_ptr<GL::Image> LoadSerializedData(const std::string& path);

    std::optional<GL::Texture> texture_;
    GL::Vec4 factor_;

    friend class Material;
    friend class Skybox;
};

} // namespace App