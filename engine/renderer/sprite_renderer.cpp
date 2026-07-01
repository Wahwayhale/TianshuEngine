#include "sprite_renderer.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cmath>

namespace spark {

SpriteRenderer::SpriteRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    createBuffers();

    SPARK_CORE_INFO("Sprite renderer initialized.");
}

SpriteRenderer::~SpriteRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
    if (m_vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device.getDevice(), m_vertexBuffer, nullptr);
        vkFreeMemory(m_device.getDevice(), m_vertexMemory, nullptr);
    }
}

void SpriteRenderer::beginFrame() {
    m_sprites.clear();
}

void SpriteRenderer::endFrame() {
    // 排序精灵（按层级）
    std::sort(m_sprites.begin(), m_sprites.end(),
        [](const Sprite& a, const Sprite& b) {
            return a.layer < b.layer;
        });
}

void SpriteRenderer::drawSprite(const Sprite& sprite) {
    m_sprites.push_back(sprite);
}

void SpriteRenderer::drawSprite(std::shared_ptr<Texture> texture, const Vec2& position, const Vec2& size,
                                 const Vec4& color, float rotation) {
    Sprite sprite;
    sprite.position = position;
    sprite.size = size;
    sprite.color = color;
    sprite.rotation = rotation;
    sprite.texture = texture;
    drawSprite(sprite);
}

void SpriteRenderer::drawRect(const Vec2& position, const Vec2& size, const Vec4& color) {
    Sprite sprite;
    sprite.position = position;
    sprite.size = size;
    sprite.color = color;
    drawSprite(sprite);
}

void SpriteRenderer::drawRectOutline(const Vec2& position, const Vec2& size, const Vec4& color, float thickness) {
    // 绘制矩形边框（4条线）
    // TODO: 实现线条渲染
}

void SpriteRenderer::drawCircle(const Vec2& center, float radius, const Vec4& color, int segments) {
    // 绘制圆形
    // TODO: 实现圆形渲染
}

void SpriteRenderer::render(VkCommandBuffer commandBuffer, const Mat4& projection) {
    if (m_sprites.empty()) return;

    // TODO: 实现精灵渲染
    // 需要创建精灵渲染管线和着色器
}

void SpriteRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建精灵渲染管线
    SPARK_CORE_INFO("Sprite renderer pipeline created (simplified).");
}

void SpriteRenderer::createBuffers() {
    // 创建顶点缓冲
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(float) * 6 * 4;  // 6 个顶点，每个 4 个 float
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    vkCreateBuffer(m_device.getDevice(), &bufferInfo, nullptr, &m_vertexBuffer);

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device.getDevice(), m_vertexBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_vertexMemory);
    vkBindBufferMemory(m_device.getDevice(), m_vertexBuffer, m_vertexMemory, 0);
}

} // namespace spark
