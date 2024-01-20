#version 450 core

layout (location = 0) in vec3 aPos;

uniform vec2 offset;

out vec3 ourColor;

void main() {
    gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);
    ourColor = gl_Position.xyz;
}