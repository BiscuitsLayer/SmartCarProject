#pragma once

// STL
#include <iostream>
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <constants/constants.hpp>
#include <texture/texture.hpp>
#include <bbox/bbox.hpp>

namespace App {

class Mesh {
public:
    Mesh(GL::Program &program, std::string name, Transform transform_to_model, std::vector<GL::Vertex> vertices, std::vector<int> indices, std::vector<Texture> textures, BBox bbox)
    : name_(name), transform_to_model_(transform_to_model), vertices_(vertices), indices_(indices), textures_(textures), bbox_(bbox) {       
        vbo_ = GL::VertexBuffer(vertices.data(), vertices.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);
        ebo_ = GL::VertexBuffer(indices.data(), indices.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);

        vao_.BindAttribute(program.GetAttribute("aPos"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);
        vao_.BindAttribute(program.GetAttribute("aTexCoord"), vbo_, GL::Type::Float, APP_VEC2_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_TEX_OFFSET);
        vao_.BindAttribute(program.GetAttribute("aNormal"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_NORMAL_OFFSET);
        vao_.BindElements(ebo_);
    }

    void Draw(GL::Context &gl, GL::Program &program) {
        gl.UseProgram(program);

        unsigned int diffuseNumber = 1;
        unsigned int specularNumber = 1;

        for (unsigned int i = 0; i < textures_.size(); ++i) {
            std::string texture_name;

            switch (textures_[i].type_) {
                case Texture::Type::DIFFUSE: {
                    texture_name = APP_TEXTURE_DIFFUSE_BASE_NAME + std::to_string(diffuseNumber);
                    ++diffuseNumber;
                    break;
                }
                case Texture::Type::SPECULAR: {
                    texture_name = APP_TEXTURE_SPECULAR_BASE_NAME + std::to_string(specularNumber);
                    ++specularNumber;
                    break;
                }
            }
            
	        program.SetUniform(program.GetUniform(texture_name), textures_[i].texture_unit_);
            gl.BindTexture(textures_[i].texture_, textures_[i].texture_unit_);
            // std::cout << "Bind texture " << texture_name << " to unit " << textures_[i].texture_unit_ << std::endl;
        }

        program.SetUniform(program.GetUniform("meshTransformToModel"), transform_to_model_);
        program.SetUniform(program.GetUniform("meshSelfTransform"), self_transform_);
        gl.DrawElements(vao_, GL::Primitive::Triangles, 0, indices_.size(), GL::Type::UnsignedInt);
    }

    void Draw(GL::Context &gl, GL::Program &program, GL::Program &bbox_program) {
        Draw(gl, program);

        gl.UseProgram(bbox_program);

        bbox_program.SetUniform(bbox_program.GetUniform("meshTransformToModel"), transform_to_model_);
        bbox_program.SetUniform(bbox_program.GetUniform("meshSelfTransform"), self_transform_);
        bbox_.Draw(gl, bbox_program);
    }

private:
    std::string name_;
    Transform transform_to_model_;
    Transform self_transform_;
    BBox bbox_;

    std::vector<GL::Vertex> vertices_;
    std::vector<int> indices_;
    std::vector<Texture> textures_;

    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object
    GL::VertexBuffer ebo_;  // element buffer object

    friend class CarModel;
};

} // namespace App