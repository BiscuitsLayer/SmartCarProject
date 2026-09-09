#version 330 core

// Inputs
in vec3 aPos;
in vec2 aTexCoord;
in vec3 aNormal;
in vec3 aTangent;

// Outputs to fragment shader
out vec3 Pos;
out vec2 TexCoord;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;

// Transform matrices
uniform struct {
    mat4 meshSelfTransformMatrix;
    mat4 meshTransformToModelMatrix;
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} aMatrices;

void main() {
    mat4 worldMatrix = aMatrices.modelMatrix * aMatrices.meshTransformToModelMatrix * aMatrices.meshSelfTransformMatrix;
    vec4 worldPosition = worldMatrix * vec4(aPos, 1.0);
    gl_Position = aMatrices.projectionMatrix * aMatrices.viewMatrix * worldPosition;

    // Provide values for fragment shader
    Pos = worldPosition.xyz;
    TexCoord = aTexCoord;
    mat3 normalMatrix = transpose(inverse(mat3(worldMatrix)));
    Normal = normalize(normalMatrix * aNormal);
    Tangent = normalize(normalMatrix * aTangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    Bitangent = normalize(cross(Normal, Tangent));
}
