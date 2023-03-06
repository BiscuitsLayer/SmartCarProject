#version 330 core

in vec3 vertexColor;
in vec2 textureCoordinates;

out vec4 FragColor;

uniform float shaderMixCoef;
uniform sampler2D textureDiffuse1;
uniform sampler2D textureDiffuse2;

void main() {
    vec4 textureMixed = mix(
        texture(textureDiffuse1, textureCoordinates), 
        texture(textureDiffuse2, vec2(textureCoordinates.x, 1.0 - textureCoordinates.y)), // reverse the image for awesome texture
        shaderMixCoef
    );
    
    if (vertexColor != vec3(0.0f, 0.0f, 0.0f)) {
        FragColor = vec4(vertexColor, 1.0);
    } else {
        FragColor = textureMixed;
    }
}