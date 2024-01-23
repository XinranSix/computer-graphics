#version 450 core

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

in vec3 FragPos;
in vec3 Normal;

void main() {
    // float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor; // 使用 lightColor 乘上环境光强度因子得到环境光

    vec3 norm = normalize(Normal); // 法线
    vec3 lightDir = normalize(lightPos - FragPos); // 入射光反方向
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

   // float specularStrength = 0.5; // 镜面反射系数
    vec3 viewDir = normalize(viewPos - FragPos); // 相机方向
    vec3 reflectDir = reflect(-lightDir, norm); // 方式方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diff + specular) * objectColor;
    FragColor = vec4(result, 1.0f);
}
/*
ambientStrength: float // 环境光强度
specularStrength : float//镜面反射系数
Shininess: float // 反光度
*/
