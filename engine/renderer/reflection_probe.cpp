#include "reflection_probe.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

ReflectionProbe::ReflectionProbe(Device& device, const Vec3& position, float radius)
    : m_device(device), m_position(position), m_radius(radius) {

    // 创建反射纹理
    uint32_t blackPixel = 0xFF000000;
    m_reflectionTexture = std::make_unique<Texture>(m_device, 1, 1, &blackPixel);

    SPARK_CORE_INFO("Reflection probe created at ({0}, {1}, {2})", position.x, position.y, position.z);
}

ReflectionProbe::~ReflectionProbe() = default;

void ReflectionProbe::capture(VkCommandBuffer commandBuffer) {
    // TODO: 捕获环境反射
    // 需要渲染立方体贴图
}

} // namespace spark
