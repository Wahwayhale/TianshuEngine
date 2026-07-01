#pragma once

#include "math/math_types.h"
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Texture;

// 反射探针
class ReflectionProbe {
public:
    ReflectionProbe(Device& device, const Vec3& position, float radius = 10.0f);
    ~ReflectionProbe();

    // 捕获反射
    void capture(VkCommandBuffer commandBuffer);

    // 属性
    const Vec3& getPosition() const { return m_position; }
    void setPosition(const Vec3& position) { m_position = position; }

    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }

    // 获取反射纹理
    Texture* getReflectionTexture() const { return m_reflectionTexture.get(); }

private:
    Device& m_device;
    Vec3 m_position;
    float m_radius;

    std::unique_ptr<Texture> m_reflectionTexture;
};

} // namespace spark
