#version 450 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
    sampler2D emission;
};

struct Light {
    vec3 position;
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

uniform float matrixlight;
uniform float matrixmove;



void main() {
    // 环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // 自发光
    // vec3 emission = vec3(texture(material.emission, TexCoords));
    // vec3 emission = matrixlight * texture(material.emission, vec2(TexCoords.x, TexCoords.y + matrixmove)).rgb;
    vec3 emission;
    float x = TexCoords.x;
    float y = TexCoords.y;

    if (x < 0.2 || x > 0.8 || y < 0.2 || y > 0.8) {
        // emission = matrixlight * texture(material.emission, vec2(TexCoords.x, TexCoords.y + matrixmove)).rgb;
        emission = (sin(matrixmove) / 2.0 + 0.5) * texture(material.emission, TexCoords + vec2(0.0, matrixmove)).rgb;
    } else {
        emission = vec3(0.0);
    }

    vec3 result = ambient + diffuse + specular + emission;
    FragColor =  vec4(result, 1.0f);
}


