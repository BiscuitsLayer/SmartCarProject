#version 330 core

in vec2 textureCoordinates;

out vec4 FragColor;

uniform bool baseColorHasTexture;
uniform sampler2D baseColorTexture;
uniform vec4 baseColorFactor;

void main() {
    vec4 baseColor = baseColorFactor;
    if (baseColorHasTexture) {
        baseColor *= texture(baseColorTexture, textureCoordinates);
    }

    FragColor = baseColor;
}