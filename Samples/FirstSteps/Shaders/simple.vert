#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColour;

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
    const vec4 positionWorld = push.modelMatrix * vec4(position, 1.f);
    const vec3 directionToLight = ubo.lightPosition - positionWorld.xyz;
    const float attenuation = 1.f / dot(directionToLight, directionToLight); //distance squared

    const vec3 lightColour = ubo.lightColour.rgb * ubo.lightColour.w * attenuation;
    const vec3 normalWS = normalize(mat3(push.normalMatrix) * normal);
    const vec3 diffuseLight = lightColour * max(dot(normalWS, normalize(directionToLight)), 0.f);

    const vec3 lightIntensity = (ubo.ambientLightColour.rgb * ubo.ambientLightColour.w) + diffuseLight;

    fragColour = colour * lightIntensity;

    gl_Position = ubo.projectionViewMatrix * positionWorld;
}
