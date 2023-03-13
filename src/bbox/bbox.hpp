#pragma once

// STL
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <constants/constants.hpp>

namespace App {

class BBox {
public:
    BBox(GL::Program &bbox_program, GL::Vec3 min, GL::Vec3 max): min_(min), max_(max) {
        vertices_ = {
            GL::Vertex{GL::Vec3{min_.X, min_.Y, min_.Z}, GL::Vec2{}, GL::Vec3{}},
            GL::Vertex{GL::Vec3{max_.X, min_.Y, min_.Z}, GL::Vec2{}, GL::Vec3{}},  
            GL::Vertex{GL::Vec3{max_.X, max_.Y, min_.Z}, GL::Vec2{}, GL::Vec3{}},   
            GL::Vertex{GL::Vec3{min_.X, max_.Y, min_.Z}, GL::Vec2{}, GL::Vec3{}},   
            GL::Vertex{GL::Vec3{min_.X, min_.Y, max_.Z}, GL::Vec2{}, GL::Vec3{}},
            GL::Vertex{GL::Vec3{max_.X, min_.Y, max_.Z}, GL::Vec2{}, GL::Vec3{}},
            GL::Vertex{GL::Vec3{max_.X, max_.Y, max_.Z}, GL::Vec2{}, GL::Vec3{}},
            GL::Vertex{GL::Vec3{min_.X, max_.Y, max_.Z}, GL::Vec2{}, GL::Vec3{}},
        };

        indices_ = {
	    	0, 1, 2, 2, 3, 0,
	    	1, 5, 6, 6, 2, 1,
	    	7, 6, 5, 5, 4, 7,
	    	4, 0, 3, 3, 7, 4,
	    	4, 5, 1, 1, 0, 4,
	    	3, 2, 6, 6, 7, 3,
        };

        vbo_ = GL::VertexBuffer(vertices_.data(), vertices_.size() * APP_GL_VERTEX_BYTESIZE, GL::BufferUsage::StaticDraw);
        ebo_ = GL::VertexBuffer(indices_.data(), indices_.size() * sizeof(unsigned int), GL::BufferUsage::StaticDraw);

        vao_.BindAttribute(bbox_program.GetAttribute("aPos"), vbo_, GL::Type::Float, APP_VEC3_COMPONENTS_COUNT, APP_GL_VERTEX_BYTESIZE, APP_GL_VERTEX_POS_OFFSET);
        vao_.BindElements(ebo_);
    }

    void Draw(GL::Context &gl, GL::Program &bbox_program) {
        gl.DrawElements(vao_, GL::Primitive::Triangles, 0, indices_.size(), GL::Type::UnsignedInt);
    }

private:
    GL::Vec3 min_;
    GL::Vec3 max_;

    GL::VertexArray  vao_;  // vertex array object
    GL::VertexBuffer vbo_;  // vertex buffer object
    GL::VertexBuffer ebo_;  // element buffer object

    std::vector<GL::Vertex> vertices_;
    std::vector<unsigned int> indices_;
};

} // namespace App