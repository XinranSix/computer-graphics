#version 450 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {

    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    vec4 color;

    if ( theta > light.cutOff) {
    // 环境光
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

        // 漫反射
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

        // 镜面反射
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

        float distance = length(light.position - FragPos);
        vec3 result = ambient + diffuse + specular;
        color = vec4(result, 1.0f);
    } else {
        color = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0f);
    }

    FragColor = color;
}

