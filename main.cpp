// LibSmartCar
#include <helpers/helpers.hpp>
#include <transform/transform.hpp>
#include <camera/camera.hpp>
#include <gui/gui.hpp>
#include <timer/timer.hpp>
#include <window/window.hpp>
#include <config/config_handler.hpp>
#include <intersector/intersector.hpp>

// Model
#include <model/model.hpp>
#include <car_model/car_model.hpp>
#include <skybox/skybox.hpp>

// Coordinate space matrices
// model: local space -> world space
// view: world space -> camera space
// projection: specified range of space in camera space (seen by camera) -> [-1.0, 1.0] (NDC), 
// clip coordinates out of range

int main(int argc, char **argv) try {
	auto& context = App::Context::Get();
	context.keyboard_mode = App::KeyboardMode::CAR_MOVEMENT;
	context.keyboard_status = App::KeyboardStatus{};
	context.keyboard_status->fill(false);

	if (argc != 2) {
		throw std::runtime_error("Wrong number of arguments!\nUsage: ./SmartCarMain.exe <config file path>");
	}
	App::ConfigHandler config_handler{argv[1]};

	config_handler.ParseWindowConfig();
	auto window_config = config_handler.GetWindowConfig();

	config_handler.ParseIntersectorConfig();
	auto intersector_config = config_handler.GetIntersectorConfig();

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
	gl.Enable(GL::Capability::CullFace);
	gl.Enable(GL::Capability::Multisample);

	gl.BlendFunc(GL::BlendingFactor::SourceAlpha, GL::BlendingFactor::OneMinusSourceAlpha);

	context.camera = App::Camera(camera_config);

	config_handler.ParseShaders();
	context.shader_handler = config_handler.GetShaderHandler();

	config_handler.ParseModelConfigs();
	auto model_configs = config_handler.GetModelConfigs();

	for (auto model_config : model_configs) {
        App::Timer loading_timer;
		loading_timer.Start();

		if (model_config->type == "CAR") {
			auto car_model_config = std::dynamic_pointer_cast<App::Config::CarModelConfig>(model_config);
			auto car_model = std::make_shared<App::CarModel>(*car_model_config);
			context.models.push_back(car_model);
		} else if (model_config->type == "SKYBOX") {
			auto skybox_model_config = std::dynamic_pointer_cast<App::Config::SkyboxModelConfig>(model_config);
			auto skybox_model = std::make_shared<App::Skybox>(*skybox_model_config);
			context.models.push_back(skybox_model);
		} else { // model_config->type == "COMMON"
			auto common_model_config = std::dynamic_pointer_cast<App::Config::CommonModelConfig>(model_config);
			auto model = std::make_shared<App::Model>(*common_model_config);
			context.models.push_back(model);
		}

        std::cout << "Model (" << model_config->name << ") loaded successfully in " << loading_timer.Stop<App::Timer::Milliseconds>() << " milliseconds" << std::endl;
	}

	std::dynamic_pointer_cast<App::CarModel>(context.models[0])->intersector_.emplace(intersector_config);
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

		std::dynamic_pointer_cast<App::CarModel>(context.models[0])->intersector_->ClearObstacles();
		// std::dynamic_pointer_cast<App::CarModel>(context.models[0])->intersector_->ClearCarParts();

		std::dynamic_pointer_cast<App::CarModel>(context.models[0])->intersector_->AddObstacles(context.models[2]->CollectMABB());
		std::dynamic_pointer_cast<App::CarModel>(context.models[0])->intersector_->AddObstacles(context.models[3]->CollectMABB());

		// intersector.AddCarParts(context.models[0]->CollectMABB());

		std::dynamic_pointer_cast<App::CarModel>(context.models[0])->Move(delta_time);

		gl.Clear(GL::Buffer::Color | GL::Buffer::Depth);
		gui.Prepare();
		

		for (auto model : context.models) {
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