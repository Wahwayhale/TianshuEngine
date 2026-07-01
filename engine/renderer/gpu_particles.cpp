#include "gpu_particles.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <random>
#include <algorithm>

namespace spark {

GPUParticleSystem::GPUParticleSystem(Device& device, const GPUParticleSettings& settings)
    : m_device(device), m_settings(settings) {

    m_particles.resize(m_settings.maxParticles);
    createBuffers();

    SPARK_CORE_INFO("GPU particle system created: max {0} particles", m_settings.maxParticles);
}

GPUParticleSystem::~GPUParticleSystem() {
    if (m_particleBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device.getDevice(), m_particleBuffer, nullptr);
        vkFreeMemory(m_device.getDevice(), m_particleMemory, nullptr);
    }
}

void GPUParticleSystem::update(float deltaTime) {
    if (!m_active) return;

    // 发射新粒子
    m_emissionAccumulator += m_settings.emissionRate * deltaTime;
    int particlesToEmit = static_cast<int>(m_emissionAccumulator);
    m_emissionAccumulator -= particlesToEmit;

    emit(particlesToEmit);

    // 更新粒子
    updateParticles(deltaTime);
}

void GPUParticleSystem::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    if (m_particleCount == 0) return;

    // TODO: 实现 GPU 粒子渲染
    // 需要创建粒子渲染管线和着色器
}

void GPUParticleSystem::emit(int count) {
    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < count && m_particleCount < m_settings.maxParticles; i++) {
        GPUParticle particle;

        // 位置
        particle.position = Vec3(0.0f, 0.0f, 0.0f);

        // 速度（带随机变化）
        particle.velocity = m_settings.initialVelocity + Vec3(
            dist(rng) * m_settings.velocityVariance.x,
            dist(rng) * m_settings.velocityVariance.y,
            dist(rng) * m_settings.velocityVariance.z
        );

        // 颜色
        particle.color = m_settings.startColor;

        // 大小
        particle.size = m_settings.startSize;

        // 生命周期
        particle.lifetime = m_settings.lifetime + dist(rng) * m_settings.lifetimeVariance;
        particle.age = 0.0f;

        m_particles[m_particleCount] = particle;
        m_particleCount++;
    }
}

void GPUParticleSystem::updateParticles(float deltaTime) {
    for (int i = 0; i < m_particleCount; i++) {
        auto& particle = m_particles[i];

        // 更新年龄
        particle.age += deltaTime;

        // 检查是否死亡
        if (particle.age >= particle.lifetime) {
            // 移除粒子（交换到最后）
            m_particles[i] = m_particles[m_particleCount - 1];
            m_particleCount--;
            i--;
            continue;
        }

        // 更新位置
        particle.velocity += m_settings.gravity * deltaTime;
        particle.position += particle.velocity * deltaTime;

        // 更新颜色（插值）
        float t = particle.age / particle.lifetime;
        particle.color = m_settings.startColor * (1.0f - t) + m_settings.endColor * t;

        // 更新大小（插值）
        particle.size = m_settings.startSize * (1.0f - t) + m_settings.endSize * t;
    }
}

void GPUParticleSystem::createBuffers() {
    VkDeviceSize bufferSize = sizeof(GPUParticle) * m_settings.maxParticles;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(m_device.getDevice(), &bufferInfo, nullptr, &m_particleBuffer);

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device.getDevice(), m_particleBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_particleMemory);
    vkBindBufferMemory(m_device.getDevice(), m_particleBuffer, m_particleMemory, 0);
}

} // namespace spark
