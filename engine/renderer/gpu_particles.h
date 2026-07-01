#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Buffer;

// GPU 粒子设置
struct GPUParticleSettings {
    int maxParticles = 10000;
    float emissionRate = 100.0f;
    float lifetime = 2.0f;
    float lifetimeVariance = 0.5f;
    Vec3 initialVelocity = Vec3(0.0f, 5.0f, 0.0f);
    Vec3 velocityVariance = Vec3(1.0f, 1.0f, 1.0f);
    Vec4 startColor = Vec4(1.0f, 0.5f, 0.0f, 1.0f);
    Vec4 endColor = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
    float startSize = 0.1f;
    float endSize = 0.01f;
    Vec3 gravity = Vec3(0.0f, -9.81f, 0.0f);
};

// GPU 粒子数据
struct GPUParticle {
    Vec3 position;
    Vec3 velocity;
    Vec4 color;
    float size;
    float lifetime;
    float age;
    float padding;
};

// GPU 粒子系统
class GPUParticleSystem {
public:
    GPUParticleSystem(Device& device, const GPUParticleSettings& settings = {});
    ~GPUParticleSystem();

    // 更新粒子
    void update(float deltaTime);

    // 渲染粒子
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const GPUParticleSettings& settings) { m_settings = settings; }
    const GPUParticleSettings& getSettings() const { return m_settings; }

    // 发射控制
    void emit(int count);
    void setEmissionRate(float rate) { m_settings.emissionRate = rate; }

    // 状态
    int getParticleCount() const { return m_particleCount; }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

private:
    void createBuffers();
    void updateParticles(float deltaTime);

    Device& m_device;
    GPUParticleSettings m_settings;

    // 粒子数据
    std::vector<GPUParticle> m_particles;
    int m_particleCount = 0;
    bool m_active = true;
    float m_emissionAccumulator = 0.0f;

    // GPU 缓冲
    VkBuffer m_particleBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_particleMemory = VK_NULL_HANDLE;
};

} // namespace spark
