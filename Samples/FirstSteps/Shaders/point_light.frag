#version 450

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outColour;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    vec4 ambientLightColour;
    vec3 lightPosition;
    vec4 lightColour;
} ubo;

void main() {
    const float dist = sqrt(dot(fragOffset, fragOffset));
    if (dist >= 1.f) {
        discard;
    }

    outColour = vec4(ubo.lightColour.rgb, 1.f);
}
