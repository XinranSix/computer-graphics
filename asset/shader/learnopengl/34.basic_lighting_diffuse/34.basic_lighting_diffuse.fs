#version 450 core

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

in vec3 FragPos;
in vec3 Normal;

void main() {
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor; // 使用 lightColor 乘上环境光强度因子得到环境光

    vec3 norm = normalize(Normal); // 法线
    vec3 lightDir = normalize(lightPos - FragPos); // 入射光反方向
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diff) * objectColor;
    FragColor = vec4(result, 1.0f);
}
