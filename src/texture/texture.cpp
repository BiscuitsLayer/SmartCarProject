#include "texture.hpp"

// Incomplete type resolve
#include <helpers/helpers.hpp>

namespace App {

// Extern variables
/* empty */

Texture::Texture(GL::Texture texture, GL::Vec4 factor)
    : texture_(texture), factor_(factor) {
    texture_->SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
}

Texture::Texture(std::string path, GL::Vec4 factor)
    : texture_(std::nullopt), factor_(factor) {
    auto image_ptr = LoadSerializedData(path);
    texture_ = GL::Texture{*image_ptr, GL::InternalFormat::RGB};
    texture_->SetWrapping(GL::Wrapping::Repeat, GL::Wrapping::Repeat);
}

Texture::Texture(GL::Texture texture)
    : Texture(texture, GL::Vec4{}) {}

Texture::Texture(std::string path)
    : Texture(path, GL::Vec4{}) {}

Texture::Texture(GL::Vec4 factor)
    : texture_(std::nullopt), factor_(factor) {}

Texture::Texture()
    : texture_(std::nullopt), factor_(GL::Vec4{}) {}

Texture Texture::Cubemap(std::string path, std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames) {
    std::string buffer;
    std::array<std::shared_ptr<GL::Image>, APP_CUBEMAP_TEXTURES_COUNT> image_ptrs;

    unsigned int index = 0;
    for (auto filename : filenames) {
        std::string full_path = path + "/" + filename;
        image_ptrs[index] = LoadSerializedData(full_path);
        ++index;
    }

    return Texture{GL::Texture::Cubemap(image_ptrs, GL::InternalFormat::RGB)};
}

std::shared_ptr<GL::Image> Texture::LoadSerializedData(const std::string& path) {
    auto image_ptr = std::make_shared<GL::Image>();
    std::string serialized_filename = APP_SERIALIZED_FILES_DIR + GetFilenameFromPath(path) + ".serialized";
    
    try {
        auto serialized_data = ReadFileData(serialized_filename, false);
        std::cout << "\tSerialized texture (" << path << ") found" << std::endl;
        std::vector<GL::uchar> serialized_data_reformatted{serialized_data.begin(), serialized_data.end()};
        image_ptr->Deserialize(serialized_data_reformatted.data());
        std::cout << "\tSerialized texture (" << path << ") loaded successfully" << std::endl;
    } catch (std::exception& e) {
        std::string buffer = App::ReadFileData(path, false);
        std::cout << "\tLoading texture (" << path << ") from raw data" << std::endl;
        image_ptr->Load(reinterpret_cast<unsigned char*>(buffer.data()), static_cast<GL::uint>(buffer.size()));

        std::cout << "\tSaving texture (" << path << ") in serialized data" << std::endl;
        std::vector<GL::uchar> serialized_data(image_ptr->GetSerializedBufferSize(), 0);
        image_ptr->Serialize(serialized_data.data());
        SaveToFile(serialized_filename, serialized_data, false);
        std::cout << "\tSaved texture (" << path << ") successfully!" << std::endl;
    }

    return image_ptr;
}

} // namespace App