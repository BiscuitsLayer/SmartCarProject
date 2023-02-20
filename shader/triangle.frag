#version 330 core

in vec3 vertexColor;

out vec4 FragColor;

uniform vec4 testShaderUniform;

void main() {
    // FragColor = testShaderUniform;
    FragColor = vec4(vertexColor, 1.0);
}