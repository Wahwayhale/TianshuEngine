#include "vulkan/fwd.h"
#pragma once

#include <vulkan/vulkan.h>
#include <vector>


namespace spark {


class Buffer {
public:
    Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~Buffer();

    VkBuffer getBuffer() const { return m_buffer; }
    VkDeviceMemory getMemory() const { return m_memory; }
    void* getMappedData() const { return m_mappedData; }

    void map();
    void unmap();
    void copyTo(const void* data, VkDeviceSize size);

private:
    Device& m_device;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    void* m_mappedData = nullptr;
    VkDeviceSize m_size;
};

} // namespace spark
