#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

// G-Buffer 输入
layout(set = 0, binding = 0) uniform sampler2D gAlbedo;
layout(set = 0, binding = 1) uniform sampler2D gNormal;
layout(set = 0, binding = 2) uniform sampler2D gPosition;

// 场景 UBO
layout(set = 0, binding = 3) uniform SceneUBO {
    mat4 view;
    mat4 projection;
    vec3 cameraPos;
    float padding0;
    vec3 ambientColor;
    float padding1;
} scene;

// 光源结构体
struct DirectionalLight {
    vec4 directionAndIntensity;
    vec4 color;
};

struct PointLight {
    vec4 positionAndRange;
    vec4 colorAndIntensity;
};

struct SpotLight {
    vec4 positionAndRange;
    vec4 directionAndIntensity;
    vec4 colorAndCutoff;
    vec4 outerCutoff;
};

const int MAX_DIRECTIONAL_LIGHTS = 4;
const int MAX_POINT_LIGHTS = 16;
const int MAX_SPOT_LIGHTS = 8;

// 光源 UBO
layout(set = 0, binding = 4) uniform LightUBO {
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    int directionalLightCount;
    int pointLightCount;
    int spotLightCount;
    float padding;
} lights;

const float PI = 3.14159265359;

// PBR 函数
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
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calculateBRDF(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness, vec3 F0) {
    vec3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    vec3 diffuse = kD * albedo / PI;

    float NdotL = max(dot(N, L), 0.0);
    return (diffuse + specular) * radiance * NdotL;
}

void main() {
    // 从 G-Buffer 读取数据
    vec4 albedoMetallic = texture(gAlbedo, fragTexCoord);
    vec4 normalRoughness = texture(gNormal, fragTexCoord);
    vec4 positionAO = texture(gPosition, fragTexCoord);

    vec3 albedo = albedoMetallic.rgb;
    float metallic = albedoMetallic.a;
    vec3 normal = normalRoughness.rgb * 2.0 - 1.0;  // 解码法线
    float roughness = normalRoughness.a;
    vec3 worldPos = positionAO.rgb;
    float ao = positionAO.a;

    // 视线方向
    vec3 V = normalize(scene.cameraPos - worldPos);

    // F0
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // 累积光照
    vec3 Lo = vec3(0.0);

    // 方向光
    for (int i = 0; i < lights.directionalLightCount; i++) {
        vec3 lightDir = -normalize(lights.directionalLights[i].directionAndIntensity.xyz);
        float intensity = lights.directionalLights[i].directionAndIntensity.w;
        vec3 lightColor = lights.directionalLights[i].color.rgb;
        vec3 radiance = lightColor * intensity;
        Lo += calculateBRDF(normal, V, lightDir, radiance, albedo, metallic, roughness, F0);
    }

    // 点光源
    for (int i = 0; i < lights.pointLightCount; i++) {
        vec3 lightPos = lights.pointLights[i].positionAndRange.xyz;
        float range = lights.pointLights[i].positionAndRange.w;
        vec3 lightColor = lights.pointLights[i].colorAndIntensity.rgb;
        float intensity = lights.pointLights[i].colorAndIntensity.w;

        vec3 lightVec = lightPos - worldPos;
        float distance = length(lightVec);
        vec3 L = normalize(lightVec);

        float attenuation = 1.0 / (distance * distance + 0.0001);
        float smoothFactor = 1.0 - smoothstep(range * 0.75, range, distance);
        attenuation *= smoothFactor;

        vec3 radiance = lightColor * intensity * attenuation;
        Lo += calculateBRDF(normal, V, L, radiance, albedo, metallic, roughness, F0);
    }

    // 聚光灯
    for (int i = 0; i < lights.spotLightCount; i++) {
        vec3 lightPos = lights.spotLights[i].positionAndRange.xyz;
        float range = lights.spotLights[i].positionAndRange.w;
        vec3 spotDir = normalize(lights.spotLights[i].directionAndIntensity.xyz);
        float intensity = lights.spotLights[i].directionAndIntensity.w;
        vec3 lightColor = lights.spotLights[i].colorAndCutoff.rgb;
        float innerCutoff = lights.spotLights[i].colorAndCutoff.w;
        float outerCutoff = lights.spotLights[i].outerCutoff.x;

        vec3 lightVec = lightPos - worldPos;
        float distance = length(lightVec);
        vec3 L = normalize(lightVec);

        float theta = dot(L, -spotDir);
        float epsilon = innerCutoff - outerCutoff;
        float spotFactor = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

        float distanceAttenuation = 1.0 / (distance * distance + 0.0001);
        float smoothFactor = 1.0 - smoothstep(range * 0.75, range, distance);
        distanceAttenuation *= smoothFactor;

        vec3 radiance = lightColor * intensity * spotFactor * distanceAttenuation;
        Lo += calculateBRDF(normal, V, L, radiance, albedo, metallic, roughness, F0);
    }

    // 环境光
    vec3 ambient = scene.ambientColor * albedo * ao;

    // 最终颜色
    vec3 color = ambient + Lo;

    outColor = vec4(color, 1.0);
}
