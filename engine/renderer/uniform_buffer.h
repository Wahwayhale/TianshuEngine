#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <cstring>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// Uniform Buffer Object 结构体
struct UniformBufferObject {
    Mat4 model;
    Mat4 view;
    Mat4 projection;
};

// Uniform Buffer 类 - 支持多帧缓冲
class UniformBuffer {
public:
    UniformBuffer(Device& device, uint32_t frameCount = 2);
    ~UniformBuffer();

    // 更新指定帧的 UBO 数据
    void update(uint32_t frameIndex, const UniformBufferObject& ubo);

    // 获取指定帧的 buffer
    VkBuffer getBuffer(uint32_t frameIndex) const;

    // 获取帧数
    uint32_t getFrameCount() const { return static_cast<uint32_t>(m_buffers.size()); }

private:
    std::vector<std::unique_ptr<Buffer>> m_buffers;
};

} // namespace spark
