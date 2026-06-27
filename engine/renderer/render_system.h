#pragma once

#include "ecs/system.h"
#include "descriptor_set.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace spark {

class Pipeline;
class Camera;
class ShadowMap;

// 渲染系统 - 从 ECS 实体驱动渲染
class RenderSystem : public System {
public:
    RenderSystem();
    ~RenderSystem() override;

    void update(Scene& scene, float deltaTime) override;
    void render(Scene& scene) override;

    // 设置渲染资源
    void setPipeline(Pipeline* pipeline) { m_pipeline = pipeline; }
    void setCommandBuffer(VkCommandBuffer cmd) { m_commandBuffer = cmd; }
    void setDescriptorSets(DescriptorSets* descSets) { m_descriptorSets = descSets; }
    void setSceneUBO(UniformBuffer* ubo) { m_sceneUBO = ubo; }
    void setLightUBO(UniformBuffer* ubo) { m_lightUBO = ubo; }
    void setMaterialUBO(UniformBuffer* ubo) { m_materialUBO = ubo; }
    void setShadowUBO(UniformBuffer* ubo) { m_shadowUBO = ubo; }
    void setCamera(Camera* camera) { m_camera = camera; }
    void setLightData(const LightUBO& lightData) { m_lightData = lightData; }
    void setLightDir(const Vec3& dir) { m_lightDir = dir; }
    void setAmbientColor(const Vec3& color) { m_ambientColor = color; }
    void setCurrentFrame(uint32_t frame) { m_currentFrame = frame; }
    void setViewportSize(float w, float h) { m_viewportWidth = w; m_viewportHeight = h; }

private:
    Pipeline* m_pipeline = nullptr;
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    DescriptorSets* m_descriptorSets = nullptr;
    UniformBuffer* m_sceneUBO = nullptr;
    UniformBuffer* m_lightUBO = nullptr;
    UniformBuffer* m_materialUBO = nullptr;
    UniformBuffer* m_shadowUBO = nullptr;
    Camera* m_camera = nullptr;

    LightUBO m_lightData;
    Vec3 m_lightDir;
    Vec3 m_ambientColor = Vec3(0.03f);
    uint32_t m_currentFrame = 0;
    float m_viewportWidth = 1280.0f;
    float m_viewportHeight = 720.0f;
};

} // namespace spark
