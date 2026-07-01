#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Texture;

// 光照探针类型
enum class LightProbeType {
    Reflection,     // 反射探针
    Irradiance,     // 辐照度探针
    Volume          // 体积探针
};

// 光照探针
class LightProbe {
public:
    LightProbe(Device& device, LightProbeType type, const Vec3& position);
    ~LightProbe();

    // 捕获环境
    void capture(VkCommandBuffer commandBuffer);

    // 属性
    LightProbeType getType() const { return m_type; }
    const Vec3& getPosition() const { return m_position; }
    void setPosition(const Vec3& position) { m_position = position; }

    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }

    // 获取纹理
    Texture* getTexture() const { return m_texture.get(); }

private:
    Device& m_device;
    LightProbeType m_type;
    Vec3 m_position;
    float m_radius = 10.0f;

    std::unique_ptr<Texture> m_texture;
};

// 光照探针管理器
class LightProbeManager {
public:
    static LightProbeManager& get();

    // 创建探针
    std::shared_ptr<LightProbe> createProbe(LightProbeType type, const Vec3& position);

    // 删除探针
    void removeProbe(std::shared_ptr<LightProbe> probe);

    // 获取最近的探针
    std::shared_ptr<LightProbe> getNearestProbe(const Vec3& position) const;

    // 更新所有探针
    void updateAll(VkCommandBuffer commandBuffer);

private:
    LightProbeManager() = default;

    std::vector<std::shared_ptr<LightProbe>> m_probes;
};

} // namespace spark
