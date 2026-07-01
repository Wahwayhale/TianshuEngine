#include "glass_renderer.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

GlassRenderer::GlassRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    SPARK_CORE_INFO("Glass renderer initialized.");
}

GlassRenderer::~GlassRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void GlassRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // TODO: 实现玻璃渲染
    // 需要折射和反射着色器
}

void GlassRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建玻璃渲染管线
    SPARK_CORE_INFO("Glass pipeline created (simplified).");
}

} // namespace spark
