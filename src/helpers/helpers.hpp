#pragma once

// STL
#include <unordered_map>
#include <memory>
#include <optional>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <constants/constants.hpp>

namespace App {

class Camera; // Forward declaration

using ShaderHandler = std::unordered_map<std::string, std::shared_ptr<GL::Program>>;

enum class KeyboardMode: int {
    ORBIT_CAMERA = 0,
    FIRST_PERSON_CAMERA,
    CAR_MOVEMENT
};

using KeyboardStatus = std::array<bool, APP_KEYBOARD_KEYS_COUNT>;

/*
    Singleton class to share
    application resources between files
*/
struct Context {
public:
    static Context& Get() {
        static Context instance;
        return instance;
    }

    Context(const Context&) = delete;
    Context& operator=(Context&) = delete;

    std::optional<std::reference_wrapper<GL::Context>> gl;
    std::optional<ShaderHandler> shader_handler;
    std::optional<std::shared_ptr<Camera>> camera;
    std::optional<GL::Mat4> projection_matrix;
    std::optional<KeyboardMode> keyboard_mode;
    std::optional<KeyboardStatus> keyboard_status;

private:
    Context() {}
};

GL::Vec3 GetTranslation(GL::Mat4 matrix) {
    return GL::Vec3{matrix.m[12], matrix.m[13], matrix.m[14]};
}

std::string ReadFileData(std::string filename, bool debug_dump = true) {
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