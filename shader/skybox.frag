#version 330 core

in vec3 textureDirection; // same as textureCoordinate, but in 3D

out vec4 FragColor;

uniform samplerCube cubemap; // cubemap texture sampler

void main() {             
    FragColor = texture(cubemap, textureDirection);
}