#include "texture.hpp"

// Incomplete type resolve
#include <helpers/helpers.hpp>

namespace App {

Texture::Texture(GL::Texture texture, GL::Vec4 factor)
    : texture_(texture), color_(factor) {}

Texture::Texture(std::string path, GL::Vec4 factor)
    : texture_(std::nullopt), color_(factor) {
    std::string buffer = App::ReadFileData(path, false);
    auto image = GL::Image{reinterpret_cast<unsigned char*>(buffer.data()), static_cast<GL::uint>(buffer.size())};
    texture_ = GL::Texture{image, GL::InternalFormat::RGB};
    texture_->SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
}

Texture::Texture(GL::Texture texture)
    : texture_(texture), color_(GL::Vec4{}) {
    texture_->SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
}

Texture::Texture(std::string path)
    : texture_(std::nullopt), color_(GL::Vec4{}) {
    std::string buffer = App::ReadFileData(path, false);
    auto image = GL::Image{reinterpret_cast<unsigned char*>(buffer.data()), static_cast<GL::uint>(buffer.size())};
    texture_ = GL::Texture{image, GL::InternalFormat::RGB};
    texture_->SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
}

Texture::Texture(GL::Vec4 color)
    : texture_(std::nullopt), color_(color) {}

Texture::Texture()
    : texture_(std::nullopt), color_(GL::Vec4{}) {}

Texture Texture::Cubemap(std::string path, std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames) {
    std::string buffer;
    std::array<std::shared_ptr<GL::Image>, APP_CUBEMAP_TEXTURES_COUNT> image_ptrs;

    unsigned int index = 0;
    for (auto filename : filenames) {
        std::string full_path = path + "/" + filename;
        buffer = App::ReadFileData(full_path, false);
        auto image = std::make_shared<GL::Image>(reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
        image_ptrs[index] = image;
        ++index;
    }

    // TODO: fix texture unit
    return Texture{GL::Texture::Cubemap(image_ptrs, GL::InternalFormat::RGB)};
}

} // namespace App