#version 450

layout(location = 0) in vec2 fragOffset;

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
    vec4 position;
    vec4 colour;
    float radius;
} push;


void main() {
    const float dist = sqrt(dot(fragOffset, fragOffset));
    if (dist >= 1.f) {
        discard;
    }

    outColour = vec4(push.colour.rgb, 1.f);
}
