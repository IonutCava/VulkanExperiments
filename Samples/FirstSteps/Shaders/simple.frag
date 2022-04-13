#version 450

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWS;

layout(location = 0) out vec4 outColour;

struct PointLight {
    vec4 position; //w is unused
    vec4 colour; //w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    vec4 ambientLightColour;
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec3 diffuseLight = ubo.ambientLightColour.rgb * ubo.ambientLightColour.w;

    const vec3 surfaceNormal = normalize(fragNormalWS);
    for (int i = 0; i < ubo.numLights; ++i) {
        PointLight light = ubo.pointLights[i];
        const vec3 directionToLight = light.position.xyz - fragPosWorld.xyz;
        const float attenuation = 1.f / dot(directionToLight, directionToLight); //distance squared
        const float cosAngIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0.f);
        const vec3 intensity = light.colour.xyz * light.colour.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;
    }
    
    outColour = vec4(diffuseLight * fragColour, 1.f);
}
