#include "vulkan/fwd.h"
#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {



// G-Buffer 格式
struct GBuffer {
    // 颜色 + 金属度 (RGBA8)
    VkImage albedoImage = VK_NULL_HANDLE;
    VkDeviceMemory albedoMemory = VK_NULL_HANDLE;
    VkImageView albedoView = VK_NULL_HANDLE;

    // 法线 + 粗糙度 (RGBA16F)
    VkImage normalImage = VK_NULL_HANDLE;
    VkDeviceMemory normalMemory = VK_NULL_HANDLE;
    VkImageView normalView = VK_NULL_HANDLE;

    // 世界位置 (RGBA16F)
    VkImage positionImage = VK_NULL_HANDLE;
    VkDeviceMemory positionMemory = VK_NULL_HANDLE;
    VkImageView positionView = VK_NULL_HANDLE;

    // 深度缓冲
    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthMemory = VK_NULL_HANDLE;
    VkImageView depthView = VK_NULL_HANDLE;
};

// 延迟渲染器
class DeferredRenderer {
public:
    DeferredRenderer(Device& device, uint32_t width, uint32_t height);
    ~DeferredRenderer();

    void recreate(uint32_t width, uint32_t height);

    // 获取渲染通道
    VkRenderPass getGeometryPass() const { return m_geometryPass; }
    VkRenderPass getLightingPass() const { return m_lightingPass; }

    // 获取帧缓冲
    VkFramebuffer getGeometryFramebuffer() const { return m_geometryFramebuffer; }
    VkFramebuffer getLightingFramebuffer() const { return m_lightingFramebuffer; }

    // 获取 G-Buffer 纹理
    VkImageView getAlbedoView() const { return m_gbuffer.albedoView; }
    VkImageView getNormalView() const { return m_gbuffer.normalView; }
    VkImageView getPositionView() const { return m_gbuffer.positionView; }
    VkImageView getDepthView() const { return m_gbuffer.depthView; }

    // 获取采样器
    VkSampler getSampler() const { return m_sampler; }

    // 获取最终颜色输出
    VkImageView getColorOutputView() const { return m_colorOutputView; }

    VkExtent2D getExtent() const { return m_extent; }

private:
    void createGBuffer();
    void cleanupGBuffer();
    void createRenderPasses();
    void createFramebuffers();
    void createSampler();
    void createColorOutput();

    Device& m_device;
    VkExtent2D m_extent;

    GBuffer m_gbuffer;

    // 最终颜色输出
    VkImage m_colorOutputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_colorOutputMemory = VK_NULL_HANDLE;
    VkImageView m_colorOutputView = VK_NULL_HANDLE;

    // 渲染通道
    VkRenderPass m_geometryPass = VK_NULL_HANDLE;
    VkRenderPass m_lightingPass = VK_NULL_HANDLE;

    // 帧缓冲
    VkFramebuffer m_geometryFramebuffer = VK_NULL_HANDLE;
    VkFramebuffer m_lightingFramebuffer = VK_NULL_HANDLE;

    // 采样器
    VkSampler m_sampler = VK_NULL_HANDLE;
};

} // namespace spark
