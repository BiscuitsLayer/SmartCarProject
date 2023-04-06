#include "config_handler.hpp"

// Incomplete type resolve
#include <car_model/car_model.hpp>
#include <skybox/skybox.hpp>

namespace App {

// Extern variables
extern const int APP_GL_VEC3_COMPONENTS_COUNT;

ConfigHandler::ConfigHandler(const std::string& filename, const std::string& config_files_folder)
    : data_(std::make_shared<json>(json::parse(ReadFileData(filename, false)))), camera_case_selected_index_(-1) {
    if (data_ == nullptr || data_->is_discarded()) {
        throw std::runtime_error("Error reading JSON file: " + filename);
    }

	auto& context = App::Context::Get();

    auto window_config_filename = FindString(data_, "window_config");
    auto window_config_json = std::make_shared<json>(json::parse(ReadFileData(config_files_folder + window_config_filename, false)));
    SetWindowConfig(window_config_json);

    context.window.emplace(window_config_);
	GL::Context& gl = context.window->GetContext(
		App::APP_WINDOW_COLOR_BITS,
		App::APP_WINDOW_DEPTH_BITS,
		App::APP_WINDOW_STENCIL_BITS,
		App::APP_WINDOW_MULTISAMPLE_BITS
	);
	context.gl = context.window->GetContext();

    auto intersector_config_filename = FindString(data_, "intersector_config");
    auto intersector_config_json = std::make_shared<json>(json::parse(ReadFileData(config_files_folder + intersector_config_filename, false)));
    SetIntersectorConfig(intersector_config_json);

    auto cameras_config_filename = FindString(data_, "cameras_config");
    auto cameras_config_json = std::make_shared<json>(json::parse(ReadFileData(config_files_folder + cameras_config_filename, false)));
    SetCamerasConfigs(cameras_config_json);

    auto shaders_config_filename = FindString(data_, "shaders_config");
    auto shaders_config_json = std::make_shared<json>(json::parse(ReadFileData(config_files_folder + shaders_config_filename, false)));
    SetShaderHandler(shaders_config_json);
    context.shader_handler = shader_handler_;

    auto models_config_filename = FindString(data_, "models_config");
    auto models_config_json = std::make_shared<json>(json::parse(ReadFileData(config_files_folder + models_config_filename, false)));
    SetModelsConfigs(models_config_json);

    auto case_selected_index = FindInteger(data_, "case");
    auto cases = FindArray(data_, "cases");
    for (auto main_case : cases) {
        auto index = FindInteger(main_case, "index");
        if (index == case_selected_index) {
            case_selected_ = main_case;
        }
    }
    camera_case_selected_index_ = FindInteger(case_selected_, "camera_case");

    App::Timer loading_timer;

    ///// CAR /////

    loading_timer.Start();

    auto car = FindObject(case_selected_, "car");
    auto car_model_name = FindString(car, "name");

    auto car_model_config = std::dynamic_pointer_cast<Config::CarModelConfig>(models_configs_.at(car_model_name));
    car_model_config->transform = FindTransform(car, true);
    context.car_model = std::make_shared<App::CarModel>(*car_model_config);

    std::cout << "Model (" << car_model_name << ") loaded successfully in " << loading_timer.Stop<App::Timer::Milliseconds>() << " milliseconds" << std::endl;
    
    ///// SKYBOX /////

    loading_timer.Start();

    auto skybox = FindObject(case_selected_, "skybox");
    auto skybox_model_name = FindString(skybox, "name");

    auto skybox_config = std::dynamic_pointer_cast<Config::SkyboxModelConfig>(models_configs_.at(skybox_model_name));
	context.skybox = std::make_shared<App::Skybox>(*skybox_config);

    std::cout << "Model (" << skybox_model_name << ") loaded successfully in " << loading_timer.Stop<App::Timer::Milliseconds>() << " milliseconds" << std::endl;

    ///// ENVIRONMENT /////

    auto env = FindArray(case_selected_, "env");
    for (auto env_object : env) {    
        loading_timer.Start();

        auto env_object_name = FindString(env_object, "name");

        auto env_object_config = *std::dynamic_pointer_cast<Config::CommonModelConfig>(models_configs_.at(env_object_name));
        env_object_config.transform = FindTransform(env_object, true);

        context.env.push_back(std::make_shared<App::Model>(env_object_config));

        std::cout << "Model (" << env_object_name << ") loaded successfully in " << loading_timer.Stop<App::Timer::Milliseconds>() << " milliseconds" << std::endl;
    }

    ///// OBSTACLES /////

    auto obstacles = FindArray(case_selected_, "obstacles");
    for (auto obstacle_object : obstacles) {
        loading_timer.Start();

        auto obstacle_object_name = FindString(obstacle_object, "name");
        
        auto obstacle_config = *std::dynamic_pointer_cast<Config::CommonModelConfig>(models_configs_.at(obstacle_object_name));
        obstacle_config.transform = FindTransform(obstacle_object, true);
        
        context.obstacles.push_back(std::make_shared<App::Model>(obstacle_config));

        std::cout << "Model (" << obstacle_object_name << ") loaded successfully in " << loading_timer.Stop<App::Timer::Milliseconds>() << " milliseconds" << std::endl;
    }
}

Config::WindowConfig ConfigHandler::GetWindowConfig() const {
    return window_config_;
}

Config::IntersectorConfig ConfigHandler::GetIntersectorConfig() const {
    return intersector_config_;
}

Config::CameraConfig ConfigHandler::GetCameraConfig() const {
    if ((camera_case_selected_index_ < 0) && (camera_case_selected_index_ >= cameras_configs_.size())) {
        throw std::runtime_error("Wrong camera_case set");
    }
    return cameras_configs_[camera_case_selected_index_];
}

ShaderHandler ConfigHandler::GetShaderHandler() const {
    return shader_handler_;
}

void ConfigHandler::SetWindowConfig(const std::shared_ptr<json> window_json) {
    auto max_fps = FindObject(window_json, "max_fps");
    window_config_.max_fps.value = FindFloat(max_fps, "value");
    window_config_.max_fps.enabled = FindBoolean(max_fps, "enabled");

    auto params = FindObject(window_json, "params");
    window_config_.params.width = FindInteger(params, "width");
    window_config_.params.height = FindInteger(params, "height");
    window_config_.params.title = FindString(params, "title");
    window_config_.params.fullscreen = FindBoolean(params, "fullscreen");
};

void ConfigHandler::SetIntersectorConfig(const std::shared_ptr<json> intersector_json) {
    auto shader = FindObject(intersector_json, "shader");
    intersector_config_.shader.compute_shader_name = FindString(shader, "default");
    intersector_config_.enabled = FindBoolean(intersector_json, "enabled");
};

void ConfigHandler::SetCamerasConfigs(const std::shared_ptr<json> cameras_json) {
    auto cases = ConvertToArray(cameras_json);

    for (auto camera_case : cases) {
        Config::CameraConfig camera_config{};

        auto index = FindInteger(camera_case, "index");

        auto projection = FindObject(camera_case, "projection");
        camera_config.projection.FOV = FindFloat(projection, "FOV");
        camera_config.projection.near_plane = FindFloat(projection, "near");
        camera_config.projection.far_plane = FindFloat(projection, "far");

        auto length_to_target = FindObject(camera_case, "length_to_target");
        camera_config.length_to_target.min = FindFloat(length_to_target, "min");
        camera_config.length_to_target.max = FindFloat(length_to_target, "max");

        auto position = FindObject(camera_case, "position");
        camera_config.position.value = FindVec3(position, "value");
        camera_config.position.fixed_behind_car = FindBoolean(position, "fixed_behind_car");
        camera_config.position.translation_from_car = FindVec3(position, "translation_from_car");

        auto target = FindObject(camera_case, "target");
        camera_config.target.value = FindVec3(target, "value");
        camera_config.target.fixed_on_car = FindBoolean(target, "fixed_on_car");

        auto speed = FindObject(camera_case, "speed");
        camera_config.speed.move = FindFloat(speed, "move");
        camera_config.speed.rotate = FindFloat(speed, "rotate");

        if (cameras_configs_.size() <= index) {
            cameras_configs_.resize(index + 1);
        }
        cameras_configs_[index] = camera_config;
    }
};

void ConfigHandler::SetShaderHandler(const std::shared_ptr<json> shaders_json) {
    auto shaders = ConvertToArray(shaders_json);

    for (auto shader : shaders) {
        const std::string name = FindString(shader, "name");

        const std::string vertex = FindString(shader, "vertex", true);
        if (!vertex.empty()) {
            const std::string vertex_path = APP_SHADER_DIR + vertex;
            GL::Shader vertex_shader(GL::ShaderType::Vertex, App::ReadFileData(vertex_path, false));

            const std::string fragment_path = APP_SHADER_DIR + FindString(shader, "fragment", true);
            GL::Shader fragment_shader(GL::ShaderType::Fragment, App::ReadFileData(fragment_path, false));

            shader_handler_[name] = std::make_shared<GL::Program>(vertex_shader, fragment_shader);
        }

        const std::string compute = FindString(shader, "compute", true);
        if (!compute.empty()) {
            const std::string compute_path = APP_SHADER_DIR + compute;
            GL::Shader compute_shader(GL::ShaderType::Compute, App::ReadFileData(compute_path, false));

            shader_handler_[name] = std::make_shared<GL::Program>(compute_shader);
        }
    }
}

void ConfigHandler::SetModelsConfigs(const std::shared_ptr<json> models_json) {
    auto models = ConvertToArray(models_json);

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

void ConfigHandler::SetCarModelConfig(const json_object& car_object) {
    Config::CarModelConfig car_model_config;

    car_model_config.name = FindString(car_object, "name");
    car_model_config.type = FindString(car_object, "type");
    car_model_config.gltf = APP_ASSETS_DIR + FindString(car_object, "GLTF");

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

    car_model_config.acceleration = FindFloat(car_object, "acceleration");
    car_model_config.rotation_center = FindVec3(car_object, "rotation_center");

    auto shader = FindObject(car_object, "shader");
    car_model_config.shader.default_shader_name = FindString(shader, "default");
    car_model_config.shader.bbox_shader_name = FindString(shader, "bbox");

    models_configs_[car_model_config.name] = std::make_shared<Config::CarModelConfig>(car_model_config);
}

void ConfigHandler::SetSkyboxModelConfig(const json_object& skybox_object) {
    Config::SkyboxModelConfig skybox_model_config;

    skybox_model_config.name = FindString(skybox_object, "name");
    skybox_model_config.type = FindString(skybox_object, "type");
    skybox_model_config.folder = APP_ASSETS_DIR + FindString(skybox_object, "folder");

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
    skybox_model_config.shader.default_shader_name = FindString(shader, "default");

    models_configs_[skybox_model_config.name] = std::make_shared<Config::SkyboxModelConfig>(skybox_model_config);
}

void ConfigHandler::SetCommonModelConfig(const json_object& common_object) {
    Config::CommonModelConfig common_model_config;

    common_model_config.name = FindString(common_object, "name");
    common_model_config.type = FindString(common_object, "type");
    common_model_config.gltf = APP_ASSETS_DIR + FindString(common_object, "GLTF");

    auto shader = FindObject(common_object, "shader");
    common_model_config.shader.default_shader_name = FindString(shader, "default");
    common_model_config.shader.bbox_shader_name = FindString(shader, "bbox");

    models_configs_[common_model_config.name] = std::make_shared<Config::CommonModelConfig>(common_model_config);
}

const std::vector<json_object> ConfigHandler::ConvertToArray(const std::shared_ptr<json> array_json) const {
    if (!array_json->is_array()) {
        throw std::runtime_error("Incorrect JSON format: Cannot convert JSON to array");
    }

    std::vector<json_object> result{};
    result.reserve(array_json->size());
    for (auto entry = array_json->begin(); entry != array_json->end(); ++entry) {
        result.push_back(entry);
    }

    return result;
}

const json_object ConfigHandler::FindObject(std::shared_ptr<json> parent, const std::string& object_name, bool can_skip) const {
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

const json_object ConfigHandler::FindObject(const json_object& parent, const std::string& object_name, bool can_skip) const {
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

const std::vector<json_object> ConfigHandler::FindArray(std::shared_ptr<json> parent, const std::string& array_name, bool can_skip, std::vector<json_object> default_value) const {
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

const std::vector<json_object> ConfigHandler::FindArray(const json_object& parent, const std::string& array_name, bool can_skip, std::vector<json_object> default_value) const {
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

const float ConfigHandler::FindFloat(std::shared_ptr<json> parent, const std::string& number_name, bool can_skip, float default_value) const {
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

const float ConfigHandler::FindFloat(const json_object& parent, const std::string& number_name, bool can_skip, float default_value) const {
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

const int ConfigHandler::FindInteger(std::shared_ptr<json> parent, const std::string& number_name, bool can_skip, int default_value) const {
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

const int ConfigHandler::FindInteger(const json_object& parent, const std::string& number_name, bool can_skip, int default_value) const {
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

const bool ConfigHandler::FindBoolean(std::shared_ptr<json> parent, const std::string& boolean_name, bool can_skip, bool default_value) const {
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

const bool ConfigHandler::FindBoolean(const json_object& parent, const std::string& boolean_name, bool can_skip, bool default_value) const {
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

const std::string ConfigHandler::FindString(std::shared_ptr<json> parent, const std::string& string_name, bool can_skip, const std::string default_value) const {
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

const std::string ConfigHandler::FindString(const json_object& parent, const std::string& string_name, bool can_skip, const std::string default_value) const {
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

const GL::Vec3 ConfigHandler::FindVec3(std::shared_ptr<json> parent, const std::string& vector_name, bool can_skip, GL::Vec3 default_value) const {
    auto vector = parent->find(vector_name);
    if (vector == parent->end() || !vector->is_array()) {
        if (!can_skip) {
            throw std::runtime_error("Incorrect JSON format: Cannot find Vec3 " + vector_name);
        } else {
            return default_value;
        }
    }
    if (vector->size() != APP_GL_VEC3_COMPONENTS_COUNT) {
        throw std::runtime_error("Incorrect JSON format: Wrong number of Vec3 components " + vector_name);
    }

    GL::Vec3 result {
        vector.value()[0].get<float>(),
            vector.value()[1].get<float>(),
            vector.value()[2].get<float>()
    };
    return result;
}

const GL::Vec3 ConfigHandler::FindVec3(const json_object& parent, const std::string& vector_name, bool can_skip, GL::Vec3 default_value) const {
    auto vector = parent->find(vector_name);
    if (vector == parent->end() || !vector->is_array()) {
        if (!can_skip) {
            throw std::runtime_error("Incorrect JSON format: Cannot find Vec3 " + vector_name);
        } else {
            return default_value;
        }
    }
    if (vector->size() != APP_GL_VEC3_COMPONENTS_COUNT) {
        throw std::runtime_error("Incorrect JSON format: Wrong number of Vec3 components " + vector_name);
    }

    GL::Vec3 result {
        vector.value()[0].get<float>(),
            vector.value()[1].get<float>(),
            vector.value()[2].get<float>()
    };
    return result;
}

const Transform ConfigHandler::FindTransform(std::shared_ptr<json> parent, bool can_skip, Transform default_value) const {
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

const Transform ConfigHandler::FindTransform(const json_object& parent, bool can_skip, Transform default_value) const {
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

} // namespace App