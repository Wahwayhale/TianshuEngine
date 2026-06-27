#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <memory>

namespace spark {

class Device;
class Buffer;

class Texture {
public:
    Texture(Device& device, const std::string& filepath);
    Texture(Device& device, uint32_t width, uint32_t height, const void* data);
    ~Texture();

    VkImage getImage() const { return m_image; }
    VkImageView getImageView() const { return m_imageView; }
    VkSampler getSampler() const { return m_sampler; }
    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

private:
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void createImageView(VkFormat format);
    void createSampler();
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);

    Device& m_device;
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    uint32_t m_width, m_height;
};

} // namespace spark
