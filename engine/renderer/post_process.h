#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// 后处理设置
struct PostProcessSettings {
    float exposure = 1.5f;
    float bloomThreshold = 1.0f;
    float bloomIntensity = 0.04f;
    int bloomIterations = 5;
    float gamma = 2.2f;
    float saturation = 1.1f;
    float contrast = 1.1f;
    float brightness = 0.0f;
};

// 后处理 UBO
struct PostProcessUBO {
    float exposure;
    float bloomThreshold;
    float bloomIntensity;
    float gamma;
    float saturation;
    float contrast;
    float brightness;
    float padding;
};

class PostProcess {
public:
    PostProcess(Device& device, VkFormat swapchainFormat, uint32_t width, uint32_t height);
    ~PostProcess();

    void recreate(uint32_t width, uint32_t height);

    VkRenderPass getRenderPass() const { return m_renderPass; }
    VkFramebuffer getFramebuffer() const { return m_framebuffer; }
    VkExtent2D getExtent() const { return m_extent; }

    VkPipeline getPipeline() const { return m_pipeline; }
    VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; }
    VkDescriptorSet getDescriptorSet(uint32_t frameIndex) const;

    void updateSettings(uint32_t frameIndex, const PostProcessSettings& settings);
    void setSwapchainImageView(VkImageView imageView, uint32_t imageIndex);

    void draw(VkCommandBuffer commandBuffer, uint32_t frameIndex);

private:
    void createOffscreenResources();
    void cleanupOffscreenResources();
    void createRenderPass();
    void createFramebuffer();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    void createPipeline(VkFormat swapchainFormat);
    void createFullscreenQuad();
    void createUniformBuffer();

    Device& m_device;
    VkFormat m_swapchainFormat;
    VkExtent2D m_extent;

    // 离屏渲染资源
    VkImage m_colorImage = VK_NULL_HANDLE;
    VkDeviceMemory m_colorMemory = VK_NULL_HANDLE;
    VkImageView m_colorImageView = VK_NULL_HANDLE;
    VkSampler m_colorSampler = VK_NULL_HANDLE;

    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthMemory = VK_NULL_HANDLE;
    VkImageView m_depthImageView = VK_NULL_HANDLE;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    // 后处理管线
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    // 全屏四边形
    VkBuffer m_quadVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_quadVertexMemory = VK_NULL_HANDLE;

    // UBO
    std::unique_ptr<Buffer> m_uboBuffer;
    void* m_uboMapped = nullptr;

    uint32_t m_width, m_height;
    uint32_t m_frameCount = 2;
};

} // namespace spark
