#include <iostream>
#include <fstream>
#include <cmath>

#include <chrono>

#include <GL/OOGL.hpp>

const int APP_INIT_WINDOW_WIDTH = 800;
const int APP_INIT_WINDOW_HEIGHT = 600;
const std::string APP_INIT_WINDOW_TITLE = "DemoApp";
const bool APP_INIT_IS_FULLSCREEN = false;

const int APP_VEC3_COMPONENTS_COUNT = 3;
const int APP_VEC3_BYTESIZE = 3 * sizeof(float);

std::vector<float> vertices {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
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

std::string ReadFileData(std::string filename) {
    std::ifstream source{filename};
    if (!source) {
        throw std::runtime_error("ReadFileData: can't open file: " + filename);
    }

    source.seekg(0, std::ios::end);
    size_t length = source.tellg();
    source.seekg(0, std::ios::beg);

    std::string buffer(length, ' ');
    source.read(buffer.data(), length);

	std::cout << buffer << std::endl;
    return buffer;
}
	 
int main() try {
	GL::Window window(APP_INIT_WINDOW_WIDTH, APP_INIT_WINDOW_HEIGHT, 
		APP_INIT_WINDOW_TITLE, APP_INIT_IS_FULLSCREEN ? GL::WindowStyle::Fullscreen : GL::WindowStyle::Close);
	GL::Context& gl = window.GetContext();

	GL::Shader vert(GL::ShaderType::Vertex, ReadFileData("../shader/triangle.vert"));
	GL::Shader frag(GL::ShaderType::Fragment, ReadFileData("../shader/triangle.frag"));
	GL::Program program(vert, frag);

	GL::VertexBuffer vbo(vertices.data(), vertices.size() * sizeof(float), GL::BufferUsage::StaticDraw);
	GL::VertexBuffer ebo(indices.data(), indices.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);
	GL::VertexBuffer colors_buffer_object(colors.data(), colors.size() * sizeof(float), GL::BufferUsage::StaticDraw);

	GL::VertexArray vao;
	vao.BindAttribute(program.GetAttribute("aPos"), vbo, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_VEC3_BYTESIZE, 0);
	vao.BindAttribute(program.GetAttribute("aColor"), colors_buffer_object, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_VEC3_BYTESIZE, 0);
	vao.BindElements(ebo);

	auto init_time = std::chrono::steady_clock::now();

	GL::Event ev;
	while (window.IsOpen()) {
		while (window.GetEvent(ev));

		gl.ClearColor(GL::Color(std::ceil(255 * 0.2), std::ceil(255 * 0.3), std::ceil(255 * 0.3), std::ceil(255 * 1.0)));
		gl.Clear();

		auto time_duration = (std::chrono::steady_clock::now() - init_time);
		auto time_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_duration).count() / 200;
		auto sin_value = (std::sin(time_duration_ms) / 2.0f) + 0.5f;
		program.SetUniform(program.GetUniform("testShaderUniform"), GL::Vec4(0.0f, sin_value, 0.0f, 1.0f));

		gl.DrawElements(vao, GL::Primitive::Triangles, 0, indices.size(), GL::Type::UnsignedInt);

		window.Present();
	}
	return 0;
} 
catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
	return 0;
}