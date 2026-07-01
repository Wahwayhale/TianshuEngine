#include "decals.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

DecalRenderer::DecalRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    SPARK_CORE_INFO("Decal renderer initialized.");
}

DecalRenderer::~DecalRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void DecalRenderer::addDecal(const DecalData& decal) {
    m_decals.push_back(decal);
}

void DecalRenderer::clearDecals() {
    m_decals.clear();
}

void DecalRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    if (m_decals.empty()) return;

    // TODO: 实现贴花渲染
    // 需要投影纹理到表面
}

void DecalRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建贴花渲染管线
    SPARK_CORE_INFO("Decal pipeline created (simplified).");
}

} // namespace spark
