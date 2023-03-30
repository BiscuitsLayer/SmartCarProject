#include "helpers.hpp"

namespace App {

// Extern variables
/* empty */

MemoryAlignedBBox::MemoryAlignedBBox(const GL::Vec3& new_min, const GL::Vec3& new_max,
    const GL::Mat4& new_model, const GL::Mat4& new_mesh_to_model)
    : min_point(GL::Vec4(new_min.X, new_min.Y, new_min.Z, 1.0)), max_point(GL::Vec4(new_max.X, new_max.Y, new_max.Z, 1.0)),
    model(new_model), mesh_to_model(new_mesh_to_model) {}

Context& Context::Get() {
    static Context instance;
    return instance;
}

Context::Context()
    : gl(std::nullopt), shader_handler(std::nullopt), camera(std::nullopt), projection_matrix(std::nullopt),
    keyboard_mode(std::nullopt), keyboard_status(std::nullopt), models({}) {}

GL::Vec3 GetTranslation(const GL::Mat4& matrix) {
    return GL::Vec3{matrix.m[12], matrix.m[13], matrix.m[14]};
}

std::string ReadFileData(const std::string& filename, bool debug_dump) {
    std::ifstream source(filename, std::ios::binary);
    if (!source) {
        throw std::runtime_error("ReadFileData: can't open file: " + filename);
    }

    source.seekg(0, std::ios::end);
    size_t length = source.tellg();
    source.seekg(0, std::ios::beg);

    std::string buffer(length, 0);
    source.read(buffer.data(), length);

    if (debug_dump) {
        std::cout << buffer << std::endl;
    }
    return buffer;
}

} // namespace App