#include "helpers.hpp"

namespace App {

MemoryAlignedBBox::MemoryAlignedBBox(GL::Vec3 new_min, GL::Vec3 new_max, GL::Mat4 new_model, GL::Mat4 new_mesh_to_model)
    : min_point(GL::Vec4(new_min.X, new_min.Y, new_min.Z, 1.0)), max_point(GL::Vec4(new_max.X, new_max.Y, new_max.Z, 1.0)),
    model(new_model), mesh_to_model(new_mesh_to_model) {}

Context& Context::Get() {
    static Context instance;
    return instance;
}

GL::Vec3 GetTranslation(GL::Mat4 matrix) {
    return GL::Vec3{matrix.m[12], matrix.m[13], matrix.m[14]};
}

std::string ReadFileData(std::string filename, bool debug_dump) {
    std::ifstream source{filename};
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