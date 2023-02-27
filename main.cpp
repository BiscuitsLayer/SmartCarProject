// STL
#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// ImGUI
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

// Sources
#include <transform/transform.hpp>
#include <camera/camera.hpp>
#include <timer/timer.hpp>

const int APP_INIT_WINDOW_WIDTH = 800;
const int APP_INIT_WINDOW_HEIGHT = 600;
const std::string APP_INIT_WINDOW_TITLE = "DemoApp";
const bool APP_INIT_IS_FULLSCREEN = true;

const float APP_INIT_FOV_IN_DEGREES = 45.0f;
const float APP_INIT_NEAR_PLANE = 0.1f;
const float APP_INIT_FAR_PLANE = 100.0f;

const float APP_INIT_MAX_FPS = 800.0f;
const bool APP_MAX_FPS_MODE_ON = false;

const int APP_VEC2_COMPONENTS_COUNT = 2;
const int APP_VEC2_BYTESIZE = 2 * sizeof(float);

const int APP_VEC3_COMPONENTS_COUNT = 3;
const int APP_VEC3_BYTESIZE = 3 * sizeof(float);

const int APP_WOODEN_TEXTURE_UNIT = 0;
const int APP_AWESOME_TEXTURE_UNIT = 1;

std::vector<float> vertices {
    //  0.5f,  0.5f, 0.0f,  // top right
    //  0.5f, -0.5f, 0.0f,  // bottom right
    // -0.5f, -0.5f, 0.0f,  // bottom left
    // -0.5f,  0.5f, 0.0f   // top left

	-0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  

    -0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  

    -0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  

     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  

    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f, -0.5f,  

    -0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f,  
};

std::vector<unsigned int> indices {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

std::vector<float> colors {
	1.0f, 0.0f, 0.0f,	// red
	0.0f, 1.0f, 0.0f,	// green
	0.0f, 0.0f, 1.0f,	// blue
	1.0f, 1.0f, 0.0f	// white
};

// texture coordinate axis names:
// s <-> x
// t <-> y
// r <-> z (only for 3D textures)
std::vector<float> tex_coords = {
    // 1.0f, 1.0f,  
    // 1.0f, 0.0f,
    // 0.0f, 0.0f,
	// 0.0f, 1.0f

	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f
};

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

std::string ReadFileData(std::string filename, bool debug_dump = true) {
    std::ifstream source{filename};
    if (!source) {
        throw std::runtime_error("ReadFileData: can't open file: " + filename);
    }

    source.seekg(0, std::ios::end);
    size_t length = source.tellg();
    source.seekg(0, std::ios::beg);

    std::string buffer(length, 0);
    source.read(buffer.data(), length);

	if (debug_dump) {
		std::cout << buffer << std::endl;
	}
    return buffer;
}
	 
int main() try {
	GL::Window window(APP_INIT_WINDOW_WIDTH, APP_INIT_WINDOW_HEIGHT, 
		APP_INIT_WINDOW_TITLE, APP_INIT_IS_FULLSCREEN ? GL::WindowStyle::Fullscreen : GL::WindowStyle::Close);
	GL::Context& gl = window.GetContext();
	App::Camera main_camera;

	gl.Enable(GL::Capability::DepthTest);

// imgui setup part
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    const char* glsl_version = "#version 140";
    ImGui_ImplWin32_Init(window.GetRaw());
    ImGui_ImplOpenGL3_Init(glsl_version);

	ImVec4 clear_color = ImVec4(1.000F, 1.000F, 1.000F, 1.0F);
    ImGui::StyleColorsClassic();
// imgui setup part

	GL::Shader vert(GL::ShaderType::Vertex, ReadFileData("../shader/triangle.vert"));
	GL::Shader frag(GL::ShaderType::Fragment, ReadFileData("../shader/triangle.frag"));
	GL::Program program(vert, frag);

	GL::VertexBuffer vbo(vertices.data(), vertices.size() * sizeof(float), GL::BufferUsage::StaticDraw);
	//GL::VertexBuffer ebo(indices.data(), indices.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);
	GL::VertexBuffer colors_buffer_object(colors.data(), colors.size() * sizeof(float), GL::BufferUsage::StaticDraw);
	GL::VertexBuffer tex_coords_buffer_object(
		tex_coords.data(), tex_coords.size() * sizeof(float), GL::BufferUsage::StaticDraw
	);

	GL::VertexArray vao;
	vao.BindAttribute(program.GetAttribute("aPos"), vbo, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_VEC3_BYTESIZE, 0);
	vao.BindAttribute(program.GetAttribute("aColor"), colors_buffer_object, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_VEC3_BYTESIZE, 0);
	vao.BindAttribute(program.GetAttribute("aTexCoord"), tex_coords_buffer_object, GL::Type::Float, APP_VEC2_COMPONENTS_COUNT, APP_VEC2_BYTESIZE, 0);
	//vao.BindElements(ebo);

	int frame_counter = 0;
	int last_frame_count = frame_counter;

	float fps = 0;
	
	App::Timer main_timer;
	main_timer.Start();
	App::Timer texture_timer;
	texture_timer.Start();

	std::string buffer = ReadFileData("../assets/container.jpg", false);
	GL::Image wooden_image(reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
	GL::Texture wooden_texture(wooden_image, GL::InternalFormat::RGB);
	program.SetUniform(program.GetUniform("woodenTexture"), APP_WOODEN_TEXTURE_UNIT);

	buffer = ReadFileData("../assets/awesomeface.jpg", false);
	GL::Image awesome_image(reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
	GL::Texture awesome_texture(awesome_image, GL::InternalFormat::RGB);
	program.SetUniform(program.GetUniform("awesomeTexture"), APP_AWESOME_TEXTURE_UNIT);

	GL::Event ev;
	while (window.IsOpen()) {
		auto delta_time = static_cast<float>(main_timer.Tick<App::Timer::Seconds>());
		// std::cout << "dt = " << delta_time << std::endl;

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

// imgui prepare part
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
// imgui prepare part

		gl.ClearColor(GL::Color(std::ceil(255 * 0.2), std::ceil(255 * 0.3), std::ceil(255 * 0.3), std::ceil(255 * 1.0)));
		gl.Clear(GL::Buffer::Color | GL::Buffer::Depth);

		float sin_value = (std::sin(texture_timer.Elapsed<App::Timer::Seconds>()) + 1.0f) / 2.0f;
		program.SetUniform(program.GetUniform("shaderMixCoef"), sin_value);

		App::Transform model_matrix;
		model_matrix.UpdateRotation(texture_timer.Elapsed<App::Timer::Seconds>(), GL::Vec3(5.0f, 2.0f, 5.0f));
		
		auto mvp = App::GetModelViewProjectionMatrix(model_matrix, main_camera.GetViewMatrix(), projection_matrix);
		program.SetUniform(program.GetUniform("MVP"), mvp);
		
		gl.BindTexture(wooden_texture, APP_WOODEN_TEXTURE_UNIT);
		gl.BindTexture(awesome_texture, APP_AWESOME_TEXTURE_UNIT);

		//gl.DrawElements(vao, GL::Primitive::Triangles, 0, indices.size(), GL::Type::UnsignedInt);
		gl.DrawArrays(vao, GL::Primitive::Triangles, 0, vertices.size());

// imgui draw part
		ImGui::ShowDemoWindow();
		ImGui::EndFrame();
        ImGui::Render();  
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
// imgui draw part

		window.Present();
		
		if (APP_MAX_FPS_MODE_ON) {
			static std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
			static std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

			const float max_elapsed_time = 1000.0f / APP_INIT_MAX_FPS;

			a = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> work_time = a - b;
			if (work_time.count() < max_elapsed_time) {
				std::chrono::duration<double, std::milli> delta_ms(max_elapsed_time - work_time.count());
				auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
				// std::cout << "sleep for = " << delta_ms_duration.count() << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
			}
			b = std::chrono::system_clock::now();
		}

		frame_counter++;
		auto elapsed_time = static_cast<float>(main_timer.Elapsed<App::Timer::Seconds>());
		if (elapsed_time > 0.5f) { // update FPS every 0.5 seconds
			fps = (frame_counter - last_frame_count) / elapsed_time;

			std::cout << "fps = " <<  fps << std::endl;

			last_frame_count = frame_counter;
			main_timer.Lap();
		}
	}

// imgui cleanup part
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
// imgui cleanup part

	return 0;
} 
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
	return 0;
}