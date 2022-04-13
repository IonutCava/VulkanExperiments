#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColour;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec3 directionToLight;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

const float AMBIENT = 0.02f;

void main() {
    const vec3 normalWS = normalize(mat3(push.normalMatrix) * normal);
    const float lightIntensity = AMBIENT + max(dot(normalWS, ubo.directionToLight), 0.f);

    fragColour = colour * lightIntensity;

    gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.f);
}
