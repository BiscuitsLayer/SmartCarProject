#pragma once

// STL
#include <string>

// Source
#include <helpers/helpers.hpp>

namespace App {

class Texture {
public:
    enum class Type : uint32_t {
        DIFFUSE = 0,
        SPECULAR
    };

    Texture(GL::Texture texture, Type type, int texture_unit = APP_TEXTURE_NEXT_FREE_UNIT++)
    : texture_(texture), texture_unit_(texture_unit), type_(type) {
        texture_.SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
    }

    Texture(std::string path, Type type, int texture_unit = APP_TEXTURE_NEXT_FREE_UNIT++)
    : texture_unit_(texture_unit), type_(type) {
        std::string buffer = App::ReadFileData(path, false);
	    auto image = GL::Image{reinterpret_cast<unsigned char*>(buffer.data()), buffer.size()};
	    texture_ = GL::Texture{image, GL::InternalFormat::RGB};
        texture_.SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
    }

// private:
    GL::Texture texture_;
    int texture_unit_;
    Type type_;
};

} // namespace App