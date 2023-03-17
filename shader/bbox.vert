#version 330 core

in vec3 aPos;

uniform mat4 meshTransformToModel;
uniform mat4 MVP;

void main() {
    gl_Position = MVP * meshTransformToModel * vec4(aPos,  1.0);
}