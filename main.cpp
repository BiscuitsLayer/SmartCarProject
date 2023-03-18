// STL
#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// ImGui
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

// Sources
#include <constants/constants.hpp>
#include <helpers/helpers.hpp>
#include <transform/transform.hpp>
#include <camera/camera.hpp>
#include <gui/gui.hpp>
#include <timer/timer.hpp>
#include <window/window.hpp>
#include <config/config_handler.hpp>

// Model
// #include <cube_example/src/cube.hpp>
#include <model/model.hpp>
#include <car/car.hpp>
#include <skybox/skybox.hpp>

// Coordinate space matrices
// model: local space -> world space
// view: world space -> camera space
// projection: specified range of space in camera space (seen by camera) -> [-1.0, 1.0] (NDC), 
// clip coordinates out of range

int main() try {
	auto& context = App::Context::Get();
	context.keyboard_mode = App::KeyboardMode::CAR_MOVEMENT;
	context.keyboard_status = App::KeyboardStatus{};
	context.keyboard_status.value().fill(false);

	App::ConfigHandler config_handler{"../config.json"};

	config_handler.ParseWindowConfig();
	auto window_config = config_handler.GetWindowConfig();

	config_handler.ParseCameraConfig();
	auto camera_config = config_handler.GetCameraConfig();

	context.projection_matrix = GL::Mat4::Perspective(
		GL::Rad(camera_config.projection.FOV),
		static_cast<float>(window_config.params.width) / static_cast<float>(window_config.params.height),
		camera_config.projection.near_plane, camera_config.projection.far_plane
	);

	App::CustomWindow window(window_config);
	GL::Context& gl = window.GetContext();
	context.gl = window.GetContext();

	gl.Enable(GL::Capability::DepthTest);
	gl.Enable(GL::Capability::Blend);
	gl.BlendFunc(GL::BlendingFactor::SourceAlpha, GL::BlendingFactor::OneMinusSourceAlpha);

	context.camera = std::make_shared<App::Camera>(camera_config);

	config_handler.ParseShaders();
	context.shader_handler = config_handler.GetShaderHandler();

	config_handler.ParseModelConfigs();
	auto model_configs = config_handler.GetModelConfigs();

	std::vector<std::shared_ptr<App::Model>> models;

	for (auto model_config : model_configs) {
		if (model_config->type == "CAR") {
			auto car_model_config = std::dynamic_pointer_cast<App::Config::CarModelConfig>(model_config);
			auto car_model = std::make_shared<App::CarModel>(*car_model_config);
			models.push_back(car_model);
		} else if (model_config->type == "SKYBOX") {
			auto skybox_model_config = std::dynamic_pointer_cast<App::Config::SkyboxModelConfig>(model_config);
			auto skybox_model = std::make_shared<App::Skybox>(*skybox_model_config);
			models.push_back(skybox_model);
		} else { // model_config->type == "COMMON"
			auto common_model_config = std::dynamic_pointer_cast<App::Config::CommonModelConfig>(model_config);
			auto model = std::make_shared<App::Model>(*common_model_config);
			models.push_back(model);
		}
	}

	App::Gui gui(window_config);

	App::Timer main_timer;
	main_timer.Start();

	GL::Event ev;
	while (window.IsOpen()) {
		auto delta_time = static_cast<float>(main_timer.Tick<App::Timer::Seconds>());

		while (window.GetEvent(ev)) {
			if (ev.Type == GL::Event::KeyDown) {
				context.keyboard_status.value()[ev.Key.Code] = true;
			} else if (ev.Type == GL::Event::KeyUp) {
				context.keyboard_status.value()[ev.Key.Code] = false;
			}

			if (context.keyboard_status.value()[GL::Key::Escape]) {
				window.Close();
			}
		}

		std::dynamic_pointer_cast<App::CarModel>(models[0])->Move(delta_time);

		gl.Clear(GL::Buffer::Color | GL::Buffer::Depth);
		gui.Prepare();

		context.camera.value()->UpdateWithModel(models[0]->GetModelMatrix());

		for (auto model : models) {
			model->Draw();
		}
		gui.Draw();

		window.Present();
		App::LimitMaxFps(window_config);
	}

	gui.Cleanup();
	return 0;
}
catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return 0;
}