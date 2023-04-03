#version 330 core

out vec4 FragColor;

uniform bool isWireframe;

void main() {
    if (isWireframe) {
        FragColor = vec4(1.0, 1.0, 1.0, 0.1);
    } else {
        FragColor = vec4(1.0, 0.0, 0.0, 0.5);
    }
}