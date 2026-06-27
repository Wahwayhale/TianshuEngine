#include "particle_system.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

ParticleSystem::ParticleSystem(Device& device, const ParticleEmitterConfig& config)
    : m_device(device), m_config(config), m_distribution(0.0f, 1.0f) {

    m_particles.resize(m_config.maxParticles);
    m_randomEngine.seed(std::random_device()());

    SPARK_CORE_INFO("Particle system created with {0} max particles", m_config.maxParticles);
}

ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::update(float deltaTime) {
    // Emit new particles
    m_emissionAccumulator += m_config.emissionRate * deltaTime;

    while (m_emissionAccumulator >= 1.0f) {
        emitParticle();
        m_emissionAccumulator -= 1.0f;
    }

    // Update existing particles
    updateParticles(deltaTime);
}

void ParticleSystem::draw(VkCommandBuffer commandBuffer) {
    if (m_aliveCount == 0) return;

    // Draw particles
    // This would use instanced rendering in a real implementation
}

void ParticleSystem::emit(uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        emitParticle();
    }
}

void ParticleSystem::emitParticle() {
    if (m_aliveCount >= m_config.maxParticles) return;

    Particle& particle = m_particles[m_aliveCount];

    // Position with variance
    particle.position = m_config.position + Vec3(
        (m_distribution(m_randomEngine) - 0.5f) * 2.0f * m_config.positionVariance.x,
        (m_distribution(m_randomEngine) - 0.5f) * 2.0f * m_config.positionVariance.y,
        (m_distribution(m_randomEngine) - 0.5f) * 2.0f * m_config.positionVariance.z
    );

    // Velocity with variance
    particle.velocity = m_config.velocity + Vec3(
        (m_distribution(m_randomEngine) - 0.5f) * 2.0f * m_config.velocityVariance.x,
        (m_distribution(m_randomEngine) - 0.5f) * 2.0f * m_config.velocityVariance.y,
        (m_distribution(m_randomEngine) - 0.5f) * 2.0f * m_config.velocityVariance.z
    );

    // Color
    particle.color = m_config.startColor;

    // Size
    particle.size = m_config.startSize;

    // Life
    particle.life = m_config.lifeTime + (m_distribution(m_randomEngine) - 0.5f) * 2.0f * m_config.lifeTimeVariance;

    // Rotation
    particle.rotation = m_distribution(m_randomEngine) * 360.0f;
    particle.rotationSpeed = (m_distribution(m_randomEngine) - 0.5f) * 2.0f * 180.0f;

    m_aliveCount++;
}

void ParticleSystem::updateParticles(float deltaTime) {
    for (uint32_t i = 0; i < m_aliveCount; i++) {
        Particle& particle = m_particles[i];

        // Update life
        particle.life -= deltaTime;

        if (particle.life <= 0.0f) {
            // Swap with last alive particle and decrease count
            swapParticles(i, m_aliveCount - 1);
            m_aliveCount--;
            i--;
            continue;
        }

        // Calculate life ratio (0 = just born, 1 = about to die)
        float lifeRatio = 1.0f - (particle.life / m_config.lifeTime);

        // Update position
        particle.position += particle.velocity * deltaTime;

        // Apply gravity
        particle.velocity.y -= 9.81f * deltaTime;

        // Update color (interpolate between start and end)
        particle.color = glm::mix(m_config.startColor, m_config.endColor, lifeRatio);

        // Update size (interpolate between start and end)
        particle.size = glm::mix(m_config.startSize, m_config.endSize, lifeRatio);

        // Update rotation
        particle.rotation += particle.rotationSpeed * deltaTime;
    }
}

void ParticleSystem::swapParticles(uint32_t a, uint32_t b) {
    if (a == b) return;
    std::swap(m_particles[a], m_particles[b]);
}

} // namespace spark
