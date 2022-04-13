#version 450

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWS;

layout(location = 0) out vec4 outColour;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 ambientLightColour;
    vec3 lightPosition;
    vec4 lightColour;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {

    const vec3 directionToLight = ubo.lightPosition - fragPosWorld.xyz;
    const float attenuation = 1.f / dot(directionToLight, directionToLight); //distance squared
    const vec3 lightColour = ubo.lightColour.rgb * ubo.lightColour.w * attenuation;
    const vec3 ambientLight = ubo.ambientLightColour.rgb * ubo.ambientLightColour.w;

    const vec3 diffuseLight = lightColour * max(dot(normalize(fragNormalWS), normalize(directionToLight)), 0.f);
    
    outColour = vec4((diffuseLight + ambientLight) * fragColour, 1.f);
}
