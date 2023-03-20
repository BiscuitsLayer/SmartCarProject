#include "texture.hpp"

namespace App {

Texture::Texture(GL::Texture texture, Type type, int texture_unit)
    : texture_(texture), texture_unit_(texture_unit), type_(type) {
    texture_.SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
}

Texture::Texture(std::string path, Type type, int texture_unit)
    : texture_unit_(texture_unit), type_(type) {
    std::string buffer = App::ReadFileData(path, false);
    auto image = GL::Image{ reinterpret_cast<unsigned char*>(buffer.data()), static_cast<GL::uint>(buffer.size()) };
    texture_ = GL::Texture{ image, GL::InternalFormat::RGB };
    texture_.SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
}

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
    return Texture{ GL::Texture::Cubemap(image_ptrs, GL::InternalFormat::RGB), Type::CUBEMAP, 0 };
}

} // namespace App