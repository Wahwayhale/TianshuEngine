#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Texture;
class ShadowMap;

// =============================================
// 光源数量限制
// =============================================
constexpr int MAX_DIRECTIONAL_LIGHTS = 4;
constexpr int MAX_POINT_LIGHTS = 16;
constexpr int MAX_SPOT_LIGHTS = 8;

// =============================================
// 光源结构体
// =============================================

struct DirectionalLightData {
    Vec4 directionAndIntensity;
    Vec4 color;
};

struct PointLightData {
    Vec4 positionAndRange;
    Vec4 colorAndIntensity;
};

struct SpotLightData {
    Vec4 positionAndRange;
    Vec4 directionAndIntensity;
    Vec4 colorAndCutoff;
    Vec4 outerCutoff;
};

// =============================================
// UBO 结构体定义
// =============================================

struct SceneUBO {
    Mat4 view;
    Mat4 projection;
    Vec3 cameraPos;
    float padding0 = 0.0f;
    Vec3 ambientColor;
    float padding1 = 0.0f;
};

struct LightUBO {
    DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLightData pointLights[MAX_POINT_LIGHTS];
    SpotLightData spotLights[MAX_SPOT_LIGHTS];
    int directionalLightCount = 0;
    int pointLightCount = 0;
    int spotLightCount = 0;
    float padding = 0.0f;
};

struct MaterialUBO {
    Vec4 albedo = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    float emission = 0.0f;
};

struct ShadowUBO {
    Mat4 lightSpaceMatrix;
    float shadowBias = 0.005f;
    float shadowStrength = 0.8f;
    Vec2 padding;
};

struct PushConstants {
    Mat4 model;
};

// =============================================
// Uniform Buffer 类
// =============================================

class Buffer;

class UniformBuffer {
public:
    UniformBuffer(Device& device, VkDeviceSize size, uint32_t frameCount = 2);
    ~UniformBuffer();

    void update(uint32_t frameIndex, const void* data, VkDeviceSize size);
    VkBuffer getBuffer(uint32_t frameIndex) const;
    uint32_t getFrameCount() const { return static_cast<uint32_t>(m_buffers.size()); }

private:
    std::vector<std::unique_ptr<Buffer>> m_buffers;
    VkDeviceSize m_size;
};

// =============================================
// 描述符集布局封装
// =============================================

class DescriptorSetLayout {
public:
    DescriptorSetLayout(Device& device);
    ~DescriptorSetLayout();

    VkDescriptorSetLayout getLayout() const { return m_layout; }

private:
    Device& m_device;
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
};

// =============================================
// 描述符池封装
// =============================================

class DescriptorPool {
public:
    DescriptorPool(Device& device, uint32_t maxSets = 2);
    ~DescriptorPool();

    VkDescriptorPool getPool() const { return m_pool; }

private:
    Device& m_device;
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
};

// =============================================
// 描述符集封装
// =============================================

class DescriptorSets {
public:
    DescriptorSets(Device& device,
                   VkDescriptorSetLayout layout,
                   VkDescriptorPool pool,
                   uint32_t count);
    ~DescriptorSets();

    void bindSceneUBO(uint32_t frameIndex, UniformBuffer& ubo);
    void bindLightUBO(uint32_t frameIndex, UniformBuffer& ubo);
    void bindMaterialUBO(uint32_t frameIndex, UniformBuffer& ubo);
    void bindShadowUBO(uint32_t frameIndex, UniformBuffer& ubo);
    void bindTexture(uint32_t frameIndex, Texture& texture);
    void bindShadowMap(uint32_t frameIndex, ShadowMap& shadowMap);

    VkDescriptorSet getSet(uint32_t frameIndex) const;
    uint32_t getCount() const { return static_cast<uint32_t>(m_sets.size()); }

private:
    Device& m_device;
    std::vector<VkDescriptorSet> m_sets;
};

} // namespace spark
