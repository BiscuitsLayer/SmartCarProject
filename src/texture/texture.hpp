#pragma once

// STL
#include <string>

// Source
#include <helpers/helpers.hpp>

namespace App {

class Texture {
public:
    enum class Type: uint32_t {
        DIFFUSE = 0,
        SPECULAR,
        CUBEMAP
    };

    Texture(GL::Texture texture, Type type, int texture_unit = APP_TEXTURE_NEXT_FREE_UNIT++);
    Texture(std::string path, Type type, int texture_unit = APP_TEXTURE_NEXT_FREE_UNIT++);

    static Texture Cubemap(std::string path, std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames);

    // private:
    GL::Texture texture_;
    int texture_unit_;
    Type type_;
};

} // namespace App