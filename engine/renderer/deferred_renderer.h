#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// G-Buffer 纹理
struct GBufferTexture {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkFormat format;
};

// 延迟渲染器
class DeferredRenderer {
public:
    DeferredRenderer(Device& device, uint32_t width, uint32_t height);
    ~DeferredRenderer();

    void recreate(uint32_t width, uint32_t height);

    // 渲染通道
    VkRenderPass getGeometryPass() const { return m_geometryPass; }
    VkRenderPass getLightingPass() const { return m_lightingPass; }

    // G-Buffer
    VkImageView getAlbedoView() const { return m_albedo.view; }
    VkImageView getNormalView() const { return m_normal.view; }
    VkImageView getPositionView() const { return m_position.view; }
    VkImageView getDepthView() const { return m_depth.view; }

    // 输出
    VkImageView getColorOutputView() const { return m_colorOutput.view; }

private:
    void createGBuffer();
    void cleanupGBuffer();
    void createRenderPasses();
    void createFramebuffers();

    Device& m_device;
    uint32_t m_width, m_height;

    GBufferTexture m_albedo;
    GBufferTexture m_normal;
    GBufferTexture m_position;
    GBufferTexture m_depth;
    GBufferTexture m_colorOutput;

    VkRenderPass m_geometryPass = VK_NULL_HANDLE;
    VkRenderPass m_lightingPass = VK_NULL_HANDLE;

    VkFramebuffer m_geometryFramebuffer = VK_NULL_HANDLE;
    VkFramebuffer m_lightingFramebuffer = VK_NULL_HANDLE;
};

} // namespace spark
