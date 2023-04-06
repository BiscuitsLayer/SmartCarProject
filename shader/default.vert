#version 330 core

// Inputs
in vec3 aPos;
in vec2 aTexCoord;
in vec3 aNormal;
in vec3 aTangent;

// Outputs to fragment shader
out vec3 Pos;
out vec2 TexCoord;
out vec3 Tangent;

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
    gl_Position = MVP * aMatrices.meshTransformToModelMatrix * aMatrices.meshSelfTransformMatrix * vec4(aPos,  1.0);

    // Provide values for fragment shader
    Pos = vec3(aMatrices.modelMatrix * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
    Tangent = aTangent;
}