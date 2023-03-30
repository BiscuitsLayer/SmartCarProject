#version 330 core

// Same as textureCoordinate, but in 3D
in vec3 textureDirection;

out vec4 FragColor;

// Cubemap texture sampler
uniform samplerCube cubemap;

void main() {             
    FragColor = texture(cubemap, textureDirection);
}