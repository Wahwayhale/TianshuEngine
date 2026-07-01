#include "vulkan/fwd.h"
#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {


class Buffer;

// 体积雾设置
struct VolumetricFogSettings {
    bool enabled = true;
    float density = 0.02f;
    float heightFalloff = 0.1f;
    float height = 0.0f;
    Vec3 color = Vec3(0.7f, 0.8f, 0.9f);
    float maxDistance = 100.0f;
    int sampleCount = 64;
};

// 体积光设置
struct VolumetricLightSettings {
    bool enabled = true;
    float intensity = 0.5f;
    float maxDistance = 50.0f;
    int sampleCount = 32;
    float scattering = 0.5f;
};

// 云渲染设置
struct CloudSettings {
    bool enabled = false;
    float coverage = 0.5f;
    float thickness = 1.0f;
    float height = 100.0f;
    float scale = 0.001f;
    Vec3 color = Vec3(1.0f);
    float absorption = 0.5f;
};

// 天空大气设置
struct SkyAtmosphereSettings {
    bool enabled = true;
    float sunIntensity = 10.0f;
    Vec3 sunDirection = glm::normalize(Vec3(0.5f, -0.5f, 0.3f));
    Vec3 sunColor = Vec3(1.0f, 0.95f, 0.9f);
    float rayleighScattering = 0.01f;
    float mieScattering = 0.01f;
    float mieG = 0.8f;
    float atmosphereHeight = 100.0f;
};

// 体积渲染系统
class VolumetricRenderer {
public:
    VolumetricRenderer(Device& device, uint32_t width, uint32_t height);
    ~VolumetricRenderer();

    void recreate(uint32_t width, uint32_t height);

    // 渲染
    void renderFog(VkCommandBuffer commandBuffer, VkImageView depthImageView, const Mat4& viewMatrix, const Mat4& projMatrix);
    void renderLight(VkCommandBuffer commandBuffer, const Vec3& lightPos, const Vec3& lightColor, float lightIntensity);
    void renderSky(VkCommandBuffer commandBuffer);

    // 设置
    void setFogSettings(const VolumetricFogSettings& settings) { m_fogSettings = settings; }
    void setLightSettings(const VolumetricLightSettings& settings) { m_lightSettings = settings; }
    void setCloudSettings(const CloudSettings& settings) { m_cloudSettings = settings; }
    void setSkySettings(const SkyAtmosphereSettings& settings) { m_skySettings = settings; }

    VolumetricFogSettings& getFogSettings() { return m_fogSettings; }
    VolumetricLightSettings& getLightSettings() { return m_lightSettings; }
    CloudSettings& getCloudSettings() { return m_cloudSettings; }
    SkyAtmosphereSettings& getSkySettings() { return m_skySettings; }

    // 获取输出
    VkImageView getFogOutputView() const { return m_fogOutputView; }
    VkImageView getLightOutputView() const { return m_lightOutputView; }
    VkImageView getSkyOutputView() const { return m_skyOutputView; }

private:
    void createResources();
    void cleanupResources();
    void createPipelines();

    Device& m_device;
    uint32_t m_width, m_height;

    // 输出纹理
    VkImage m_fogOutputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_fogOutputMemory = VK_NULL_HANDLE;
    VkImageView m_fogOutputView = VK_NULL_HANDLE;

    VkImage m_lightOutputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_lightOutputMemory = VK_NULL_HANDLE;
    VkImageView m_lightOutputView = VK_NULL_HANDLE;

    VkImage m_skyOutputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_skyOutputMemory = VK_NULL_HANDLE;
    VkImageView m_skyOutputView = VK_NULL_HANDLE;

    // 管线
    VkPipeline m_fogPipeline = VK_NULL_HANDLE;
    VkPipeline m_lightPipeline = VK_NULL_HANDLE;
    VkPipeline m_skyPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    // 设置
    VolumetricFogSettings m_fogSettings;
    VolumetricLightSettings m_lightSettings;
    CloudSettings m_cloudSettings;
    SkyAtmosphereSettings m_skySettings;
};

} // namespace spark
