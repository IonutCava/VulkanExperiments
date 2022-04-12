#version 450

layout(location = 0) in vec3 colour;

layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(colour, 1.f);
}
