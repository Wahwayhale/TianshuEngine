#include "skin_renderer.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

SkinRenderer::SkinRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    SPARK_CORE_INFO("Skin renderer initialized.");
}

SkinRenderer::~SkinRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void SkinRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // TODO: 实现皮肤渲染
    // 需要次表面散射着色器
}

void SkinRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建皮肤渲染管线
    SPARK_CORE_INFO("Skin pipeline created (simplified).");
}

} // namespace spark
