#include "buffer.h"
#include "renderer/vulkan/device.h"
#include "core/log.h"
#include <stdexcept>
#include <cstring>

namespace spark {

Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : m_device(device), m_size(size) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.getDevice(), m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device.getDevice(), m_buffer, m_memory, 0);
}

Buffer::~Buffer() {
    if (m_mappedData) {
        unmap();
    }
    vkDestroyBuffer(m_device.getDevice(), m_buffer, nullptr);
    vkFreeMemory(m_device.getDevice(), m_memory, nullptr);
}

void Buffer::map() {
    if (vkMapMemory(m_device.getDevice(), m_memory, 0, m_size, 0, &m_mappedData) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map buffer memory!");
    }
}

void Buffer::unmap() {
    vkUnmapMemory(m_device.getDevice(), m_memory);
    m_mappedData = nullptr;
}

void Buffer::copyTo(const void* data, VkDeviceSize size) {
    if (!m_mappedData) {
        map();
    }
    memcpy(m_mappedData, data, size);
}

} // namespace spark
