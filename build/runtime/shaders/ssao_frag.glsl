#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

// 深度缓冲
layout(set = 0, binding = 0) uniform sampler2D depthTexture;

// SSAO 设置
layout(set = 0, binding = 1) uniform SSAOUBO {
    mat4 projection;
    mat4 view;
    float radius;
    float bias;
    float power;
    int kernelSize;
    vec2 screenSize;
    float noiseScale;
    float padding;
} settings;

// 采样核（在 CPU 端生成）
const int MAX_KERNEL_SIZE = 64;
layout(set = 0, binding = 2) uniform SampleKernel {
    vec4 samples[MAX_KERNEL_SIZE];
} kernel;

// 噪声纹理
layout(set = 0, binding = 3) uniform sampler2D noiseTexture;

// 从深度重建视空间位置
vec3 viewPosFromDepth(vec2 uv, float depth) {
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewPos = inverse(settings.projection) * clipPos;
    return viewPos.xyz / viewPos.w;
}

// 从深度重建视空间法线（使用深度差分）
vec3 viewNormalFromDepth(vec2 uv, float depth) {
    vec2 texelSize = 1.0 / settings.screenSize;

    float depthL = texture(depthTexture, uv - vec2(texelSize.x, 0.0)).r;
    float depthR = texture(depthTexture, uv + vec2(texelSize.x, 0.0)).r;
    float depthU = texture(depthTexture, uv - vec2(0.0, texelSize.y)).r;
    float depthD = texture(depthTexture, uv + vec2(0.0, texelSize.y)).r;

    vec3 posL = viewPosFromDepth(uv - vec2(texelSize.x, 0.0), depthL);
    vec3 posR = viewPosFromDepth(uv + vec2(texelSize.x, 0.0), depthR);
    vec3 posU = viewPosFromDepth(uv - vec2(0.0, texelSize.y), depthU);
    vec3 posD = viewPosFromDepth(uv + vec2(0.0, texelSize.y), depthD);

    vec3 dX = posR - posL;
    vec3 dY = posD - posU;

    return normalize(cross(dX, dY));
}

void main() {
    float depth = texture(depthTexture, fragTexCoord).r;

    // 跳过天空（深度为 1.0）
    if (depth >= 1.0) {
        outColor = vec4(1.0);
        return;
    }

    // 重建视空间位置和法线
    vec3 fragPos = viewPosFromDepth(fragTexCoord, depth);
    vec3 normal = viewNormalFromDepth(fragTexCoord, depth);

    // 获取噪声向量
    vec2 noiseUV = fragTexCoord * settings.noiseScale;
    vec3 randomVec = texture(noiseTexture, noiseUV).xyz * 2.0 - 1.0;

    // 构建 TBN 矩阵
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // 计算 SSAO
    float occlusion = 0.0;
    for (int i = 0; i < settings.kernelSize; i++) {
        // 将采样点从切线空间变换到视空间
        vec3 samplePos = TBN * kernel.samples[i].xyz;
        samplePos = fragPos + samplePos * settings.radius;

        // 投影到屏幕空间
        vec4 offset = settings.projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        // 获取采样点的深度
        float sampleDepth = texture(depthTexture, offset.xy).r;
        vec3 sampleViewPos = viewPosFromDepth(offset.xy, sampleDepth);

        // 范围检查
        float rangeCheck = smoothstep(0.0, 1.0, settings.radius / abs(fragPos.z - sampleViewPos.z));

        // 累加遮蔽
        occlusion += (sampleViewPos.z >= samplePos.z + settings.bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(settings.kernelSize));
    occlusion = pow(occlusion, settings.power);

    outColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
