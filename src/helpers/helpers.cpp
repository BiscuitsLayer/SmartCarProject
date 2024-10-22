#include "helpers.hpp"

// Incomplete type resolve
#include <car_model/car_model.hpp>
#include <skybox/skybox.hpp>

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

void Context::ClearCarTransform() {
    // Update car movement transform
    car_model->precomputed_movement_transform_ = Transform{};
    car_model->UpdateMovementTransform();
    car_model->accelerator_.Stop();
    
    // Update camera target and position
    camera->reached_final_position_ = false;
}

Context::Context()
    : gl(std::nullopt), shader_handler(std::nullopt), camera(std::nullopt), projection_matrix(std::nullopt),
    keyboard_mode(std::nullopt), keyboard_status(std::nullopt), env({}), obstacles({}) {}

GL::Vec3 GetTranslation(const GL::Mat4& matrix) {
    return GL::Vec3{matrix.m[12], matrix.m[13], matrix.m[14]};
}

std::string GetLastSavedFileWithPrefix(const std::string& path, const std::string& prefix) {
    std::filesystem::file_time_type ans_last_mod_time;
    std::string ans{};

    bool is_first = true;
    for (auto entry : std::filesystem::directory_iterator(path)) {
        std::string entry_filepath = entry.path().u8string();
        std::string entry_filename = GetFilenameFromPath(entry_filepath);
        if (entry_filename.substr(0, prefix.size()) != prefix) {
            continue;
        }
        std::filesystem::file_time_type last_mod_time = std::filesystem::last_write_time(entry);
        if ((last_mod_time >= ans_last_mod_time) || is_first) {
            is_first = false;
            ans_last_mod_time = last_mod_time;
            ans = entry_filepath;
        }
    }

    return ans;
}

std::string GetFilenameFromPath(const std::string& path) {
    size_t starting_index = path.find_last_of('/') + 1;
    if (starting_index >= path.size()) {
        return {};
    }
    return path.substr(starting_index);
}

std::string GetFolderFromPath(const std::string& path) {
    return path.substr(0, path.find_last_of('/')) + '/';
}

bool CheckIfFileExists(const std::string& path) {
    std::ifstream file{path};
    return file.good();
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

void SaveToFile(const std::string& filename, const std::vector<unsigned char>& buffer, bool debug_dump) {
    // WARNING: using modified ofstream instead of std::ofstream to write unsigned chars
    std::basic_fstream<unsigned char, std::char_traits<unsigned char>> destination;
    destination.open(filename, std::fstream::out | std::fstream::binary);
    if (!destination) {
        throw std::runtime_error("SaveToFile: can't open file: " + filename);
    }

    destination.write(buffer.data(), buffer.size());

    if (debug_dump) {
        std::cout << std::string{buffer.begin(), buffer.end()} << std::endl;
    }
}

} // namespace App