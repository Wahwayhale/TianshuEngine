#pragma once

#include "math/math_types.h"
#include <string>
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Texture;
class Buffer;

// UI 顶点
struct UIVertex {
    Vec2 position;
    Vec2 texCoord;
    Vec4 color;
};

// UI 渲染器
class UIRenderer {
public:
    UIRenderer(Device& device, VkRenderPass renderPass);
    ~UIRenderer();

    // 开始/结束帧
    void beginFrame();
    void endFrame();

    // 绘制命令
    void drawRect(const Vec2& position, const Vec2& size, const Vec4& color);
    void drawRectFilled(const Vec2& position, const Vec2& size, const Vec4& color);
    void drawTexture(const Vec2& position, const Vec2& size, Texture* texture, const Vec4& color = Vec4(1.0f));
    void drawText(const Vec2& position, const std::string& text, const Vec4& color = Vec4(1.0f), float scale = 1.0f);

    // 渲染
    void render(VkCommandBuffer commandBuffer);

    // 统计
    int getDrawCallCount() const { return m_drawCallCount; }

private:
    void createPipeline(VkRenderPass renderPass);
    void flushBatch();

    Device& m_device;

    std::vector<UIVertex> m_vertices;
    std::vector<uint32_t> m_indices;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    int m_drawCallCount = 0;
};

} // namespace spark
