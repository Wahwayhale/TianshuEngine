#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// 后处理效果类型
enum class PostProcessEffect {
    Bloom,
    SSAO,
    Vignette,
    ColorGrading,
    FXAA
};

// Bloom 设置
struct BloomSettings {
    bool enabled = true;
    float threshold = 1.0f;
    float intensity = 0.04f;
    int iterations = 5;
};

// SSAO 设置
struct SSAOSettings {
    bool enabled = true;
    float radius = 0.5f;
    float bias = 0.025f;
    float power = 2.0f;
};

// 色调映射模式
enum class ToneMappingMode {
    Linear,
    Reinhard,
    ACES,
    AgX,
    Filmic
};

// 色彩校正设置
struct ColorGradingSettings {
    float exposure = 1.5f;
    float gamma = 2.2f;
    float saturation = 1.1f;
    float contrast = 1.1f;
    float brightness = 0.0f;
    ToneMappingMode toneMappingMode = ToneMappingMode::ACES;
};

// 后处理设置
struct PostProcessSettings {
    BloomSettings bloom;
    SSAOSettings ssao;
    ColorGradingSettings colorGrading;

    bool vignetteEnabled = true;
    float vignetteIntensity = 0.3f;
    float vignetteSmoothness = 0.5f;

    bool fxaaEnabled = true;
};

// 后处理 UBO
struct PostProcessUBO {
    // Bloom
    float bloomThreshold;
    float bloomIntensity;
    int bloomIterations;
    float bloomPadding;

    // SSAO
    float ssaoRadius;
    float ssaoBias;
    float ssaoPower;
    int ssaoEnabled;

    // 色彩校正
    float exposure;
    float gamma;
    float saturation;
    float contrast;
    float brightness;
    int toneMappingMode;
    float padding[2];

    // 暗角
    float vignetteIntensity;
    float vignetteSmoothness;
    int vignetteEnabled;
    int fxaaEnabled;
};

// 后处理系统
class PostProcessSystem {
public:
    PostProcessSystem(Device& device, uint32_t width, uint32_t height);
    ~PostProcessSystem();

    void recreate(uint32_t width, uint32_t height);

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

    // 设置
    void setSettings(const PostProcessSettings& settings) { m_settings = settings; }
    PostProcessSettings& getSettings() { return m_settings; }

    // 更新 UBO
    void updateUBO();

private:
    void createResources();
    void cleanupResources();

    Device& m_device;
    uint32_t m_width, m_height;

    // 输出纹理
    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    // UBO
    std::unique_ptr<Buffer> m_uboBuffer;

    // 设置
    PostProcessSettings m_settings;
};

} // namespace spark
