#version 330 core

// Inputs from vertex shader
in vec3 Pos;
in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

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

    vec3 N = normalize(Normal);
    if (normalHasTexture) {
        vec3 mappedNormal = texture(normalTexture, TexCoord).rgb * 2.0 - 1.0;
        N = normalize(mat3(Tangent, Bitangent, Normal) * mappedNormal);
    }
    vec3 V = normalize(cameraPosition - Pos);
    vec3 L = normalize(vec3(-0.35, 1.0, 0.25));
    vec3 H = normalize(L + V);

    vec3 albedo = pow(max(baseColor.rgb, vec3(0.0)), vec3(2.2));
    float diffuse = max(dot(N, L), 0.0);
    float hemisphere = N.y * 0.5 + 0.5;
    float gloss = mix(24.0, 6.0, clamp(roughness, 0.0, 1.0));
    float specular = pow(max(dot(N, H), 0.0), gloss) * mix(0.08, 0.3, clamp(metallic, 0.0, 1.0));

    vec3 lighting = vec3(0.24, 0.27, 0.34)
        + hemisphere * vec3(0.18, 0.20, 0.22)
        + diffuse * vec3(1.05, 0.96, 0.84);
    float rim = pow(1.0 - max(dot(N, V), 0.0), 3.0);
    vec3 color = albedo * lighting
        + specular * vec3(1.0, 0.92, 0.78)
        + rim * albedo * vec3(0.12, 0.16, 0.24);
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, baseColor.a);
    return;
}
