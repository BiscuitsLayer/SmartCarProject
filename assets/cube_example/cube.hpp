#pragma once

// STL
#include <vector>

const int APP_WOODEN_TEXTURE_UNIT = 0;
const int APP_AWESOME_TEXTURE_UNIT = 1;

namespace App {

class Cube {
public:
    void Load(GL::Program &program) {
        LoadGeometry(program);
        LoadTextures(program);
    }

    void LoadGeometry(GL::Program &program) {
        vertex_buffer_object = GL::VertexBuffer(vertices.data(), vertices.size() * sizeof(float), GL::BufferUsage::StaticDraw);
        // element_buffer_object = GL::VertexBuffer(indices.data(), indices.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);
        colors_buffer_object = GL::VertexBuffer(colors.data(), colors.size() * sizeof(float), GL::BufferUsage::StaticDraw);
        tex_coords_buffer_object = GL::VertexBuffer(tex_coords.data(), tex_coords.size() * sizeof(float), GL::BufferUsage::StaticDraw);

        vertex_array_object.BindAttribute(program.GetAttribute("aPos"), vertex_buffer_object, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_VEC3_BYTESIZE, 0);
        vertex_array_object.BindAttribute(program.GetAttribute("aColor"), colors_buffer_object, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_VEC3_BYTESIZE, 0);
        vertex_array_object.BindAttribute(program.GetAttribute("aTexCoord"), tex_coords_buffer_object, GL::Type::Float, APP_VEC2_COMPONENTS_COUNT, APP_VEC2_BYTESIZE, 0);
        // vertex_array_object.BindElements(element_buffer_object);
    }

    void LoadTextures(GL::Program &program) {
        std::string buffer;

        buffer = App::ReadFileData("../assets/cube_example/container.jpg", false);
	    auto wooden_image = GL::Image{reinterpret_cast<unsigned char*>(buffer.data()), buffer.size()};
	    wooden_texture = GL::Texture{wooden_image, GL::InternalFormat::RGB};
	    program.SetUniform(program.GetUniform("woodenTexture"), APP_WOODEN_TEXTURE_UNIT);

	    buffer = App::ReadFileData("../assets/cube_example/awesomeface.jpg", false);
	    auto awesome_image = GL::Image{reinterpret_cast<unsigned char*>(buffer.data()), buffer.size()};
	    awesome_texture = GL::Texture{awesome_image, GL::InternalFormat::RGB};
	    program.SetUniform(program.GetUniform("awesomeTexture"), APP_AWESOME_TEXTURE_UNIT);
    }

    void BindTextures(GL::Context &gl) {
        gl.BindTexture(wooden_texture, APP_WOODEN_TEXTURE_UNIT);
		gl.BindTexture(awesome_texture, APP_AWESOME_TEXTURE_UNIT);
    }

    void Draw(GL::Context &gl) {
        //gl.DrawElements(vertex_array_object, GL::Primitive::Triangles, 0, indices.size(), GL::Type::UnsignedInt);
		gl.DrawArrays(vertex_array_object, GL::Primitive::Triangles, 0, vertices.size());
    }

private:
    GL::VertexArray vertex_array_object;

    GL::VertexBuffer vertex_buffer_object;
    GL::VertexBuffer element_buffer_object;
    GL::VertexBuffer colors_buffer_object;
    GL::VertexBuffer tex_coords_buffer_object;

    GL::Texture wooden_texture;
    GL::Texture awesome_texture;

        std::vector<float> vertices {
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
};

} // namespace App