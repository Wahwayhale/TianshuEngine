#include "imgui_integration.h"
#include "renderer/renderer.h"
#include "renderer/vulkan/device.h"
#include "core/log.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <array>

namespace spark {

ImGuiIntegration::ImGuiIntegration() = default;
ImGuiIntegration::~ImGuiIntegration() = default;

bool ImGuiIntegration::initialize(GLFWwindow* window, Renderer* renderer) {
    m_renderer = renderer;

    // 创建 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 设置样式
    ImGui::StyleColorsDark();

    // 初始化 GLFW 后端
    ImGui_ImplGlfw_InitForVulkan(window, true);

    // 创建描述符池
    if (!createDescriptorPool()) {
        SPARK_ERROR("Failed to create ImGui descriptor pool!");
        return false;
    }

    // 创建 ImGui 专用渲染通道
    if (!createRenderPass()) {
        SPARK_ERROR("Failed to create ImGui render pass!");
        return false;
    }

    // 初始化 Vulkan 后端
    if (!initImGuiVulkan()) {
        SPARK_ERROR("Failed to initialize ImGui Vulkan backend!");
        return false;
    }

    m_initialized = true;
    SPARK_INFO("ImGui integration initialized successfully.");
    return true;
}

void ImGuiIntegration::shutdown() {
    if (!m_initialized) return;

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_imguiRenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_renderer->getDevice().getDevice(), m_imguiRenderPass, nullptr);
    }

    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_renderer->getDevice().getDevice(), m_descriptorPool, nullptr);
    }

    m_initialized = false;
    SPARK_INFO("ImGui integration shutdown.");
}

void ImGuiIntegration::newFrame() {
    if (!m_initialized) return;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiIntegration::render(VkCommandBuffer commandBuffer) {
    if (!m_initialized) return;

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

bool ImGuiIntegration::createDescriptorPool() {
    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000;
    poolInfo.poolSizeCount = sizeof(poolSizes) / sizeof(poolSizes[0]);
    poolInfo.pPoolSizes = poolSizes;

    if (vkCreateDescriptorPool(m_renderer->getDevice().getDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool ImGuiIntegration::createRenderPass() {
    // 创建 ImGui 专用渲染通道（只有颜色附件）
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;  // 交换链格式
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // 清除
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_renderer->getDevice().getDevice(), &renderPassInfo, nullptr, &m_imguiRenderPass) != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool ImGuiIntegration::initImGuiVulkan() {
    // 初始化 Vulkan 后端
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.ApiVersion = VK_API_VERSION_1_3;
    init_info.Instance = m_renderer->getInstance();
    init_info.PhysicalDevice = m_renderer->getDevice().getPhysicalDevice();
    init_info.Device = m_renderer->getDevice().getDevice();
    init_info.QueueFamily = m_renderer->getDevice().getQueueFamilyIndices().graphicsFamily.value();
    init_info.Queue = m_renderer->getDevice().getGraphicsQueue();
    init_info.DescriptorPool = m_descriptorPool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = m_renderer->getSwapchainImageCount();

    // 使用专用渲染通道
    init_info.PipelineInfoMain.RenderPass = m_imguiRenderPass;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    if (!ImGui_ImplVulkan_Init(&init_info)) {
        return false;
    }

    return true;
}

void ImGuiIntegration::renderInSeparatePass(VkCommandBuffer commandBuffer, VkImageView swapchainImageView, VkExtent2D extent) {
    if (!m_initialized) return;

    // 创建临时帧缓冲
    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_imguiRenderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &swapchainImageView;
    fbInfo.width = extent.width;
    fbInfo.height = extent.height;
    fbInfo.layers = 1;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(m_renderer->getDevice().getDevice(), &fbInfo, nullptr, &framebuffer) != VK_SUCCESS) {
        return;
    }

    // 开始 ImGui 渲染通道
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_imguiRenderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearValue{};
    clearValue.color = {{0.1f, 0.1f, 0.15f, 1.0f}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // 渲染 ImGui
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    vkCmdEndRenderPass(commandBuffer);

    // 清理帧缓冲
    vkDestroyFramebuffer(m_renderer->getDevice().getDevice(), framebuffer, nullptr);
}

} // namespace spark
