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
    // float bias = 0.005;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    bias = 0.0f;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

   

    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0; 

     if(projCoords.z > 1.0) {
        shadow = 0.0;
    }

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

