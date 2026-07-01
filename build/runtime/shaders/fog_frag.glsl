#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

// 场景纹理
layout(set = 0, binding = 0) uniform sampler2D sceneTexture;

// 深度缓冲
layout(set = 0, binding = 1) uniform sampler2D depthTexture;

// 雾设置
layout(set = 0, binding = 2) uniform FogUBO {
    mat4 projection;
    vec4 fogColor;        // 雾颜色
    float fogDensity;     // 雾密度
    float fogStart;       // 线性雾开始距离
    float fogEnd;         // 线性雾结束距离
    int fogType;          // 0 = 线性, 1 = 指数, 2 = 指数平方
    float fogHeight;      // 高度雾高度
    float fogHeightFalloff; // 高度雾衰减
    float padding;
} settings;

// 从深度重建视空间位置
vec3 viewPosFromDepth(vec2 uv, float depth) {
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewPos = inverse(settings.projection) * clipPos;
    return viewPos.xyz / viewPos.w;
}

void main() {
    vec4 sceneColor = texture(sceneTexture, fragTexCoord);
    float depth = texture(depthTexture, fragTexCoord).r;

    // 跳过天空
    if (depth >= 1.0) {
        outColor = sceneColor;
        return;
    }

    // 重建视空间位置
    vec3 fragPos = viewPosFromDepth(fragTexCoord, depth);
    float distance = length(fragPos);

    // 计算雾因子
    float fogFactor = 0.0;

    if (settings.fogType == 0) {
        // 线性雾
        fogFactor = (settings.fogEnd - distance) / (settings.fogEnd - settings.fogStart);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
    } else if (settings.fogType == 1) {
        // 指数雾
        fogFactor = exp(-settings.fogDensity * distance);
    } else {
        // 指数平方雾
        fogFactor = exp(-pow(settings.fogDensity * distance, 2.0));
    }

    // 高度雾
    if (settings.fogHeightFalloff > 0.0) {
        float heightFactor = exp(-max(0.0, fragPos.y - settings.fogHeight) * settings.fogHeightFalloff);
        fogFactor *= heightFactor;
    }

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // 混合场景颜色和雾颜色
    vec3 finalColor = mix(settings.fogColor.rgb, sceneColor.rgb, fogFactor);

    outColor = vec4(finalColor, sceneColor.a);
}
