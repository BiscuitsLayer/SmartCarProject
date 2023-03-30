#version 440 core

in FragData {
    vec2 TexCoords;
    vec3 FragPos;
    mat3 TBN;
    vec4 FragPosLightSpace;
} fragData;

// IBL (TODO: implement from learnopengl)
uniform samplerCube irradianceTexture;
uniform samplerCube prefilterTexture;
uniform sampler2D brdfLUT;

// PBR material parameters
uniform sampler2D baseColorTexture;
uniform sampler2D normalTexture;
uniform sampler2D metallicTexture;
uniform sampler2D roughnessTexture;
//uniform sampler2D aoTexture;

uniform float bloomThreshold;

// lights
uniform vec3 directionalLight;
uniform vec3 lightColor;

uniform vec3 camPos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#include "pbr.inc"

void main() {       
    // material properties
    const vec3 baseColor = pow(texture(baseColorTexture, fragData.TexCoords).rgb, vec3(2.2));
    const float metallic = texture(metallicTexture, fragData.TexCoords).r;
    const float roughness = texture(roughnessTexture, fragData.TexCoords).r;
       
    // input lighting data
    // Get world-space normals from TBN matrix
    vec3 N = texture(normalTexture, fragData.TexCoords).rgb;
    N = normalize(N * 2.0 - 1.0);
    N = normalize(fragData.TBN * N); 
    const vec3 V = normalize(camPos - fragData.FragPos);
    const vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if di-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the baseColor color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, baseColor, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
{
    // calculate per-light radiance
    const vec3 L = normalize(directionalLight);
    const vec3 H = normalize(V + L);
    const vec3 radiance = lightColor;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;     

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);        

    const float shadow = ComputeShadow(fragData.FragPosLightSpace);
    // add to outgoing radiance Lo
    Lo = (kD * baseColor / PI + specular) * radiance * NdotL * shadow;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}
    
    // ambient lighting (we now use IBL as the ambient term)
    const vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;     
    
    vec3 irradiance = texture(irradianceTexture, N).rgb;
    vec3 diffuse    = irradiance * baseColor;
    
    // sample both the pre-filter Texture and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterTexture, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    const vec3 ambient = kD * diffuse + specular; // * ao;
    
    vec3 color = ambient * 0.5 + Lo;

    // Apply bloom threshold
    const float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > bloomThreshold) {
        BrightColor = vec4(color, 1.0);
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    FragColor = vec4(color, 1.0);
}