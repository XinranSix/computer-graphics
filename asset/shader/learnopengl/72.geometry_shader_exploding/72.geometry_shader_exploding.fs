#version 450 core

out vec4 FragColor;

out vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main() {
    FragColor = texture(texture_diffuse1, TexCoords);
}
