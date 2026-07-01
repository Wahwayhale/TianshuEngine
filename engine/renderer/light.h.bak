#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <string>

namespace spark {

// 光源类型
enum class LightType {
    Directional,
    Point,
    Spot,
    Area
};

// 光源衰减模式
enum class AttenuationMode {
    Linear,
    Quadratic,
    Physical
};

// 光源基类
class Light {
public:
    Light(const std::string& name = "Light");
    virtual ~Light();

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    LightType getType() const { return m_type; }

    Vec3 getColor() const { return m_color; }
    void setColor(const Vec3& color) { m_color = color; }

    float getIntensity() const { return m_intensity; }
    void setIntensity(float intensity) { m_intensity = intensity; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    bool isCastingShadow() const { return m_castShadow; }
    void setCastShadow(bool castShadow) { m_castShadow = castShadow; }

protected:
    std::string m_name;
    LightType m_type;
    Vec3 m_color = Vec3(1.0f);
    float m_intensity = 1.0f;
    bool m_enabled = true;
    bool m_castShadow = true;
};

// 方向光
class DirectionalLight : public Light {
public:
    DirectionalLight(const std::string& name = "Directional Light");

    Vec3 getDirection() const { return m_direction; }
    void setDirection(const Vec3& direction) { m_direction = glm::normalize(direction); }

    int getCascadeCount() const { return m_cascadeCount; }
    void setCascadeCount(int count) { m_cascadeCount = count; }

    float getShadowDistance() const { return m_shadowDistance; }
    void setShadowDistance(float distance) { m_shadowDistance = distance; }

    float getShadowBias() const { return m_shadowBias; }
    void setShadowBias(float bias) { m_shadowBias = bias; }

private:
    Vec3 m_direction = glm::normalize(Vec3(-0.5f, -1.0f, -0.3f));
    int m_cascadeCount = 4;
    float m_shadowDistance = 100.0f;
    float m_shadowBias = 0.005f;
};

// 点光源
class PointLight : public Light {
public:
    PointLight(const std::string& name = "Point Light");

    Vec3 getPosition() const { return m_position; }
    void setPosition(const Vec3& position) { m_position = position; }

    float getRange() const { return m_range; }
    void setRange(float range) { m_range = range; }

    AttenuationMode getAttenuationMode() const { return m_attenuationMode; }
    void setAttenuationMode(AttenuationMode mode) { m_attenuationMode = mode; }

    float calculateAttenuation(float distance) const;

private:
    Vec3 m_position = Vec3(0.0f);
    float m_range = 10.0f;
    AttenuationMode m_attenuationMode = AttenuationMode::Quadratic;
};

// 聚光灯
class SpotLight : public Light {
public:
    SpotLight(const std::string& name = "Spot Light");

    Vec3 getPosition() const { return m_position; }
    void setPosition(const Vec3& position) { m_position = position; }

    Vec3 getDirection() const { return m_direction; }
    void setDirection(const Vec3& direction) { m_direction = glm::normalize(direction); }

    float getInnerCutoff() const { return m_innerCutoff; }
    void setInnerCutoff(float cutoff) { m_innerCutoff = cutoff; }

    float getOuterCutoff() const { return m_outerCutoff; }
    void setOuterCutoff(float cutoff) { m_outerCutoff = cutoff; }

    float getRange() const { return m_range; }
    void setRange(float range) { m_range = range; }

    float calculateAttenuation(float distance) const;
    float calculateSpotFactor(const Vec3& lightDir) const;

private:
    Vec3 m_position = Vec3(0.0f);
    Vec3 m_direction = glm::normalize(Vec3(0.0f, -1.0f, 0.0f));
    float m_innerCutoff = 12.5f;
    float m_outerCutoff = 17.5f;
    float m_range = 15.0f;
};

// 面光源
class AreaLight : public Light {
public:
    AreaLight(const std::string& name = "Area Light");

    Vec3 getPosition() const { return m_position; }
    void setPosition(const Vec3& position) { m_position = position; }

    Vec3 getDirection() const { return m_direction; }
    void setDirection(const Vec3& direction) { m_direction = glm::normalize(direction); }

    Vec2 getSize() const { return m_size; }
    void setSize(const Vec2& size) { m_size = size; }

private:
    Vec3 m_position = Vec3(0.0f);
    Vec3 m_direction = glm::normalize(Vec3(0.0f, -1.0f, 0.0f));
    Vec2 m_size = Vec2(1.0f, 1.0f);
};

// 光照管理器
class LightManager {
public:
    static LightManager& get();

    std::shared_ptr<DirectionalLight> createDirectionalLight(const std::string& name = "Directional Light");
    std::shared_ptr<PointLight> createPointLight(const std::string& name = "Point Light");
    std::shared_ptr<SpotLight> createSpotLight(const std::string& name = "Spot Light");
    std::shared_ptr<AreaLight> createAreaLight(const std::string& name = "Area Light");

    void removeLight(const std::string& name);
    std::shared_ptr<Light> getLight(const std::string& name) const;

    const std::vector<std::shared_ptr<DirectionalLight>>& getDirectionalLights() const { return m_directionalLights; }
    const std::vector<std::shared_ptr<PointLight>>& getPointLights() const { return m_pointLights; }
    const std::vector<std::shared_ptr<SpotLight>>& getSpotLights() const { return m_spotLights; }
    const std::vector<std::shared_ptr<AreaLight>>& getAreaLights() const { return m_areaLights; }

    void clear();
    int getLightCount() const;

private:
    LightManager() = default;

    std::vector<std::shared_ptr<DirectionalLight>> m_directionalLights;
    std::vector<std::shared_ptr<PointLight>> m_pointLights;
    std::vector<std::shared_ptr<SpotLight>> m_spotLights;
    std::vector<std::shared_ptr<AreaLight>> m_areaLights;
};

} // namespace spark
