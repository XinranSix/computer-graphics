#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;


out vec3 result;

out vec3 FragPos;
out vec3 Normal;

void main () {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0f)); // 世界空间的顶点坐标
    // 法线变换要使用法线矩阵，即逆转矩阵
    Normal = mat3(transpose(inverse(model))) * aNormal;

    vec3 ambient = ambientStrength * lightColor; // 使用 lightColor 乘上环境光强度因子得到环境光

    vec3 norm = normalize(Normal); // 法线
    vec3 lightDir = normalize(lightPos - FragPos); // 入射光反方向
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos); // 相机方向
    vec3 reflectDir = reflect(-lightDir, norm); // 方式方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    result = (ambient + diff + specular) * objectColor;
}
