#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;  // 世界坐标系中的片元坐标
    vec3 Normal;  // 世界坐标系中的法线
    vec2 TexCoords;
    vec4 FragPosLightSpace; // 光坐标系中的片元坐标
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix; // 光坐标系变换矩阵

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0); // 光变换矩阵将世界坐标系中的坐标变换为光坐标
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}
