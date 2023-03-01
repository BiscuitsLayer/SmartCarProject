#pragma once

// STL
#include <string>
#include <vector>

// OpenGL Wrapper
#include <GL/OOGL.hpp>

// Sources
#include <texture/texture.hpp>

namespace App {

class Mesh {
public:
    Mesh() {

    }

private:
    std::vector<GL::Vertex> vertices_;
    std::vector<uint32_t> indices_;
    std::vector<Texture> textures_;

    
};

} // namespace App