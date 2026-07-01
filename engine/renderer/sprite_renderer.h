#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Texture;
class Buffer;

// 精灵数据
struct Sprite {
    Vec2 position;
    Vec2 size;
    Vec4 uvRect = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
    Vec4 color = Vec4(1.0f);
    float rotation = 0.0f;
    int layer = 0;
    std::shared_ptr<Texture> texture;
};

// 精灵渲染器
class SpriteRenderer {
public:
    SpriteRenderer(Device& device, VkRenderPass renderPass);
    ~SpriteRenderer();

    // 开始/结束帧
    void beginFrame();
    void endFrame();

    // 绘制精灵
    void drawSprite(const Sprite& sprite);
    void drawSprite(std::shared_ptr<Texture> texture, const Vec2& position, const Vec2& size,
                    const Vec4& color = Vec4(1.0f), float rotation = 0.0f);

    // 绘制矩形
    void drawRect(const Vec2& position, const Vec2& size, const Vec4& color);
    void drawRectOutline(const Vec2& position, const Vec2& size, const Vec4& color, float thickness = 1.0f);

    // 绘制圆形
    void drawCircle(const Vec2& center, float radius, const Vec4& color, int segments = 32);

    // 渲染
    void render(VkCommandBuffer commandBuffer, const Mat4& projection);

    // 统计
    int getSpriteCount() const { return static_cast<int>(m_sprites.size()); }

private:
    void createPipeline(VkRenderPass renderPass);
    void createBuffers();

    Device& m_device;

    std::vector<Sprite> m_sprites;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_vertexMemory = VK_NULL_HANDLE;
};

} // namespace spark
