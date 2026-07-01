#include "vulkan/fwd.h"
#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <random>

namespace spark {


class Buffer;

struct Particle {
    Vec3 position = Vec3(0.0f);
    Vec3 velocity = Vec3(0.0f);
    Vec4 color = Vec4(1.0f);
    float life = 1.0f;
    float size = 1.0f;
    float rotation = 0.0f;
    float rotationSpeed = 0.0f;
};

struct ParticleEmitterConfig {
    Vec3 position = Vec3(0.0f);
    Vec3 positionVariance = Vec3(0.5f);
    Vec3 velocity = Vec3(0.0f, 1.0f, 0.0f);
    Vec3 velocityVariance = Vec3(0.5f);
    Vec4 startColor = Vec4(1.0f, 0.5f, 0.0f, 1.0f);
    Vec4 endColor = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
    float startSize = 0.1f;
    float endSize = 0.01f;
    float lifeTime = 2.0f;
    float lifeTimeVariance = 0.5f;
    float emissionRate = 50.0f;
    uint32_t maxParticles = 1000;
    bool looping = true;
};

class ParticleSystem {
public:
    ParticleSystem(Device& device, const ParticleEmitterConfig& config = ParticleEmitterConfig());
    ~ParticleSystem();

    void update(float deltaTime);
    void draw(VkCommandBuffer commandBuffer);

    void emit(uint32_t count = 1);
    void setConfig(const ParticleEmitterConfig& config) { m_config = config; }
    const ParticleEmitterConfig& getConfig() const { return m_config; }

    uint32_t getAliveCount() const { return m_aliveCount; }

private:
    void emitParticle();
    void updateParticles(float deltaTime);
    void swapParticles(uint32_t a, uint32_t b);

    Device& m_device;
    ParticleEmitterConfig m_config;
    std::vector<Particle> m_particles;
    uint32_t m_aliveCount = 0;
    float m_emissionAccumulator = 0.0f;

    std::unique_ptr<Buffer> m_particleBuffer;
    std::mt19937 m_randomEngine;
    std::uniform_real_distribution<float> m_distribution;
};

} // namespace spark
