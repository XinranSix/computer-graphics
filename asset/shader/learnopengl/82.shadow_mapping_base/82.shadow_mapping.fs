#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;  // 世界坐标系中的片元坐标
    vec3 Normal;  // 世界坐标系中的法线
    vec2 TexCoords;
    vec4 FragPosLightSpace; // 光坐标系中的片元坐标
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;

float ShadowCalculaiton(vec4 fragPosLightSpace) {
    // 转到裁剪空间
    // fragPosLightSpace 是直接从片段着色器传递过来的，OpenGL 不会对其做透视除法
    // 在正交投影中，w 的值始终未 1.0 这一步实际上无异于
    // 但在透视投影中这一步相当重要，执行这一步可以保证无论使用什么投影矩阵都没问题
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    // 获取光空间中的深度
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 当前片段的深度
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // 环境光
    vec3 ambient = 0.15 * lightColor;
    // 漫反射
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // 高光项
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(viewDir + lightDir);
    spec = pow(max(dot(halfwayDir, normal), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // 阴影
    float shadow = ShadowCalculaiton(fs_in.FragPosLightSpace);
    // Note: 环境光未计算阴影
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    FragColor = vec4(lighting, 1.0);
}

