#version 330 core

// Inputs from vertex shader
in vec3 Pos;
in vec2 TexCoord;
in vec3 Tangent;

// Outputs
out vec4 FragColor;

// Base color
uniform bool baseColorHasTexture;
uniform sampler2D baseColorTexture;
uniform vec4 baseColorFactor;

// Metallic roughness
uniform bool metallicRoughnessHasTexture;
uniform sampler2D metallicRoughnessTexture;
uniform vec4 metallicRoughnessFactor;

// Normal map
uniform bool normalHasTexture;
uniform sampler2D normalTexture;

// Camera position
uniform vec3 cameraPosition;

void main() {
    // Get base color from texture and factor
    vec4 baseColor = baseColorFactor;
    if (baseColorHasTexture) {
        baseColor *= texture(baseColorTexture, TexCoord);
    }

    // Get metallic and roughness values
    float metallic = metallicRoughnessFactor.r;
    float roughness = metallicRoughnessFactor.g;
    if (metallicRoughnessHasTexture) {
        metallic *= texture(metallicRoughnessTexture, TexCoord).r;
        roughness *= texture(metallicRoughnessTexture, TexCoord).g;
    }

    // Get world-space normals from TBN matrix
    vec3 N = texture(normalTexture, TexCoord).rgb;
    N = normalize(N * 2.0 - 1.0);
    N = normalize(Tangent * N); 
    vec3 V = normalize(cameraPosition - Pos);
    vec3 R = reflect(-1.0 * V, N); 

    FragColor = vec4(R, 1.0);
    return;
}