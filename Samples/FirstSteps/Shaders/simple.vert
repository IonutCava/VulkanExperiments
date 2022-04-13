#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colour;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColour;

layout(push_constant) uniform Push {
    mat4 transform; //projection * view * model
    mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.f, -3.f, -1.f));
const float AMBIENT = 0.02f;

void main() {
    const vec3 normalWS = normalize(mat3(push.normalMatrix) * normal);
    const float lightIntensity = AMBIENT + max(dot(normalWS, DIRECTION_TO_LIGHT), 0.f);

    fragColour = colour * lightIntensity;

    gl_Position = push.transform * vec4(position, 1.f);
}
