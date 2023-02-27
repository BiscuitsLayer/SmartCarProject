#version 330 core

// in vec3 vertexColor;
in vec2 textureCoordinates;

out vec4 FragColor;

uniform float shaderMixCoef;
uniform sampler2D woodenTexture;
uniform sampler2D awesomeTexture;

void main() {
    vec4 textureMixed = mix(
        texture(woodenTexture, textureCoordinates), 
        texture(awesomeTexture, vec2(textureCoordinates.x, 1.0 - textureCoordinates.y)), // reverse the image for awesome texture
        shaderMixCoef
    );
    // FragColor = vec4(vertexColor, 1.0) * textureMixed;
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    FragColor = textureMixed;
    }