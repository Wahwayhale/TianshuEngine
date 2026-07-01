#pragma once

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace spark {

namespace vulkan {
    class Device;
}

class Renderer;

// ImGui Vulkan 集成
class ImGuiIntegration {
public:
    ImGuiIntegration();
    ~ImGuiIntegration();

    // 初始化
    bool initialize(GLFWwindow* window, Renderer* renderer);
    void shutdown();

    // 渲染
    void newFrame();
    void render(VkCommandBuffer commandBuffer);
    void renderInSeparatePass(VkCommandBuffer commandBuffer, VkImageView swapchainImageView, VkExtent2D extent);

    // 状态
    bool isInitialized() const { return m_initialized; }

    // 获取渲染通道
    VkRenderPass getRenderPass() const { return m_imguiRenderPass; }

private:
    bool createDescriptorPool();
    bool createRenderPass();
    bool initImGuiVulkan();
    bool uploadFonts();

    Renderer* m_renderer = nullptr;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkRenderPass m_imguiRenderPass = VK_NULL_HANDLE;
    bool m_initialized = false;
};

} // namespace spark
