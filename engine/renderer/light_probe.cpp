#include "light_probe.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

// =============================================
// LightProbe
// =============================================

LightProbe::LightProbe(Device& device, LightProbeType type, const Vec3& position)
    : m_device(device), m_type(type), m_position(position) {

    // 创建探针纹理
    uint32_t blackPixel = 0xFF000000;
    m_texture = std::make_unique<Texture>(m_device, 1, 1, &blackPixel);

    SPARK_CORE_INFO("Light probe created at ({0}, {1}, {2})", position.x, position.y, position.z);
}

LightProbe::~LightProbe() = default;

void LightProbe::capture(VkCommandBuffer commandBuffer) {
    // TODO: 捕获环境光照
    // 需要渲染立方体贴图
}

// =============================================
// LightProbeManager
// =============================================

LightProbeManager& LightProbeManager::get() {
    static LightProbeManager instance;
    return instance;
}

std::shared_ptr<LightProbe> LightProbeManager::createProbe(LightProbeType type, const Vec3& position) {
    auto probe = std::make_shared<LightProbe>(m_device, type, position);
    m_probes.push_back(probe);
    return probe;
}

void LightProbeManager::removeProbe(std::shared_ptr<LightProbe> probe) {
    auto it = std::find(m_probes.begin(), m_probes.end(), probe);
    if (it != m_probes.end()) {
        m_probes.erase(it);
    }
}

std::shared_ptr<LightProbe> LightProbeManager::getNearestProbe(const Vec3& position) const {
    if (m_probes.empty()) return nullptr;

    std::shared_ptr<LightProbe> nearest = nullptr;
    float nearestDist = 1e10f;

    for (const auto& probe : m_probes) {
        float dist = glm::length(probe->getPosition() - position);
        if (dist < nearestDist) {
            nearestDist = dist;
            nearest = probe;
        }
    }

    return nearest;
}

void LightProbeManager::updateAll(VkCommandBuffer commandBuffer) {
    for (auto& probe : m_probes) {
        probe->capture(commandBuffer);
    }
}

} // namespace spark
