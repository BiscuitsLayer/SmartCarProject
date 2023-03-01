#version 330 core

in vec3 aPos;
in vec3 aColor;
in vec2 aTexCoord;

out vec3 vertexColor;
out vec2 textureCoordinates;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(aPos,  1.0);

    vertexColor = aColor;
    textureCoordinates = aTexCoord;
}