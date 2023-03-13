#version 330 core

in vec3 aPos;

uniform mat4 meshTransformToModel;
uniform mat4 meshSelfTransform;
uniform mat4 MVP;

void main() {
    gl_Position = MVP * meshTransformToModel * meshSelfTransform * vec4(aPos,  1.0);
}