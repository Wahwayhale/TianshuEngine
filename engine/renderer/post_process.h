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
    SSR,
    TAA,
    MotionBlur,
    DepthOfField,
    Vignette,
    ChromaticAberration,
    ColorGrading
};

// Bloom 设置
struct BloomSettings {
    bool enabled = true;
    float threshold = 1.0f;
    float intensity = 0.04f;
    int iterations = 5;
    float softKnee = 0.5f;
};

// SSAO 设置
struct SSAOSettings {
    bool enabled = true;
    float radius = 0.5f;
    float bias = 0.025f;
    float power = 2.0f;
    int kernelSize = 64;
};

// SSR 设置
struct SSRSettings {
    bool enabled = false;
    float maxDistance = 50.0f;
    float stride = 1.0f;
    int maxSteps = 64;
    float thickness = 0.5f;
};

// TAA 设置
struct TAASettings {
    bool enabled = true;
    float jitterScale = 1.0f;
    float feedback = 0.9f;
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
    float temperature = 0.0f;  // 色温
    float tint = 0.0f;         // 色调
    ToneMappingMode toneMappingMode = ToneMappingMode::ACES;
};

// 后处理设置
struct PostProcessSettings {
    BloomSettings bloom;
    SSAOSettings ssao;
    SSRSettings ssr;
    TAASettings taa;
    ColorGradingSettings colorGrading;

    // 景深
    bool depthOfFieldEnabled = false;
    float focalDistance = 10.0f;
    float focalRange = 5.0f;
    float blurRadius = 5.0f;

    // 运动模糊
    bool motionBlurEnabled = false;
    float motionBlurStrength = 0.5f;
    int motionBlurSamples = 8;

    // 暗角
    bool vignetteEnabled = true;
    float vignetteIntensity = 0.3f;
    float vignetteSmoothness = 0.5f;

    // 色差
    bool chromaticAberrationEnabled = false;
    float chromaticAberrationIntensity = 0.01f;
};

// 后处理 UBO
struct PostProcessUBO {
    // Bloom
    float bloomThreshold;
    float bloomIntensity;
    float bloomSoftKnee;
    int bloomIterations;

    // SSAO
    float ssaoRadius;
    float ssaoBias;
    float ssaoPower;
    int ssaoKernelSize;

    // SSR
    float ssrMaxDistance;
    float ssrStride;
    int ssrMaxSteps;
    float ssrThickness;

    // TAA
    float taaJitterScale;
    float taaFeedback;
    float taaPadding[2];

    // 色彩校正
    float exposure;
    float gamma;
    float saturation;
    float contrast;
    float brightness;
    float temperature;
    float tint;
    int toneMappingMode;

    // 景深
    float dofFocalDistance;
    float dofFocalRange;
    float dofBlurRadius;
    int dofEnabled;

    // 运动模糊
    float motionBlurStrength;
    int motionBlurSamples;
    int motionBlurEnabled;
    float motionBlurPadding;

    // 暗角
    float vignetteIntensity;
    float vignetteSmoothness;
    int vignetteEnabled;
    float vignettePadding;

    // 色差
    float chromaticAberrationIntensity;
    int chromaticAberrationEnabled;
    float chromaticPadding[2];
};

// 后处理系统
class PostProcessSystem {
public:
    PostProcessSystem(Device& device, VkFormat swapchainFormat, uint32_t width, uint32_t height);
    ~PostProcessSystem();

    void recreate(uint32_t width, uint32_t height);

    // 渲染后处理
    void render(VkCommandBuffer commandBuffer, uint32_t frameIndex);

    // 设置
    void setSettings(const PostProcessSettings& settings) { m_settings = settings; }
    PostProcessSettings& getSettings() { return m_settings; }
    const PostProcessSettings& getSettings() const { return m_settings; }

    // 获取渲染通道
    VkRenderPass getRenderPass() const { return m_renderPass; }
    VkFramebuffer getFramebuffer() const { return m_framebuffer; }
    VkExtent2D getExtent() const { return m_extent; }

    // 获取纹理
    VkImageView getColorOutputView() const { return m_colorOutputView; }
    VkSampler getColorSampler() const { return m_colorSampler; }

private:
    void createResources();
    void cleanupResources();
    void createPipelines();
    void createDescriptorSets();
    void updateUBO(uint32_t frameIndex);

    Device& m_device;
    VkFormat m_swapchainFormat;
    VkExtent2D m_extent;

    // 渲染资源
    VkImage m_colorOutputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_colorOutputMemory = VK_NULL_HANDLE;
    VkImageView m_colorOutputView = VK_NULL_HANDLE;
    VkSampler m_colorSampler = VK_NULL_HANDLE;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    // 管线
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    // 描述符
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;

    // UBO
    std::unique_ptr<Buffer> m_uboBuffer;

    // 全屏四边形
    VkBuffer m_quadVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_quadVertexMemory = VK_NULL_HANDLE;

    // 设置
    PostProcessSettings m_settings;
    uint32_t m_frameCount = 2;
};

} // namespace spark
