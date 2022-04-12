#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 colour;

layout(location = 0) out vec3 fragColour;

void main() {
    fragColour = colour;
    gl_Position = vec4(position, 0.f, 1.f);
}
