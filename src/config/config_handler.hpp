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
    Transform transform;

    // To make the class polymorphic, so we are able
    // to use down-casting with shared_ptr
    virtual ~BaseModelConfig() {}
};

struct CommonModelConfig: public BaseModelConfig {
    std::string gltf;
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
};

struct SkyboxModelConfig: public BaseModelConfig {
    std::string folder;
    std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames;
};

} // namespace Config

class ConfigHandler {
public:
    ConfigHandler(const std::string& filename);

    void ParseWindowConfig();
    void ParseIntersectorConfig();
    void ParseCameraConfig();
    void ParseShaders();
    void ParseModelConfigs();

    Config::WindowConfig GetWindowConfig() const;
    Config::IntersectorConfig GetIntersectorConfig() const;
    Config::CameraConfig GetCameraConfig() const;
    ShaderHandler GetShaderHandler() const;
    std::vector<std::shared_ptr<Config::BaseModelConfig>> GetModelConfigs() const;

private:
    void SetWindowConfig(const json_object& window);
    void SetIntersectorConfig(const json_object& intersector);
    void SetCameraConfig(const json_object& camera);
    void SetShaderHandler(const std::vector<json_object>& shaders);

    void SetCarModelConfig(const json_object& car_object);
    void SetSkyboxModelConfig(const json_object& skybox_object);
    void SetCommonModelConfig(const json_object& common_object);

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

    Config::WindowConfig window_config_;
    Config::IntersectorConfig intersector_config_;
    Config::CameraConfig camera_config_;
    ShaderHandler shader_handler_;
    std::vector<std::shared_ptr<Config::BaseModelConfig>> model_configs_;
};

} // namespace App