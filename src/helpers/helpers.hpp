#pragma once

// STL
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <optional>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <helpers/helpers_fwd.hpp>

// Complete types
#include <camera/camera.hpp>
#include <model/model.hpp>

namespace App {

struct MemoryAlignedBBox {
    MemoryAlignedBBox(const GL::Vec3& new_min, const GL::Vec3& new_max,
        const GL::Mat4& new_model, const GL::Mat4& new_mesh_to_model);

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
    std::optional<Camera> camera;
    std::optional<GL::Mat4> projection_matrix;
    std::optional<KeyboardMode> keyboard_mode;
    std::optional<KeyboardStatus> keyboard_status;
    std::vector<std::shared_ptr<Model>> models;

private:
    Context();
};

GL::Vec3 GetTranslation(const GL::Mat4& matrix);

std::string ReadFileData(const std::string& filename, bool debug_dump = true);

} // namespace App