#pragma once

// STL
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <optional>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <constants/constants.hpp>

namespace App {

class Camera; // Forward declaration
class Model; // Forward declaration

using ShaderHandler = std::unordered_map<std::string, std::shared_ptr<GL::Program>>;

enum class KeyboardMode: int {
    ORBIT_CAMERA = 0,
    FIRST_PERSON_CAMERA,
    CAR_MOVEMENT
};

using KeyboardStatus = std::array<bool, APP_KEYBOARD_KEYS_COUNT>;

struct MemoryAlignedBBox {
    MemoryAlignedBBox(GL::Vec3 new_min, GL::Vec3 new_max, GL::Mat4 new_model, GL::Mat4 new_mesh_to_model);

    GL::Vec4 min_point;
    GL::Vec4 max_point;
    GL::Mat4 model;
    GL::Mat4 mesh_to_model;
};

/*
    Singleton class to share
    application resources between files
*/
struct Context {
public:
    static Context& Get();

    Context(const Context&) = delete;
    Context& operator=(Context&) = delete;

    std::optional<std::reference_wrapper<GL::Context>> gl;
    std::optional<ShaderHandler> shader_handler;
    std::shared_ptr<Camera> camera;
    std::optional<GL::Mat4> projection_matrix;
    std::optional<KeyboardMode> keyboard_mode;
    std::optional<KeyboardStatus> keyboard_status;
    std::vector<std::shared_ptr<Model>> models;

private:
    Context() {}
};

GL::Vec3 GetTranslation(GL::Mat4 matrix);

std::string ReadFileData(std::string filename, bool debug_dump = true);

} // namespace App