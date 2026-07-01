#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

// SSAO 纹理
layout(set = 0, binding = 0) uniform sampler2D ssaoTexture;

// 模糊设置
layout(set = 0, binding = 1) uniform BlurUBO {
    vec2 screenSize;
    int blurSize;
    float padding;
} settings;

void main() {
    vec2 texelSize = 1.0 / settings.screenSize;
    float result = 0.0;

    int halfSize = settings.blurSize / 2;

    for (int x = -halfSize; x <= halfSize; x++) {
        for (int y = -halfSize; y <= halfSize; y++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, fragTexCoord + offset).r;
        }
    }

    float totalSamples = float(settings.blurSize * settings.blurSize);
    result /= totalSamples;

    outColor = vec4(result, result, result, 1.0);
}
