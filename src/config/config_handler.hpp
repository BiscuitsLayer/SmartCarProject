#pragma once

// STL
#include <fstream>
#include <memory>

// JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using json_object = nlohmann::detail::iter_impl<json>;

// Sources
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

    struct {
        GL::Vec3 value;
        bool fixed_behind_car;
        GL::Vec3 translation_from_car;
    } position;
    struct {
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
    GL::Vec3 rotation_center;
};

struct SkyboxModelConfig: public BaseModelConfig {
    std::string folder;
    std::array<std::string, APP_CUBEMAP_TEXTURES_COUNT> filenames;
};

} // namespace Config

class ConfigHandler {
public:
    ConfigHandler(std::string filename);

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
    void SetWindowConfig(json_object window);
    void SetIntersectorConfig(json_object intersector);
    void SetCameraConfig(json_object camera);
    void SetShaderHandler(std::vector<json_object> shaders);

    void SetCarModelConfig(json_object car_object);
    void SetSkyboxModelConfig(json_object skybox_object);
    void SetCommonModelConfig(json_object common_object);

    json_object FindObject(std::shared_ptr<json> parent, std::string object_name, bool can_skip = false);
    json_object FindObject(json_object parent, std::string object_name, bool can_skip = false);

    std::vector<json_object> FindArray(std::shared_ptr<json> parent, std::string array_name, bool can_skip = false, std::vector<json_object> default_value = {});
    std::vector<json_object> FindArray(json_object parent, std::string array_name, bool can_skip = false, std::vector<json_object> default_value = {});

    float FindFloat(std::shared_ptr<json> parent, std::string number_name, bool can_skip = false, float default_value = {});
    float FindFloat(json_object parent, std::string number_name, bool can_skip = false, float default_value = {});

    int FindInteger(std::shared_ptr<json> parent, std::string number_name, bool can_skip = false, int default_value = {});
    int FindInteger(json_object parent, std::string number_name, bool can_skip = false, int default_value = {});

    bool FindBoolean(std::shared_ptr<json> parent, std::string boolean_name, bool can_skip = false, bool default_value = {});
    bool FindBoolean(json_object parent, std::string boolean_name, bool can_skip = false, bool default_value = {});

    std::string FindString(std::shared_ptr<json> parent, std::string string_name, bool can_skip = false, std::string default_value = {});
    std::string FindString(json_object parent, std::string string_name, bool can_skip = false, std::string default_value = {});

    GL::Vec3 FindVec3(std::shared_ptr<json> parent, std::string vector_name, bool can_skip = false, GL::Vec3 default_value = {});
    GL::Vec3 FindVec3(json_object parent, std::string vector_name, bool can_skip = false, GL::Vec3 default_value = {});

    Transform FindTransform(std::shared_ptr<json> parent, bool can_skip = false, Transform default_value = {});
    Transform FindTransform(json_object parent, bool can_skip = false, Transform default_value = {});

    std::shared_ptr<json> data_;
    json_object case_selected_;

    Config::WindowConfig window_config_;
    Config::IntersectorConfig intersector_config_;
    Config::CameraConfig camera_config_;
    ShaderHandler shader_handler_;
    std::vector<std::shared_ptr<Config::BaseModelConfig>> model_configs_;
};

} // namespace App