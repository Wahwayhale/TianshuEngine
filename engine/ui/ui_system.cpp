#include "ui_system.h"
#include "renderer/vulkan/device.h"
#include "core/log.h"
#include "theme.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <stdexcept>

namespace spark {

UISystem::UISystem(GLFWwindow* window, Device& device, VkRenderPass renderPass, uint32_t imageCount)
    : m_device(device) {
    init(window, renderPass, imageCount);
    applyDarkTheme();
    SPARK_CORE_INFO("UI system initialized with beautiful theme.");
}

UISystem::~UISystem() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UISystem::init(GLFWwindow* window, VkRenderPass renderPass, uint32_t imageCount) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // 启用键盘导航和停靠
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= 0;

    // 设置配置标记
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // 设置默认字体
    // 注意：实际项目中应加载自定义字体
    // io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 16.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VK_NULL_HANDLE;  // Would need Vulkan instance
    init_info.PhysicalDevice = m_device.getPhysicalDevice();
    init_info.Device = m_device.getDevice();
    init_info.QueueFamily = m_device.getQueueFamilyIndices().graphicsFamily.value();
    init_info.Queue = m_device.getGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = VK_NULL_HANDLE;
    init_info.MinImageCount = 2;
    init_info.ImageCount = imageCount;

    ImGui_ImplVulkan_Init(&init_info);
}

void UISystem::applyDarkTheme() {
    m_isDarkTheme = true;
    auto theme = ui::themes::createDarkTheme();
    ui::applyTheme(theme);
    SPARK_CORE_INFO("Applied dark theme: {}", theme.name);
}

void UISystem::applyLightTheme() {
    m_isDarkTheme = false;
    auto theme = ui::themes::createLightTheme();
    ui::applyTheme(theme);
    SPARK_CORE_INFO("Applied light theme: {}", theme.name);
}

void UISystem::setCustomTheme(const std::string& themeName) {
    if (themeName == "dark") {
        applyDarkTheme();
    } else if (themeName == "light") {
        applyLightTheme();
    } else {
        SPARK_WARN("Unknown theme: {}, falling back to dark", themeName);
        applyDarkTheme();
    }
}

void UISystem::loadFont(const std::string& path, float size) {
    ImGuiIO& io = ImGui::GetIO();
    // 注意：这里只是示例，实际实现需要处理字体文件加载
    // io.Fonts->AddFontFromFileTTF(path.c_str(), size);
    SPARK_INFO("Font loading requested: {} at size {}", path, size);
}

void UISystem::setDefaultFont() {
    // 使用默认字体
    // 实际项目中应加载自定义字体
}

void UISystem::setupStyle() {
    // 额外的样式微调
    ImGuiStyle& style = ImGui::GetStyle();

    // 确保停靠标签有正确的间距
    style.ScrollbarSize = 2.0f;

    // 调整滚动条
    style.ScrollbarRounding = 6.0f;
    style.ScrollbarSize = 12.0f;
}

void UISystem::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UISystem::endFrame(VkCommandBuffer commandBuffer) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

} // namespace spark
