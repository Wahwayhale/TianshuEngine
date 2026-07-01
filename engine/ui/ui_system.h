#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <string>

struct GLFWwindow;

namespace spark::vulkan {
class Device;
}

namespace spark {

using Device = vulkan::Device;

class UISystem {
public:
    UISystem(GLFWwindow* window, Device& device, VkRenderPass renderPass, uint32_t imageCount);
    ~UISystem();

    void beginFrame();
    void endFrame(VkCommandBuffer commandBuffer);

    // 主题管理
    void applyDarkTheme();
    void applyLightTheme();
    void setCustomTheme(const std::string& themeName);

    // 字体管理
    void loadFont(const std::string& path, float size);
    void setDefaultFont();

    // 状态查询
    bool isDarkTheme() const { return m_isDarkTheme; }

private:
    void init(GLFWwindow* window, VkRenderPass renderPass, uint32_t imageCount);
    void setupStyle();

    Device& m_device;
    bool m_isDarkTheme = true;
};

} // namespace spark
