#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace spark::vulkan {

class Device;

class Swapchain {
public:
    Swapchain(Device& device, VkSurfaceKHR surface, int width, int height);
    ~Swapchain();

    void recreate(int width, int height);

    VkSwapchainKHR getSwapchain() const { return m_swapchain; }
    VkFormat getImageFormat() const { return m_imageFormat; }
    VkExtent2D getExtent() const { return m_extent; }
    size_t getImageCount() const { return m_images.size(); }
    VkImageView getImageView(size_t index) const { return m_imageViews[index]; }

private:
    void create(int width, int height);
    void cleanup();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);

    Device& m_device;
    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkFormat m_imageFormat;
    VkExtent2D m_extent;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

} // namespace spark::vulkan
