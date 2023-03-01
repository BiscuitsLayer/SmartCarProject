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

// Cube model
// #include <cube_example/cube.hpp>
#include <cube_example/cube_ebo.hpp>


// Coordinate space matrices
// model: local space -> world sapce
// view: world space -> camera space
// projection: specified range of space in camera space (seen by camera) -> [-1.0, 1.0] (NDC), 
// clip coordinates out of range
GL::Mat4 projection_matrix = GL::Mat4::Perspective(
	GL::Rad(APP_INIT_FOV_IN_DEGREES), 
	static_cast<float>(APP_INIT_WINDOW_WIDTH) / static_cast<float>(APP_INIT_WINDOW_HEIGHT),
	APP_INIT_NEAR_PLANE, APP_INIT_FAR_PLANE
);
	 
int main() try {
	App::CustomWindow window(APP_INIT_WINDOW_WIDTH, APP_INIT_WINDOW_HEIGHT, 
		APP_INIT_WINDOW_TITLE, APP_INIT_IS_FULLSCREEN ? GL::WindowStyle::Fullscreen : GL::WindowStyle::Close);
	GL::Context& gl = window.GetContext();
	gl.Enable(GL::Capability::DepthTest);

	App::Camera main_camera;
	App::CubeEbo model;
	App::Gui gui;
	gui.Setup();

	GL::Shader vert(GL::ShaderType::Vertex, App::ReadFileData("../shader/triangle.vert", false));
	GL::Shader frag(GL::ShaderType::Fragment, App::ReadFileData("../shader/triangle.frag", false));
	GL::Program program(vert, frag);

	model.Load(program);
	
	App::Timer main_timer;
	main_timer.Start();
	App::Timer texture_timer;
	texture_timer.Start();

	GL::Event ev;
	while (window.IsOpen()) {
		auto delta_time = static_cast<float>(main_timer.Tick<App::Timer::Seconds>());
		while (window.GetEvent(ev)) {
			if (ev.Type == GL::Event::KeyDown) {
				switch (ev.Key.Code) {
					case GL::Key::Escape: {
						window.Close();
						break;
					}
					case GL::Key::W: {
						main_camera.MoveFront(delta_time);
						break;
					}
					case GL::Key::A: {
						main_camera.MoveLeft(delta_time);
						break;
					}
					case GL::Key::S: {
						main_camera.MoveBack(delta_time);
						break;
					}
					case GL::Key::D: {
						main_camera.MoveRight(delta_time);
						break;
					}
				}
			}
		}

		gl.ClearColor(GL::Color(std::ceil(255 * 0.2), std::ceil(255 * 0.3), std::ceil(255 * 0.3), std::ceil(255 * 1.0)));
		gl.Clear(GL::Buffer::Color | GL::Buffer::Depth);

		gui.Prepare();

		float sin_value = (std::sin(texture_timer.Elapsed<App::Timer::Seconds>()) + 1.0f) / 2.0f;
		program.SetUniform(program.GetUniform("shaderMixCoef"), sin_value);

		App::Transform model_matrix;
		model_matrix.UpdateRotation(texture_timer.Elapsed<App::Timer::Seconds>(), GL::Vec3(5.0f, 2.0f, 5.0f));
		
		auto mvp = App::GetModelViewProjectionMatrix(model_matrix, main_camera.GetViewMatrix(), projection_matrix);
		program.SetUniform(program.GetUniform("MVP"), mvp);
		
		model.BindTextures(gl);
		model.Draw(gl);
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