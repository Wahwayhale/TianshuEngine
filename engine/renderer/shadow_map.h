#pragma once

#include <vulkan/vulkan.h>
#include "math/math_types.h"

namespace spark {

class Device;

class ShadowMap {
public:
    ShadowMap(Device& device, uint32_t width = 2048, uint32_t height = 2048);
    ~ShadowMap();

    VkImage getImage() const { return m_image; }
    VkImageView getImageView() const { return m_imageView; }
    VkSampler getSampler() const { return m_sampler; }
    VkFramebuffer getFramebuffer() const { return m_framebuffer; }
    VkRenderPass getRenderPass() const { return m_renderPass; }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    // 计算光源空间矩阵（正交投影，适合方向光）
    Mat4 getLightSpaceMatrix(const Vec3& lightDir, const Vec3& sceneCenter = Vec3(0.0f), float sceneRadius = 15.0f) const;

private:
    void createImage();
    void createImageView();
    void createSampler();
    void createRenderPass();
    void createFramebuffer();

    Device& m_device;
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    uint32_t m_width, m_height;
};

} // namespace spark
