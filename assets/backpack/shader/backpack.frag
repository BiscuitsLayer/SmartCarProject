#version 330 core

in vec2 textureCoordinates;

out vec4 FragColor;

uniform sampler2D textureDiffuse1;

void main() {
    FragColor = texture(textureDiffuse1, textureCoordinates);
}