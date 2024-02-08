#version 450 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Position;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_reflection0;
uniform samplerCube skybox;
uniform vec3 cameraPos;

void main() {
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = texture(texture_diffuse0, TexCoords) +
                2.0*vec4(vec3(texture(texture_reflection0, TexCoords)) * texture(skybox, R).rgb, 1.0);
}
