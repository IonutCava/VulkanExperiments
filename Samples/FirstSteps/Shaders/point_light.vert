#version 450

const vec2 OFFSETS[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragOffset;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    vec4 ambientLightColour;
    vec3 lightPosition;
    vec4 lightColour;
} ubo;

const float LIGHT_RADIUS = 0.1f;

void main() {
    fragOffset = OFFSETS[gl_VertexIndex];
    const vec3 cameraRightWorld = { ubo.viewMatrix[0][0], ubo.viewMatrix[1][0] , ubo.viewMatrix[2][0] };
    const vec3 cameraUpWorld = { ubo.viewMatrix[0][1], ubo.viewMatrix[1][1] , ubo.viewMatrix[2][1] };

    const vec3 positionWorld = ubo.lightPosition.xyz +
                               LIGHT_RADIUS * fragOffset.x * cameraRightWorld +
                               LIGHT_RADIUS * fragOffset.y * cameraUpWorld;

    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * vec4(positionWorld, 1.f);
}