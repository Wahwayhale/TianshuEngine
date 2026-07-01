#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

// 离屏渲染结果
layout(set = 0, binding = 0) uniform sampler2D hdrTexture;

// 后处理设置
layout(set = 0, binding = 1) uniform PostProcessUBO {
    float exposure;
    float bloomThreshold;
    float bloomIntensity;
    float gamma;
    float saturation;
    float contrast;
    float brightness;
    float padding;
} settings;

// =============================================
// 色调映射 (ACES 近似)
// =============================================
vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// =============================================
// 简单 Bloom（高斯模糊近似）
// =============================================
vec3 bloomSample(sampler2D tex, vec2 uv, vec2 texelSize) {
    vec3 sum = vec3(0.0);
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    // 中心采样
    vec3 center = texture(tex, uv).rgb;
    float brightness = dot(center, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > settings.bloomThreshold) {
        sum += center * weights[0];

        // 水平方向
        for (int i = 1; i < 5; i++) {
            vec2 offset = vec2(texelSize.x * float(i), 0.0);
            sum += texture(tex, uv + offset).rgb * weights[i];
            sum += texture(tex, uv - offset).rgb * weights[i];
        }

        // 垂直方向
        for (int i = 1; i < 5; i++) {
            vec2 offset = vec2(0.0, texelSize.y * float(i));
            sum += texture(tex, uv + offset).rgb * weights[i];
            sum += texture(tex, uv - offset).rgb * weights[i];
        }
    }

    return sum;
}

// =============================================
// 主函数
// =============================================
void main() {
    vec2 texelSize = 1.0 / textureSize(hdrTexture, 0);

    // 采样 HDR 纹理
    vec3 hdrColor = texture(hdrTexture, fragTexCoord).rgb;

    // Bloom
    vec3 bloom = bloomSample(hdrTexture, fragTexCoord, texelSize);
    hdrColor += bloom * settings.bloomIntensity;

    // 曝光
    hdrColor *= settings.exposure;

    // 色调映射 (ACES)
    vec3 color = ACESFilm(hdrColor);

    // 亮度调整
    color += settings.brightness;

    // 对比度调整
    color = (color - 0.5) * settings.contrast + 0.5;

    // 饱和度调整
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    color = mix(vec3(luminance), color, settings.saturation);

    // Gamma 校正
    color = pow(color, vec3(1.0 / settings.gamma));

    // 裁剪到 [0, 1]
    color = clamp(color, 0.0, 1.0);

    outColor = vec4(color, 1.0);
}
