#include "atmosphere.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

AtmosphereRenderer::AtmosphereRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    SPARK_CORE_INFO("Atmosphere renderer initialized.");
}

AtmosphereRenderer::~AtmosphereRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void AtmosphereRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // TODO: 渲染大气散射效果
    // 需要专门的大气散射着色器
}

void AtmosphereRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建大气渲染管线
    SPARK_CORE_INFO("Atmosphere pipeline created (simplified).");
}

} // namespace spark
