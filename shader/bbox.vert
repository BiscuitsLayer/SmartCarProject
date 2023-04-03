#version 330 core

in vec3 aWireframePos;
in vec3 aSolidPos;

uniform bool isWireframe;

// Transform matrices
uniform struct {
    mat4 meshSelfTransformMatrix;
    mat4 meshTransformToModelMatrix;
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} aMatrices;

void main() {
    mat4 MVP = aMatrices.projectionMatrix * aMatrices.viewMatrix * aMatrices.modelMatrix;

    vec3 aPos = isWireframe ? aWireframePos : aSolidPos;
    gl_Position = MVP * aMatrices.meshTransformToModelMatrix * vec4(aPos,  1.0);
}