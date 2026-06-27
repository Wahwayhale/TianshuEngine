#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// SSAO 设置
struct SSAOSettings {
    float radius = 0.5f;
    float bias = 0.025f;
    float power = 2.0f;
    int kernelSize = 64;
    int blurSize = 4;
    bool enabled = true;
};

// SSAO UBO
struct SSAOUBO {
    Mat4 projection;
    Mat4 view;
    float radius;
    float bias;
    float power;
    int kernelSize;
    Vec2 screenSize;
    float noiseScale;
    float padding;
};

// 模糊 UBO
struct BlurUBO {
    Vec2 screenSize;
    int blurSize;
    float padding;
};

class SSAO {
public:
    SSAO(Device& device, uint32_t width, uint32_t height);
    ~SSAO();

    void recreate(uint32_t width, uint32_t height);

    // 获取资源
    VkImageView getSSAOImageView() const { return m_ssaoImageView; }
    VkSampler getSSAOSampler() const { return m_ssaoSampler; }
    VkImageView getBlurredImageView() const { return m_blurredImageView; }
    VkSampler getBlurredSampler() const { return m_blurredSampler; }

    // 更新设置
    void updateSettings(const SSAOSettings& settings);

    // 渲染 SSAO
    void render(VkCommandBuffer commandBuffer, VkImageView depthImageView, const Mat4& projection, const Mat4& view);

private:
    void createResources();
    void cleanupResources();
    void createPipelines();
    void generateSampleKernel();

    Device& m_device;
    uint32_t m_width, m_height;

    // SSAO 纹理
    VkImage m_ssaoImage = VK_NULL_HANDLE;
    VkDeviceMemory m_ssaoMemory = VK_NULL_HANDLE;
    VkImageView m_ssaoImageView = VK_NULL_HANDLE;
    VkSampler m_ssaoSampler = VK_NULL_HANDLE;

    // 模糊后纹理
    VkImage m_blurredImage = VK_NULL_HANDLE;
    VkDeviceMemory m_blurredMemory = VK_NULL_HANDLE;
    VkImageView m_blurredImageView = VK_NULL_HANDLE;
    VkSampler m_blurredSampler = VK_NULL_HANDLE;

    // 噪声纹理
    VkImage m_noiseImage = VK_NULL_HANDLE;
    VkDeviceMemory m_noiseMemory = VK_NULL_HANDLE;
    VkImageView m_noiseImageView = VK_NULL_HANDLE;
    VkSampler m_noiseSampler = VK_NULL_HANDLE;

    // 采样核
    std::vector<Vec4> m_sampleKernel;

    // UBO
    std::unique_ptr<Buffer> m_ssaoUBO;
    std::unique_ptr<Buffer> m_blurUBO;
    std::unique_ptr<Buffer> m_kernelUBO;

    // 管线
    VkPipeline m_ssaoPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_ssaoPipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_blurPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_blurPipelineLayout = VK_NULL_HANDLE;

    // 描述符
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_ssaoDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSet m_blurDescriptorSet = VK_NULL_HANDLE;

    // 渲染通道
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkFramebuffer m_ssaoFramebuffer = VK_NULL_HANDLE;
    VkFramebuffer m_blurFramebuffer = VK_NULL_HANDLE;

    // 全屏四边形
    VkBuffer m_quadVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_quadVertexMemory = VK_NULL_HANDLE;

    SSAOSettings m_settings;
};

} // namespace spark
