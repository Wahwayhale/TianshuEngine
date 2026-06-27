#include "descriptor_set.h"
#include "buffer.h"
#include "texture.h"
#include "shadow_map.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <stdexcept>
#include <array>
#include <cstring>

namespace spark {

// =============================================
// UniformBuffer
// =============================================

UniformBuffer::UniformBuffer(Device& device, VkDeviceSize size, uint32_t frameCount)
    : m_size(size) {
    m_buffers.resize(frameCount);

    for (uint32_t i = 0; i < frameCount; i++) {
        m_buffers[i] = std::make_unique<Buffer>(
            device, size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        m_buffers[i]->map();
    }

    SPARK_CORE_INFO("Uniform buffer: {0} frames, {1} bytes.", frameCount, size);
}

UniformBuffer::~UniformBuffer() {}

void UniformBuffer::update(uint32_t frameIndex, const void* data, VkDeviceSize size) {
    if (frameIndex >= m_buffers.size()) return;
    void* mappedData = m_buffers[frameIndex]->getMappedData();
    if (mappedData) {
        memcpy(mappedData, data, size);
    }
}

VkBuffer UniformBuffer::getBuffer(uint32_t frameIndex) const {
    if (frameIndex >= m_buffers.size()) return VK_NULL_HANDLE;
    return m_buffers[frameIndex]->getBuffer();
}

// =============================================
// DescriptorSetLayout
// =============================================

DescriptorSetLayout::DescriptorSetLayout(Device& device) : m_device(device) {
    // Binding 0: Scene UBO
    VkDescriptorSetLayoutBinding sceneBinding{};
    sceneBinding.binding = 0;
    sceneBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sceneBinding.descriptorCount = 1;
    sceneBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // Binding 1: Light UBO
    VkDescriptorSetLayoutBinding lightBinding{};
    lightBinding.binding = 1;
    lightBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightBinding.descriptorCount = 1;
    lightBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Binding 2: Material UBO
    VkDescriptorSetLayoutBinding materialBinding{};
    materialBinding.binding = 2;
    materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    materialBinding.descriptorCount = 1;
    materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Binding 3: Shadow UBO
    VkDescriptorSetLayoutBinding shadowBinding{};
    shadowBinding.binding = 3;
    shadowBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    shadowBinding.descriptorCount = 1;
    shadowBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Binding 4: Albedo Texture
    VkDescriptorSetLayoutBinding textureBinding{};
    textureBinding.binding = 4;
    textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureBinding.descriptorCount = 1;
    textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Binding 5: Shadow Map
    VkDescriptorSetLayoutBinding shadowMapBinding{};
    shadowMapBinding.binding = 5;
    shadowMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    shadowMapBinding.descriptorCount = 1;
    shadowMapBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 6> bindings = {
        sceneBinding, lightBinding, materialBinding,
        shadowBinding, textureBinding, shadowMapBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_device.getDevice(), &layoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout!");
    }

    SPARK_CORE_INFO("Descriptor set layout: 6 bindings (Scene/Light/Material/Shadow/Texture/ShadowMap).");
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (m_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device.getDevice(), m_layout, nullptr);
    }
}

// =============================================
// DescriptorPool
// =============================================

DescriptorPool::DescriptorPool(Device& device, uint32_t maxSets) : m_device(device) {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = maxSets * 4;  // Scene + Light + Material + Shadow
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = maxSets * 2;  // Albedo + ShadowMap

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;

    if (vkCreateDescriptorPool(m_device.getDevice(), &poolInfo, nullptr, &m_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }

    SPARK_CORE_INFO("Descriptor pool: {0} max sets.", maxSets);
}

DescriptorPool::~DescriptorPool() {
    if (m_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device.getDevice(), m_pool, nullptr);
    }
}

// =============================================
// DescriptorSets
// =============================================

DescriptorSets::DescriptorSets(Device& device,
                               VkDescriptorSetLayout layout,
                               VkDescriptorPool pool,
                               uint32_t count)
    : m_device(device) {

    std::vector<VkDescriptorSetLayout> layouts(count, layout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = layouts.data();

    m_sets.resize(count);
    if (vkAllocateDescriptorSets(m_device.getDevice(), &allocInfo, m_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }

    SPARK_CORE_INFO("Descriptor sets allocated: {0}", count);
}

DescriptorSets::~DescriptorSets() {}

void DescriptorSets::bindSceneUBO(uint32_t frameIndex, UniformBuffer& ubo) {
    if (frameIndex >= m_sets.size()) return;

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = ubo.getBuffer(frameIndex);
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(SceneUBO);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_sets[frameIndex];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void DescriptorSets::bindLightUBO(uint32_t frameIndex, UniformBuffer& ubo) {
    if (frameIndex >= m_sets.size()) return;

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = ubo.getBuffer(frameIndex);
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(LightUBO);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_sets[frameIndex];
    descriptorWrite.dstBinding = 1;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void DescriptorSets::bindMaterialUBO(uint32_t frameIndex, UniformBuffer& ubo) {
    if (frameIndex >= m_sets.size()) return;

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = ubo.getBuffer(frameIndex);
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(MaterialUBO);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_sets[frameIndex];
    descriptorWrite.dstBinding = 2;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void DescriptorSets::bindShadowUBO(uint32_t frameIndex, UniformBuffer& ubo) {
    if (frameIndex >= m_sets.size()) return;

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = ubo.getBuffer(frameIndex);
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(ShadowUBO);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_sets[frameIndex];
    descriptorWrite.dstBinding = 3;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void DescriptorSets::bindTexture(uint32_t frameIndex, Texture& texture) {
    if (frameIndex >= m_sets.size()) return;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture.getImageView();
    imageInfo.sampler = texture.getSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_sets[frameIndex];
    descriptorWrite.dstBinding = 4;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void DescriptorSets::bindShadowMap(uint32_t frameIndex, ShadowMap& shadowMap) {
    if (frameIndex >= m_sets.size()) return;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = shadowMap.getImageView();
    imageInfo.sampler = shadowMap.getSampler();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = m_sets[frameIndex];
    descriptorWrite.dstBinding = 5;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

VkDescriptorSet DescriptorSets::getSet(uint32_t frameIndex) const {
    if (frameIndex >= m_sets.size()) return VK_NULL_HANDLE;
    return m_sets[frameIndex];
}

} // namespace spark
