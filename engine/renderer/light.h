#pragma once

#include "math/math_types.h"
#include <vector>

namespace spark {

struct DirectionalLight {
    Vec3 direction = Vec3(-0.2f, -1.0f, -0.3f);
    Vec3 color = Vec3(1.0f);
    float intensity = 1.0f;
};

struct PointLight {
    Vec3 position = Vec3(0.0f);
    Vec3 color = Vec3(1.0f);
    float intensity = 1.0f;
    float range = 10.0f;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotLight {
    Vec3 position = Vec3(0.0f);
    Vec3 direction = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 color = Vec3(1.0f);
    float intensity = 1.0f;
    float innerCutoff = 12.5f;  // degrees
    float outerCutoff = 17.5f;  // degrees
};

class LightManager {
public:
    void addDirectionalLight(const DirectionalLight& light) { m_directionalLights.push_back(light); }
    void addPointLight(const PointLight& light) { m_pointLights.push_back(light); }
    void addSpotLight(const SpotLight& light) { m_spotLights.push_back(light); }

    void clear() {
        m_directionalLights.clear();
        m_pointLights.clear();
        m_spotLights.clear();
    }

    const std::vector<DirectionalLight>& getDirectionalLights() const { return m_directionalLights; }
    const std::vector<PointLight>& getPointLights() const { return m_pointLights; }
    const std::vector<SpotLight>& getSpotLights() const { return m_spotLights; }

private:
    std::vector<DirectionalLight> m_directionalLights;
    std::vector<PointLight> m_pointLights;
    std::vector<SpotLight> m_spotLights;
};

} // namespace spark
