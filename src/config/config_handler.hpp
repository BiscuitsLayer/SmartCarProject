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

struct WindowConfig {
    struct MaxFps {
        int value;
        bool mode;
    } max_fps;

    struct Params {
        int width;
        int height;
        std::string title;
        bool fullscreen;
    } params;
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
    struct Shader {
        std::string default_shader_name;
        std::string bbox_shader_name;
        std::string skybox_shader_name;
    } shader;
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
    ConfigHandler(std::string filename) {
        data_ = std::make_shared<json>(json::parse(ReadFileData(filename, false)));
        if (data_ == nullptr || data_->is_discarded()) {
            data_ = nullptr;
            throw std::runtime_error("Error reading JSON file: " + filename);
        }

        auto case_index = FindInteger(data_, "case");
        case_selected_ = FindArray(data_, "cases")[case_index];
    }

    void ParseWindowConfig() {
        auto window = FindObject(data_, "window");
        SetWindowConfig(window);
    }

    void ParseCameraConfig() {
        auto camera = FindObject(case_selected_, "camera");
        SetCameraConfig(camera);
    }

    void ParseShaders() {
        auto shaders = FindArray(data_, "shaders");
        SetShaderHandler(shaders);
    }

    void ParseModelConfigs() {
        auto models = FindArray(case_selected_, "models");
        model_configs_.reserve(models.size());

        for (auto model : models) {
            auto type = FindString(model, "type");

            if (type == "CAR") {
                SetCarModelConfig(model);
            } else if (type == "SKYBOX") {
                SetSkyboxModelConfig(model);
            } else { // type == "COMMON"
                SetCommonModelConfig(model);
            }
        }
    }

    Config::WindowConfig GetWindowConfig() const {
        return window_config_;
    }

    Config::CameraConfig GetCameraConfig() const {
        return camera_config_;
    }

    ShaderHandler GetShaderHandler() const {
        return shader_handler_;
    }

    std::vector<std::shared_ptr<Config::BaseModelConfig>> GetModelConfigs() const {
        return model_configs_;
    }

private:
    void SetWindowConfig(json_object window) {
        auto max_fps = FindObject(window, "max_fps");
        window_config_.max_fps.value = FindFloat(max_fps, "value");
        window_config_.max_fps.mode = FindBoolean(max_fps, "mode");

        auto params = FindObject(window, "params");
        window_config_.params.width = FindInteger(params, "width");
        window_config_.params.height = FindInteger(params, "height");
        window_config_.params.title = FindString(params, "title");
        window_config_.params.fullscreen = FindBoolean(params, "fullscreen");
    };

    void SetCameraConfig(json_object camera) {
        auto projection = FindObject(camera, "projection");
        camera_config_.projection.FOV = FindFloat(projection, "FOV");
        camera_config_.projection.near_plane = FindFloat(projection, "near");
        camera_config_.projection.far_plane = FindFloat(projection, "far");

        auto length_to_target = FindObject(camera, "length_to_target");
        camera_config_.length_to_target.min = FindFloat(length_to_target, "min");
        camera_config_.length_to_target.max = FindFloat(length_to_target, "max");


        auto position = FindObject(camera, "position");
        camera_config_.position.value = FindVec3(position, "value");
        camera_config_.position.fixed_behind_car = FindBoolean(position, "fixed_behind_car");
        camera_config_.position.translation_from_car = FindVec3(position, "translation_from_car");

        auto target = FindObject(camera, "target");
        camera_config_.target.value = FindVec3(target, "value");
        camera_config_.target.fixed_on_car = FindBoolean(target, "fixed_on_car");

        auto speed = FindObject(camera, "speed");
        camera_config_.speed.move = FindFloat(speed, "move");
        camera_config_.speed.rotate = FindFloat(speed, "rotate");
    };

    void SetShaderHandler(std::vector<json_object> shaders) {
        for (auto shader : shaders) {
            std::string name = FindString(shader, "name");
            std::string folder = FindString(shader, "folder");

            std::string vertex_path = folder + "/" + FindString(shader, "vertex");
            GL::Shader vertex_shader(GL::ShaderType::Vertex, App::ReadFileData(vertex_path, false));

            std::string fragment_path = folder + "/" + FindString(shader, "fragment");
            GL::Shader fragment_shader(GL::ShaderType::Fragment, App::ReadFileData(fragment_path, false));

            shader_handler_[name] = std::make_shared<GL::Program>(vertex_shader, fragment_shader);
        }
    }

    void SetCarModelConfig(json_object car_object) {
        Config::CarModelConfig car_model_config;

        car_model_config.name = FindString(car_object, "name");
        car_model_config.type = FindString(car_object, "type");
        car_model_config.gltf = FindString(car_object, "GLTF");
        car_model_config.transform = FindTransform(car_object, true);

        auto wheels = FindObject(car_object, "wheels");

        auto mesh_names = FindArray(wheels, "mesh_names");
        for (auto mesh_name : mesh_names) {
            car_model_config.wheels.mesh_names.push_back(mesh_name->is_string() ? *mesh_name : "");
        }
        auto wheels_speed = FindObject(wheels, "speed");
        car_model_config.wheels.speed.rotate = FindFloat(wheels_speed, "rotate");

        auto speed = FindObject(car_object, "speed");
        car_model_config.speed.move = FindFloat(speed, "move");
        car_model_config.speed.rotate = FindFloat(speed, "rotate");

        car_model_config.rotation_center = FindVec3(car_object, "rotation_center");

        auto shader = FindObject(car_object, "shader");
        car_model_config.shader.default_shader_name = FindString(shader, "default");
        car_model_config.shader.bbox_shader_name = FindString(shader, "bbox");

        model_configs_.push_back(std::make_shared<Config::CarModelConfig>(car_model_config));
    }

    void SetSkyboxModelConfig(json_object skybox_object) {
        Config::SkyboxModelConfig skybox_model_config;

        skybox_model_config.name = FindString(skybox_object, "name");
        skybox_model_config.type = FindString(skybox_object, "type");
        skybox_model_config.folder = FindString(skybox_object, "folder");

        auto filenames = FindArray(skybox_object, "filenames");
        if (filenames.size() != APP_CUBEMAP_TEXTURES_COUNT) {
            throw std::runtime_error("Incorrect JSON: cubemap textures count != 6");
        }

        int index = 0;
        for (auto filename : filenames) {
            skybox_model_config.filenames[index] = filename->is_string() ? *filename : "";
            ++index;
        }

        auto shader = FindObject(skybox_object, "shader");
        skybox_model_config.shader.skybox_shader_name = FindString(shader, "skybox");

        model_configs_.push_back(std::make_shared<Config::SkyboxModelConfig>(skybox_model_config));
    }

    void SetCommonModelConfig(json_object common_object) {
        Config::CommonModelConfig common_model_config;

        common_model_config.name = FindString(common_object, "name");
        common_model_config.type = FindString(common_object, "type");
        common_model_config.gltf = FindString(common_object, "GLTF");
        common_model_config.transform = FindTransform(common_object, true);

        auto shader = FindObject(common_object, "shader");
        common_model_config.shader.default_shader_name = FindString(shader, "default");
        common_model_config.shader.bbox_shader_name = FindString(shader, "bbox");

        model_configs_.push_back(std::make_shared<Config::CommonModelConfig>(common_model_config));
    }

    json_object FindObject(std::shared_ptr<json> parent, std::string object_name, bool can_skip = false) {
        auto object = parent->find(object_name);
        if (object == parent->end() || !object->is_object()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find object " + object_name);
            } else {
                return parent->end();
            }
        }
        return object;
    }

    json_object FindObject(json_object parent, std::string object_name, bool can_skip = false) {
        auto object = parent->find(object_name);
        if (object == parent->end() || !object->is_object()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find object " + object_name);
            } else {
                return parent->end();
            }
        }
        return object;
    }

    std::vector<json_object> FindArray(std::shared_ptr<json> parent, std::string array_name, bool can_skip = false, std::vector<json_object> default_value = {}) {
        auto array = parent->find(array_name);
        if (array == parent->end() || !array->is_array()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find array " + array_name);
            } else {
                return default_value;
            }
        }

        std::vector<json_object> result{};
        result.reserve(array->size());
        for (auto entry = array->begin(); entry != array->end(); ++entry) {
            result.push_back(entry);
        }

        return result;
    }

    std::vector<json_object> FindArray(json_object parent, std::string array_name, bool can_skip = false, std::vector<json_object> default_value = {}) {
        auto array = parent->find(array_name);
        if (array == parent->end() || !array->is_array()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find array " + array_name);
            } else {
                return default_value;
            }
        }

        std::vector<json_object> result{};
        result.reserve(array->size());
        for (auto entry = array->begin(); entry != array->end(); ++entry) {
            result.push_back(entry);
        }

        return result;
    }

    float FindFloat(json_object parent, std::string number_name, bool can_skip = false, float default_value = {}) {
        auto number = parent->find(number_name);
        if (number == parent->end() || !number->is_number_float()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find float " + number_name);
            } else {
                return default_value;
            }
        }
        return number->get<float>();
    }

    int FindInteger(std::shared_ptr<json> parent, std::string number_name, bool can_skip = false, int default_value = {}) {
        auto number = parent->find(number_name);
        if (number == parent->end() || !number->is_number_integer()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find integer " + number_name);
            } else {
                return default_value;
            }
        }
        return number->get<int>();
    }

    int FindInteger(json_object parent, std::string number_name, bool can_skip = false, int default_value = {}) {
        auto number = parent->find(number_name);
        if (number == parent->end() || !number->is_number_integer()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find integer " + number_name);
            } else {
                return default_value;
            }
        }
        return number->get<int>();
    }

    bool FindBoolean(json_object parent, std::string boolean_name, bool can_skip = false, bool default_value = {}) {
        auto boolean = parent->find(boolean_name);
        if (boolean == parent->end() || !boolean->is_boolean()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find boolean " + boolean_name);
            } else {
                return default_value;
            }
        }
        return boolean->get<bool>();
    }

    std::string FindString(json_object parent, std::string string_name, bool can_skip = false, std::string default_value = {}) {
        auto str = parent->find(string_name);
        if (str == parent->end() || !str->is_string()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find string " + string_name);
            } else {
                return default_value;
            }
        }
        return *str;
    }

    GL::Vec3 FindVec3(json_object parent, std::string vector_name, bool can_skip = false, GL::Vec3 default_value = {}) {
        auto vector = parent->find(vector_name);
        if (vector == parent->end() || !vector->is_array()) {
            if (!can_skip) {
                throw std::runtime_error("Incorrect JSON format: Cannot find Vec3 " + vector_name);
            } else {
                return default_value;
            }
        }
        if (vector->size() != APP_VEC3_COMPONENTS_COUNT) {
            throw std::runtime_error("Incorrect JSON format: Wrong number of Vec3 components " + vector_name);
        }

        GL::Vec3 result {
            vector.value()[0].get<float>(),
                vector.value()[1].get<float>(),
                vector.value()[2].get<float>()
        };
        return result;
    }

    Transform FindTransform(json_object parent, bool can_skip = false, Transform default_value = {}) {
        Transform result{};

        auto transform = FindObject(parent, "transform", can_skip);
        if (transform == parent->end()) {
            return default_value;
        }

        result.SetScale(FindVec3(transform, "scale", true, GL::Vec3{1.0f, 1.0f, 1.0f}));
        auto rotation = FindObject(transform, "rotation", true);
        if (rotation != transform->end()) {
            result.SetRotation(FindFloat(rotation, "angle"), FindVec3(rotation, "axis"));
        }
        result.SetTranslation(FindVec3(transform, "translation", true, GL::Vec3{0.0f, 0.0f, 0.0f}));

        return result;
    }

    std::shared_ptr<json> data_;
    json_object case_selected_;

    Config::WindowConfig window_config_;
    Config::CameraConfig camera_config_;
    ShaderHandler shader_handler_;
    std::vector<std::shared_ptr<Config::BaseModelConfig>> model_configs_;
};

} // namespace App