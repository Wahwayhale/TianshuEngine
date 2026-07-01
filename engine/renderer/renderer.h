#include "vulkan/fwd.h"
#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include "vulkan/instance.h"
#include "renderer/vulkan/device.h"
#include "vulkan/swapchain.h"

struct GLFWwindow;

namespace spark {

class Renderer {
public:
    Renderer(GLFWwindow* window, int width, int height);
    ~Renderer();

    void beginFrame();
    void endFrame();
    void waitIdle();

    void onResize(int width, int height);

    vulkan::Device& getDevice() { return *m_device; }
    VkInstance getInstance() const;
    VkRenderPass getRenderPass() const { return m_renderPass; }
    VkCommandBuffer getCurrentCommandBuffer() const { return m_commandBuffers[m_currentFrame]; }
    uint32_t getCurrentFrame() const { return m_currentFrame; }
    uint32_t getImageIndex() const { return m_imageIndex; }
    VkExtent2D getSwapchainExtent() const { return m_swapchain->getExtent(); }
    VkCommandPool getCommandPool() const { return m_commandPool; }
    uint32_t getSwapchainImageCount() const { return static_cast<uint32_t>(m_framebuffers.size()); }
    VkImageView getSwapchainImageView(uint32_t index) const;
    VkFramebuffer getCurrentFramebuffer() const { return m_framebuffers[m_imageIndex]; }

private:
    void createSurface(GLFWwindow* window);
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void createDepthResources();

    void cleanupFramebuffers();
    void cleanupDepthResources();

    std::unique_ptr<vulkan::Instance> m_instance;
    std::unique_ptr<vulkan::Device> m_device;
    std::unique_ptr<vulkan::Swapchain> m_swapchain;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    std::vector<VkFramebuffer> m_framebuffers;
    std::vector<VkCommandBuffer> m_commandBuffers;

    // 深度缓冲资源
    VkImage m_depthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_depthImageMemory = VK_NULL_HANDLE;
    VkImageView m_depthImageView = VK_NULL_HANDLE;

    // Synchronization
    static const int MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;

    uint32_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;
    bool m_framebufferResized = false;

    GLFWwindow* m_window;
    int m_width, m_height;
};

} // namespace spark
