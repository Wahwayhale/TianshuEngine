#include "ui_renderer.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

UIRenderer::UIRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    SPARK_CORE_INFO("UI renderer initialized.");
}

UIRenderer::~UIRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void UIRenderer::beginFrame() {
    m_vertices.clear();
    m_indices.clear();
    m_drawCallCount = 0;
}

void UIRenderer::endFrame() {
    // 批处理结束
}

void UIRenderer::drawRect(const Vec2& position, const Vec2& size, const Vec4& color) {
    // 绘制矩形边框
    drawRectFilled(position, Vec2(size.x, 1.0f), color);
    drawRectFilled(position, Vec2(1.0f, size.y), color);
    drawRectFilled(Vec2(position.x, position.y + size.y - 1.0f), Vec2(size.x, 1.0f), color);
    drawRectFilled(Vec2(position.x + size.x - 1.0f, position.y), Vec2(1.0f, size.y), color);
}

void UIRenderer::drawRectFilled(const Vec2& position, const Vec2& size, const Vec4& color) {
    UIVertex vertices[4] = {
        {position, Vec2(0.0f, 0.0f), color},
        {Vec2(position.x + size.x, position.y), Vec2(1.0f, 0.0f), color},
        {Vec2(position.x + size.x, position.y + size.y), Vec2(1.0f, 1.0f), color},
        {Vec2(position.x, position.y + size.y), Vec2(0.0f, 1.0f), color}
    };

    uint32_t baseIndex = static_cast<uint32_t>(m_vertices.size());

    m_vertices.push_back(vertices[0]);
    m_vertices.push_back(vertices[1]);
    m_vertices.push_back(vertices[2]);
    m_vertices.push_back(vertices[3]);

    m_indices.push_back(baseIndex);
    m_indices.push_back(baseIndex + 1);
    m_indices.push_back(baseIndex + 2);
    m_indices.push_back(baseIndex);
    m_indices.push_back(baseIndex + 2);
    m_indices.push_back(baseIndex + 3);
}

void UIRenderer::drawTexture(const Vec2& position, const Vec2& size, Texture* texture, const Vec4& color) {
    // TODO: 实现纹理绘制
    drawRectFilled(position, size, color);
}

void UIRenderer::drawText(const Vec2& position, const std::string& text, const Vec4& color, float scale) {
    // TODO: 实现文本绘制
}

void UIRenderer::render(VkCommandBuffer commandBuffer) {
    if (m_vertices.empty()) return;

    // TODO: 实现 UI 渲染
    m_drawCallCount++;
}

void UIRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建 UI 渲染管线
    SPARK_CORE_INFO("UI pipeline created (simplified).");
}

void UIRenderer::flushBatch() {
    // TODO: 刷新批次
}

} // namespace spark
