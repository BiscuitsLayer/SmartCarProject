#pragma once

// STL
#include <fstream>
#include <memory>

// JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using json_object = nlohmann::detail::iter_impl<json>;

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <config/config_handler_fwd.hpp>

// LibSmartCar
#include <helpers/helpers.hpp>
#include <transform/transform.hpp>

// Incomplete type resolve
#include <car_model/car_model_fwd.hpp>
#include <skybox/skybox_fwd.hpp>

namespace App {

namespace Config {

struct Speed {
    float move;
    float rotate;
};

struct Shader {
    std::string default_shader_name;
    std::string bbox_shader_name;
    std::string compute_shader_name;
};

struct WindowConfig {
    struct MaxFps {
        int value;
        bool enabled;
    } max_fps;

    struct Params {
        int width;
        int height;
        std::string title;
        bool fullscreen;
    } params;
};

struct IntersectorConfig {
    Shader shader;
    bool enabled;
};

struct CameraConfig {
    struct Projection {
        float FOV;
        float near_plane;
        float far_plane;
    } projection;

    struct LengthToTarget {
        int min;
        int max;
    } length_to_target;

    struct Position {
        GL::Vec3 value;
        bool fixed_behind_car;
        GL::Vec3 translation_from_car;
    } position;
    struct Target {
        GL::Vec3 value;
        bool fixed_on_car;
    } target;
    Speed speed;
};

struct BaseModelConfig {
    std::string name;
    std::string type;
    Shader shader;

    // To make the class polymorphic, so we are able
    // to use down-casting with shared_ptr
    virtual ~BaseModelConfig() {}
};

struct CommonModelConfig: public BaseModelConfig {
    std::string gltf;
    Transform transform;
};

struct CarModelConfig: public BaseModelConfig {
    std::string gltf;
    struct Wheels {
        std::vector<std::string> mesh_names;
        Speed speed;
    } wheels;
    Speed speed;
    float acceleration;
    GL::Vec3 rotation_center;
    Transform transform;
};

struct SkyboxModelConfig: public BaseModelConfig {
    std::string folder;
    std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames;
};

} // namespace Config

class ConfigHandler {
public:
    ConfigHandler(const std::string& filename, const std::string& config_files_folder);

    Config::WindowConfig GetWindowConfig() const;
    Config::IntersectorConfig GetIntersectorConfig() const;
    Config::CameraConfig GetCameraConfig() const;
    ShaderHandler GetShaderHandler() const;

private:
    void SetWindowConfig(const std::shared_ptr<json> window_json);
    void SetIntersectorConfig(const std::shared_ptr<json> intersector_json);
    void SetCamerasConfigs(const std::shared_ptr<json> cameras_json);
    void SetShaderHandler(const std::shared_ptr<json> shaders_json);
    void SetModelsConfigs(const std::shared_ptr<json> models_json);

    void SetCarModelConfig(const json_object& car_object);
    void SetSkyboxModelConfig(const json_object& skybox_object);
    void SetCommonModelConfig(const json_object& common_object);

    const std::vector<json_object> ConvertToArray(const std::shared_ptr<json> array_json) const;

    const json_object FindObject(std::shared_ptr<json> parent, const std::string& object_name, bool can_skip = false) const;
    const json_object FindObject(const json_object& parent, const std::string& object_name, bool can_skip = false) const;

    const std::vector<json_object> FindArray(std::shared_ptr<json> parent, const std::string& array_name, bool can_skip = false, std::vector<json_object> default_value = {}) const;
    const std::vector<json_object> FindArray(const json_object& parent, const std::string& array_name, bool can_skip = false, std::vector<json_object> default_value = {}) const;

    const float FindFloat(std::shared_ptr<json> parent, const std::string& number_name, bool can_skip = false, float default_value = {}) const;
    const float FindFloat(const json_object& parent, const std::string& number_name, bool can_skip = false, float default_value = {}) const;

    const int FindInteger(std::shared_ptr<json> parent, const std::string& number_name, bool can_skip = false, int default_value = {}) const;
    const int FindInteger(const json_object& parent, const std::string& number_name, bool can_skip = false, int default_value = {}) const;

    const bool FindBoolean(std::shared_ptr<json> parent, const std::string& boolean_name, bool can_skip = false, bool default_value = {}) const;
    const bool FindBoolean(const json_object& parent, const std::string& boolean_name, bool can_skip = false, bool default_value = {}) const;

    const std::string FindString(std::shared_ptr<json> parent, const std::string& string_name, bool can_skip = false, std::string default_value = {}) const;
    const std::string FindString(const json_object& parent, const std::string& string_name, bool can_skip = false, std::string default_value = {}) const;

    const GL::Vec3 FindVec3(std::shared_ptr<json> parent, const std::string& vector_name, bool can_skip = false, GL::Vec3 default_value = {}) const;
    const GL::Vec3 FindVec3(const json_object& parent, const std::string& vector_name, bool can_skip = false, GL::Vec3 default_value = {}) const;

    const Transform FindTransform(std::shared_ptr<json> parent, bool can_skip = false, Transform default_value = {}) const;
    const Transform FindTransform(const json_object& parent, bool can_skip = false, Transform default_value = {}) const;

    const std::shared_ptr<json> data_;
    json_object case_selected_;
    int camera_case_selected_index_;

    Config::WindowConfig window_config_;
    Config::IntersectorConfig intersector_config_;
    std::vector<Config::CameraConfig> cameras_configs_;
    ShaderHandler shader_handler_;

    std::map<std::string, std::shared_ptr<Config::BaseModelConfig>> models_configs_;
};

} // namespace App