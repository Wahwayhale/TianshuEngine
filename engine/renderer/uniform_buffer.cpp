#include "uniform_buffer.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

UniformBuffer::UniformBuffer(Device& device, uint32_t frameCount) {
    m_buffers.resize(frameCount);

    for (uint32_t i = 0; i < frameCount; i++) {
        m_buffers[i] = std::make_unique<Buffer>(
            device,
            sizeof(UniformBufferObject),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        // 映射内存保持持久映射
        m_buffers[i]->map();
    }

    SPARK_CORE_INFO("Uniform buffer created with {0} frames.", frameCount);
}

UniformBuffer::~UniformBuffer() {
    // Buffer 的析构函数会处理 unmap 和清理
}

void UniformBuffer::update(uint32_t frameIndex, const UniformBufferObject& ubo) {
    if (frameIndex >= m_buffers.size()) {
        return;
    }

    void* mappedData = m_buffers[frameIndex]->getMappedData();
    if (mappedData) {
        memcpy(mappedData, &ubo, sizeof(UniformBufferObject));
    }
}

VkBuffer UniformBuffer::getBuffer(uint32_t frameIndex) const {
    if (frameIndex >= m_buffers.size()) {
        return VK_NULL_HANDLE;
    }
    return m_buffers[frameIndex]->getBuffer();
}

} // namespace spark
