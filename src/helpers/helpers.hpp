#pragma once

// STL
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <optional>
#include <chrono>
#include <filesystem>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <helpers/helpers_fwd.hpp>

// LibSmartCar
#include <camera/camera.hpp>
#include <model/model.hpp>
#include <window/window.hpp>

// Incomplete type resolve
#include <car_model/car_model_fwd.hpp>
#include <skybox/skybox_fwd.hpp>

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
    std::optional<CustomWindow> window;

    std::shared_ptr<CarModel> car_model;
    std::shared_ptr<Skybox> skybox;
    std::vector<std::shared_ptr<Model>> env;
    std::vector<std::shared_ptr<Model>> obstacles;

    // INPUT TO NEURAL NETWORK
    std::array<float, APP_RAY_INTERSECTOR_RAYS_COUNT> distances_from_rays;
    std::array<float, APP_CAR_STATE_PARAMETERS_COUNT> state;

    // OUTPUT FROM NEURAL NETWORK
    std::array<bool, APP_CAR_ACTIONS_COUNT> actions;
    std::array<bool, APP_CAR_ACTIONS_COUNT> user_selected_actions; // TODO: for supervides learning (should get rid of it)

    void ClearCarTransform();

private:
    Context();
};

GL::Vec3 GetTranslation(const GL::Mat4& matrix);

std::string GetLastSavedFileWithPrefix(const std::string& path, const std::string& prefix);
std::string GetFilenameFromPath(const std::string& path);
std::string GetFolderFromPath(const std::string& path);

bool CheckIfFileExists(const std::string& path);
std::string ReadFileData(const std::string& filename, bool debug_dump = true);
void SaveToFile(const std::string& filename, const std::vector<unsigned char>& buffer, bool debug_dump = true);

} // namespace App