#include "vulkan/fwd.h"
#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {


class Buffer;

// 光线追踪设置
struct RayTracingSettings {
    bool enabled = true;
    bool reflectionsEnabled = true;
    bool shadowsEnabled = true;
    int maxBounces = 2;
    int samplesPerPixel = 1;
    float reflectionIntensity = 1.0f;
    float shadowIntensity = 0.8f;
};

// 光线追踪加速结构
struct AccelerationStructure {
    VkAccelerationStructureKHR handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkBuffer buffer = VK_NULL_HANDLE;
};

// 光线追踪系统
class RayTracingSystem {
public:
    RayTracingSystem(Device& device, uint32_t width, uint32_t height);
    ~RayTracingSystem();

    void recreate(uint32_t width, uint32_t height);

    // 渲染
    void render(VkCommandBuffer commandBuffer, uint32_t frameIndex);

    // 设置
    void setSettings(const RayTracingSettings& settings) { m_settings = settings; }
    RayTracingSettings& getSettings() { return m_settings; }

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

private:
    void createResources();
    void cleanupResources();
    void createAccelerationStructures();
    void createPipelines();
    void createShaderBindingTable();

    Device& m_device;
    uint32_t m_width, m_height;

    // 输出纹理
    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    // 加速结构
    AccelerationStructure m_blas;  // 底层加速结构
    AccelerationStructure m_tlas;  // 顶层加速结构

    // 管线
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    // 着色器绑定表
    VkBuffer m_shaderBindingTable = VK_NULL_HANDLE;
    VkDeviceMemory m_sbtMemory = VK_NULL_HANDLE;

    // 设置
    RayTracingSettings m_settings;
};

} // namespace spark
