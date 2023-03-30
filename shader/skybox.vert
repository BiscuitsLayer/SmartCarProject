#version 330 core

in vec3 aPos;

out vec3 textureDirection;

uniform struct {
    mat4 viewMatrix;
    mat4 projectionMatrix;
} aMatrices;

void main() {
    // Removing translation from view matrix
    mat4 viewNoTranslateMatrix = mat4(mat3(aMatrices.viewMatrix));
    mat4 VP = aMatrices.projectionMatrix * viewNoTranslateMatrix;

    vec4 position = VP * vec4(aPos, 1.0);
    // NDC will then always have a z value equal to 1.0: the maximum depth value
    gl_Position = position.xyww;
    
    textureDirection = aPos;
}  