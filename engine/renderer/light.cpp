#include "light.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

// =============================================
// Light 基类
// =============================================

Light::Light(const std::string& name) : m_name(name) {}
Light::~Light() = default;

// =============================================
// DirectionalLight
// =============================================

DirectionalLight::DirectionalLight(const std::string& name) : Light(name) {
    m_type = LightType::Directional;
}

// =============================================
// PointLight
// =============================================

PointLight::PointLight(const std::string& name) : Light(name) {
    m_type = LightType::Point;
}

float PointLight::calculateAttenuation(float distance) const {
    if (distance >= m_range) return 0.0f;

    switch (m_attenuationMode) {
        case AttenuationMode::Linear:
            return 1.0f - (distance / m_range);

        case AttenuationMode::Quadratic: {
            float d2 = distance * distance;
            float r2 = m_range * m_range;
            return 1.0f / (1.0f + d2 / r2);
        }

        case AttenuationMode::Physical: {
            // 物理正确的平方反比衰减
            float d2 = distance * distance;
            return 1.0f / (d2 + 0.0001f);
        }

        default:
            return 1.0f;
    }
}

// =============================================
// SpotLight
// =============================================

SpotLight::SpotLight(const std::string& name) : Light(name) {
    m_type = LightType::Spot;
}

float SpotLight::calculateAttenuation(float distance) const {
    if (distance >= m_range) return 0.0f;

    float d2 = distance * distance;
    float r2 = m_range * m_range;
    return 1.0f / (1.0f + d2 / r2);
}

float SpotLight::calculateSpotFactor(const Vec3& lightDir) const {
    float theta = glm::dot(lightDir, -m_direction);
    float innerCutoffCos = cos(glm::radians(m_innerCutoff));
    float outerCutoffCos = cos(glm::radians(m_outerCutoff));

    float epsilon = innerCutoffCos - outerCutoffCos;
    return glm::clamp((theta - outerCutoffCos) / epsilon, 0.0f, 1.0f);
}

// =============================================
// AreaLight
// =============================================

AreaLight::AreaLight(const std::string& name) : Light(name) {
    m_type = LightType::Area;
}

// =============================================
// LightManager
// =============================================

LightManager& LightManager::get() {
    static LightManager instance;
    return instance;
}

std::shared_ptr<DirectionalLight> LightManager::createDirectionalLight(const std::string& name) {
    auto light = std::make_shared<DirectionalLight>(name);
    m_directionalLights.push_back(light);
    SPARK_CORE_INFO("Created directional light: {0}", name);
    return light;
}

std::shared_ptr<PointLight> LightManager::createPointLight(const std::string& name) {
    auto light = std::make_shared<PointLight>(name);
    m_pointLights.push_back(light);
    SPARK_CORE_INFO("Created point light: {0}", name);
    return light;
}

std::shared_ptr<SpotLight> LightManager::createSpotLight(const std::string& name) {
    auto light = std::make_shared<SpotLight>(name);
    m_spotLights.push_back(light);
    SPARK_CORE_INFO("Created spot light: {0}", name);
    return light;
}

std::shared_ptr<AreaLight> LightManager::createAreaLight(const std::string& name) {
    auto light = std::make_shared<AreaLight>(name);
    m_areaLights.push_back(light);
    SPARK_CORE_INFO("Created area light: {0}", name);
    return light;
}

void LightManager::removeLight(const std::string& name) {
    auto removeByName = [&name](auto& vec) {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [&name](const auto& light) { return light->getName() == name; }),
            vec.end()
        );
    };

    removeByName(m_directionalLights);
    removeByName(m_pointLights);
    removeByName(m_spotLights);
    removeByName(m_areaLights);
}

std::shared_ptr<Light> LightManager::getLight(const std::string& name) const {
    for (const auto& light : m_directionalLights) {
        if (light->getName() == name) return light;
    }
    for (const auto& light : m_pointLights) {
        if (light->getName() == name) return light;
    }
    for (const auto& light : m_spotLights) {
        if (light->getName() == name) return light;
    }
    for (const auto& light : m_areaLights) {
        if (light->getName() == name) return light;
    }
    return nullptr;
}

void LightManager::clear() {
    m_directionalLights.clear();
    m_pointLights.clear();
    m_spotLights.clear();
    m_areaLights.clear();
}

int LightManager::getLightCount() const {
    return static_cast<int>(
        m_directionalLights.size() +
        m_pointLights.size() +
        m_spotLights.size() +
        m_areaLights.size()
    );
}

} // namespace spark
