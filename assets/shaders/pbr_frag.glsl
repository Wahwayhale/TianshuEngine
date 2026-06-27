#version 450

// =============================================
// 常量定义
// =============================================
const float PI = 3.14159265359;
const int MAX_DIRECTIONAL_LIGHTS = 4;
const int MAX_POINT_LIGHTS = 16;
const int MAX_SPOT_LIGHTS = 8;

// =============================================
// 光源结构体
// =============================================
struct DirectionalLight {
    vec4 directionAndIntensity;  // xyz = direction, w = intensity
    vec4 color;                  // rgb = color
};

struct PointLight {
    vec4 positionAndRange;       // xyz = position, w = range
    vec4 colorAndIntensity;      // rgb = color, w = intensity
};

struct SpotLight {
    vec4 positionAndRange;       // xyz = position, w = range
    vec4 directionAndIntensity;  // xyz = direction, w = intensity
    vec4 colorAndCutoff;         // rgb = color, w = innerCutoff
    vec4 outerCutoff;            // x = outerCutoff
};

// =============================================
// UBO 定义
// =============================================

// Scene UBO
layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 projection;
    vec3 cameraPos;
    float padding0;
    vec3 ambientColor;
    float padding1;
} scene;

// 光源 UBO
layout(set = 0, binding = 1) uniform LightUBO {
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    int directionalLightCount;
    int pointLightCount;
    int spotLightCount;
    float padding;
} lights;

// Material UBO
layout(set = 0, binding = 2) uniform MaterialUBO {
    vec4 albedo;
    float metallic;
    float roughness;
    float ao;
    float emission;
} material;

// 阴影 UBO
layout(set = 0, binding = 3) uniform ShadowUBO {
    mat4 lightSpaceMatrix;
    float shadowBias;
    float shadowStrength;
    vec2 padding;
} shadow;

// Albedo texture
layout(set = 0, binding = 4) uniform sampler2D albedoTexture;

// Shadow map texture
layout(set = 0, binding = 5) uniform sampler2D shadowMap;

// =============================================
// 输入/输出
// =============================================
layout(location = 0) in vec3 fragWorldPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

// =============================================
// PBR 函数
// =============================================

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// =============================================
// 阴影计算
// =============================================

float calculateShadow(vec3 worldPos, vec3 normal, vec3 lightDir) {
    // 变换到光源空间
    vec4 lightSpacePos = shadow.lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;

    // 变换到 [0,1] 范围
    projCoords = projCoords * 0.5 + 0.5;

    // 超出阴影贴图范围，不在阴影中
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0) {
        return 1.0;
    }

    // 获取当前片段在光源空间的深度
    float currentDepth = projCoords.z;

    // 自适应偏移（根据法线和光线方向的角度调整）
    float bias = max(shadow.shadowBias * (1.0 - dot(normal, lightDir)), shadow.shadowBias * 0.1);

    // PCF (Percentage-Closer Filtering) 柔化阴影边缘
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0;

    // 应用阴影强度
    shadow = mix(1.0, shadow, shadow.shadowStrength);

    return shadow;
}

// =============================================
// BRDF 计算
// =============================================

vec3 calculateBRDF(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness, vec3 F0) {
    vec3 H = normalize(V + L);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 diffuse = kD * albedo / PI;

    float NdotL = max(dot(N, L), 0.0);
    return (diffuse + specular) * radiance * NdotL;
}

// =============================================
// 主函数
// =============================================
void main() {
    // 采样纹理
    vec4 texColor = texture(albedoTexture, fragTexCoord);
    vec3 albedo = material.albedo.rgb * texColor.rgb;
    float alpha = material.albedo.a * texColor.a;

    // 材质属性
    float metallic = material.metallic;
    float roughness = max(material.roughness, 0.04);
    float ao = material.ao;

    // 法线、视线方向
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(scene.cameraPos - fragWorldPos);

    // F0
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // 累积光照
    vec3 Lo = vec3(0.0);

    // ========== 方向光（带阴影）==========
    for (int i = 0; i < lights.directionalLightCount; i++) {
        vec3 lightDir = -normalize(lights.directionalLights[i].directionAndIntensity.xyz);
        float intensity = lights.directionalLights[i].directionAndIntensity.w;
        vec3 lightColor = lights.directionalLights[i].color.rgb;

        // 只对第一个方向光应用阴影
        float shadowFactor = 1.0;
        if (i == 0) {
            shadowFactor = calculateShadow(fragWorldPos, N, lightDir);
        }

        vec3 radiance = lightColor * intensity;
        Lo += calculateBRDF(N, V, lightDir, radiance, albedo, metallic, roughness, F0) * shadowFactor;
    }

    // ========== 点光源 ==========
    for (int i = 0; i < lights.pointLightCount; i++) {
        vec3 lightPos = lights.pointLights[i].positionAndRange.xyz;
        float range = lights.pointLights[i].positionAndRange.w;
        vec3 lightColor = lights.pointLights[i].colorAndIntensity.rgb;
        float intensity = lights.pointLights[i].colorAndIntensity.w;

        vec3 lightVec = lightPos - fragWorldPos;
        float distance = length(lightVec);
        vec3 L = normalize(lightVec);

        float attenuation = 1.0 / (distance * distance + 0.0001);
        float smoothFactor = 1.0 - smoothstep(range * 0.75, range, distance);
        attenuation *= smoothFactor;

        vec3 radiance = lightColor * intensity * attenuation;
        Lo += calculateBRDF(N, V, L, radiance, albedo, metallic, roughness, F0);
    }

    // ========== 聚光灯 ==========
    for (int i = 0; i < lights.spotLightCount; i++) {
        vec3 lightPos = lights.spotLights[i].positionAndRange.xyz;
        float range = lights.spotLights[i].positionAndRange.w;
        vec3 spotDir = normalize(lights.spotLights[i].directionAndIntensity.xyz);
        float intensity = lights.spotLights[i].directionAndIntensity.w;
        vec3 lightColor = lights.spotLights[i].colorAndCutoff.rgb;
        float innerCutoff = lights.spotLights[i].colorAndCutoff.w;
        float outerCutoff = lights.spotLights[i].outerCutoff.x;

        vec3 lightVec = lightPos - fragWorldPos;
        float distance = length(lightVec);
        vec3 L = normalize(lightVec);

        float theta = dot(L, -spotDir);
        float epsilon = innerCutoff - outerCutoff;
        float spotFactor = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

        float distanceAttenuation = 1.0 / (distance * distance + 0.0001);
        float smoothFactor = 1.0 - smoothstep(range * 0.75, range, distance);
        distanceAttenuation *= smoothFactor;

        vec3 radiance = lightColor * intensity * spotFactor * distanceAttenuation;
        Lo += calculateBRDF(N, V, L, radiance, albedo, metallic, roughness, F0);
    }

    // 环境光
    vec3 ambient = scene.ambientColor * albedo * ao;

    // 最终颜色
    vec3 color = ambient + Lo;

    // 自发光
    color += albedo * material.emission;

    // HDR 色调映射
    color = color / (color + vec3(1.0));

    // 伽马校正
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, alpha);
}
