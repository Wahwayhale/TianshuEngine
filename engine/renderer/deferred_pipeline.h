#include "vulkan/fwd.h"
#include <vulkan/vulkan.h>
#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {


class Buffer;
class Shader;

// G-Buffer 格式
enum class GBufferFormat {
    AlbedoMetallic,      // RGBA8: RGB=Albedo, A=Metallic
    NormalRoughness,     // RGBA16F: RGB=Normal, A=Roughness
    PositionAO,          // RGBA16F: RGB=Position, A=AO
    Emission,            // RGBA8: RGB=Emission
    Depth                // D32SFLOAT
};

// G-Buffer 纹理
struct GBufferTexture {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkFormat format;
};

// 延迟渲染管线
class DeferredPipeline {
public:
    DeferredPipeline(Device& device, uint32_t width, uint32_t height);
    ~DeferredPipeline();

    // 重建（窗口大小改变时）
    void recreate(uint32_t width, uint32_t height);

    // 渲染通道
    void beginGeometryPass(VkCommandBuffer commandBuffer);
    void endGeometryPass(VkCommandBuffer commandBuffer);

    void beginLightingPass(VkCommandBuffer commandBuffer);
    void endLightingPass(VkCommandBuffer commandBuffer);

    // 获取渲染通道
    VkRenderPass getGeometryRenderPass() const { return m_geometryRenderPass; }
    VkRenderPass getLightingRenderPass() const { return m_lightingRenderPass; }

    // 获取 G-Buffer 纹理
    VkImageView getAlbedoView() const { return m_albedo.view; }
    VkImageView getNormalView() const { return m_normal.view; }
    VkImageView getPositionView() const { return m_position.view; }
    VkImageView getEmissionView() const { return m_emission.view; }
    VkImageView getDepthView() const { return m_depth.view; }

    // 获取最终颜色输出
    VkImageView getColorOutputView() const { return m_colorOutput.view; }
    VkSampler getColorSampler() const { return m_colorSampler; }

    // 获取帧缓冲
    VkFramebuffer getGeometryFramebuffer() const { return m_geometryFramebuffer; }
    VkFramebuffer getLightingFramebuffer() const { return m_lightingFramebuffer; }

    // 获取范围
    VkExtent2D getExtent() const { return m_extent; }
    uint32_t getWidth() const { return m_extent.width; }
    uint32_t getHeight() const { return m_extent.height; }

    // 绘制全屏四边形
    void drawFullscreenQuad(VkCommandBuffer commandBuffer);

private:
    void createGBufferTextures();
    void cleanupGBufferTextures();
    void createRenderPasses();
    void createFramebuffers();
    void createPipelines();
    void createDescriptorSets();
    void createFullscreenQuad();

    Device& m_device;
    VkExtent2D m_extent;

    // G-Buffer 纹理
    GBufferTexture m_albedo;
    GBufferTexture m_normal;
    GBufferTexture m_position;
    GBufferTexture m_emission;
    GBufferTexture m_depth;

    // 最终颜色输出
    GBufferTexture m_colorOutput;
    VkSampler m_colorSampler = VK_NULL_HANDLE;

    // 渲染通道
    VkRenderPass m_geometryRenderPass = VK_NULL_HANDLE;
    VkRenderPass m_lightingRenderPass = VK_NULL_HANDLE;

    // 帧缓冲
    VkFramebuffer m_geometryFramebuffer = VK_NULL_HANDLE;
    VkFramebuffer m_lightingFramebuffer = VK_NULL_HANDLE;

    // 管线
    VkPipeline m_lightingPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_lightingPipelineLayout = VK_NULL_HANDLE;

    // 描述符
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;

    // 全屏四边形
    VkBuffer m_quadVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_quadVertexMemory = VK_NULL_HANDLE;

    // 设置
    uint32_t m_width, m_height;
};

} // namespace spark
