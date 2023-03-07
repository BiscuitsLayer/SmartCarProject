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
#include <skybox/skybox.hpp>

// Model
#include <cube_example/src/cube.hpp>
#include <model/model.hpp>
#include <car/car.hpp>

// Coordinate space matrices
// model: local space -> world space
// view: world space -> camera space
// projection: specified range of space in camera space (seen by camera) -> [-1.0, 1.0] (NDC), 
// clip coordinates out of range
GL::Mat4 projection_matrix = GL::Mat4::Perspective(
	GL::Rad(APP_INIT_FOV_IN_DEGREES), 
	static_cast<float>(APP_INIT_WINDOW_WIDTH) / static_cast<float>(APP_INIT_WINDOW_HEIGHT),
	APP_INIT_NEAR_PLANE, APP_INIT_FAR_PLANE
);
	 
int main() try {
	std::array<bool, APP_KEYBOARD_KEYS_COUNT> keyboard_status;
	keyboard_status.fill(false);

	App::CustomWindow window(APP_INIT_WINDOW_WIDTH, APP_INIT_WINDOW_HEIGHT, 
		APP_INIT_WINDOW_TITLE, APP_INIT_IS_FULLSCREEN ? GL::WindowStyle::Fullscreen : GL::WindowStyle::Close);
	GL::Context& gl = window.GetContext();
	gl.Enable(GL::Capability::DepthTest);

	auto keyboard_mode = std::make_shared<App::KeyboardMode>(App::KeyboardMode::CAR_MOVEMENT);
	// orbit camera
	auto main_camera = std::make_shared<App::Camera>();
	// todo: add first person camera

	GL::Shader vert(GL::ShaderType::Vertex, App::ReadFileData("../assets/backpack/shader/backpack.vert", false));
	GL::Shader frag(GL::ShaderType::Fragment, App::ReadFileData("../assets/backpack/shader/backpack.frag", false));
	GL::Program program(vert, frag);

	GL::Shader skybox_vert(GL::ShaderType::Vertex, App::ReadFileData("../assets/skybox/shader/skybox.vert", false));
	GL::Shader skybox_frag(GL::ShaderType::Fragment, App::ReadFileData("../assets/skybox/shader/skybox.frag", false));
	GL::Program skybox_program(skybox_vert, skybox_frag);

	std::array<std::string, 6> filenames {
        "posx.jpg",
        "negx.jpg",
        "posy.jpg",
        "negy.jpg",
        "posz.jpg",
        "negz.jpg",
    };
	App::Skybox skybox(skybox_program, "../assets/skybox/textures", filenames);

	// std::vector<std::string> car_wheels_nodes = {
	// 	"Aro externo.001_0",
	// 	"Aro externo.006_1",
	// };

	std::vector<std::string> car_wheels_meshes = {
		"Object_0",
		"Object_1",
		"Object_2",
		"Object_3",
	};
	// App::Cube model{program};
	App::CarModel car_model{program, "../assets/car/scene.gltf", car_wheels_meshes};

	// App::Model backpack{program, "../assets/backpack/scene.gltf"};
	// backpack.UpdateScale(GL::Vec3(0.01f, 0.01f, 0.01f));
	// backpack.SetTranslation(GL::Vec3(5.0f, 5.0f, 5.0f));

	App::Model garage{program, "../assets/garage/scene.gltf"};
	garage.UpdateTranslation(GL::Vec3(0.0f, -0.5f, 0.0f));

	App::Gui gui(main_camera, keyboard_mode);

	App::Timer main_timer;
	main_timer.Start();
	App::Timer texture_timer;
	texture_timer.Start();

	GL::Event ev;
	while (window.IsOpen()) {
		auto delta_time = static_cast<float>(main_timer.Tick<App::Timer::Seconds>());


		car_model.RotateWheels(delta_time);


		while (window.GetEvent(ev)) {
			if (ev.Type == GL::Event::KeyDown) {
				keyboard_status[ev.Key.Code] = true;
			} else if (ev.Type == GL::Event::KeyUp) {
				keyboard_status[ev.Key.Code] = false;
			}
		}

		if (keyboard_status[GL::Key::W]) {
			switch (*keyboard_mode) {
				case App::KeyboardMode::ORBIT_CAMERA: {
					main_camera->MoveFront(delta_time);
					break;
				}
				case App::KeyboardMode::CAR_MOVEMENT: {
					car_model.MoveFront(delta_time);
					break;
				}
			}
		}

		if (keyboard_status[GL::Key::S]) {
			switch (*keyboard_mode) {
				case App::KeyboardMode::ORBIT_CAMERA: {
					main_camera->MoveBack(delta_time);
					break;
				}
				case App::KeyboardMode::CAR_MOVEMENT: {
					car_model.MoveBack(delta_time);
					break;
				}
			}
		}

		if (keyboard_status[GL::Key::A]) {
			switch (*keyboard_mode) {
				case App::KeyboardMode::ORBIT_CAMERA: {
					main_camera->MoveLeft(delta_time);
					break;
				}
				case App::KeyboardMode::CAR_MOVEMENT: {
					if (keyboard_status[GL::Key::W]) {
						car_model.MoveFrontLeft(delta_time);
					} else if (keyboard_status[GL::Key::S]) {
						car_model.MoveBackLeft(delta_time);
					}
					break;
				}
			}
		}

		if (keyboard_status[GL::Key::D]) {
			switch (*keyboard_mode) {
				case App::KeyboardMode::ORBIT_CAMERA: {
					main_camera->MoveRight(delta_time);
					break;
				}
				case App::KeyboardMode::CAR_MOVEMENT: {
					if (keyboard_status[GL::Key::W]) {
						car_model.MoveFrontRight(delta_time);
					} else if (keyboard_status[GL::Key::S]) {
						car_model.MoveBackRight(delta_time);
					}
					break;
				}
			}
		}

		gl.UseProgram(program);

		gl.ClearColor(GL::Color(std::ceil(255 * 0.2), std::ceil(255 * 0.3), std::ceil(255 * 0.3), std::ceil(255 * 1.0)));
		gl.Clear(GL::Buffer::Color | GL::Buffer::Depth);

		gui.Prepare();

		float sin_value = (std::sin(texture_timer.Elapsed<App::Timer::Seconds>()) + 1.0f) / 2.0f;
		program.SetUniform(program.GetUniform("shaderMixCoef"), sin_value);

		GL::Mat4 mvp;
		GL::Mat4 vp;

		// backpack.SetRotation(texture_timer.Elapsed<App::Timer::Seconds>(), GL::Vec3(5.0f, 2.0f, 5.0f));
		// mvp = App::GetModelViewProjectionMatrix(backpack.GetModelMatrix(), main_camera->GetViewMatrix(), projection_matrix);
		// program.SetUniform(program.GetUniform("MVP"), mvp);
		// backpack.Draw(gl, program);

		mvp = App::GetModelViewProjectionMatrix(garage.GetModelMatrix(), main_camera->GetViewMatrix(), projection_matrix);
		program.SetUniform(program.GetUniform("MVP"), mvp);
		garage.Draw(gl, program);
		
		mvp = App::GetModelViewProjectionMatrix(car_model.GetModelMatrix(), main_camera->GetViewMatrix(), projection_matrix);
		program.SetUniform(program.GetUniform("MVP"), mvp);
		car_model.Draw(gl, program);

		gl.UseProgram(skybox_program);

		vp = App::GetViewNoTranslationProjectionMatrix(main_camera->GetViewMatrix(), projection_matrix);
		skybox_program.SetUniform(skybox_program.GetUniform("VP"), vp);
		skybox.Draw(gl, skybox_program);

		gui.Draw();
		window.Present();
		
		App::LimitMaxFps();
	}

	gui.Cleanup();
	return 0;
} 
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
	return 0;
}