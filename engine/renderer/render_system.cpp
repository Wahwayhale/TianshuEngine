#include "render_system.h"
#include "pipeline.h"
#include "camera.h"
#include "mesh.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "core/log.h"

namespace spark {

RenderSystem::RenderSystem() {
    SPARK_CORE_INFO("Render system created.");
}

RenderSystem::~RenderSystem() = default;

void RenderSystem::update(Scene& scene, float deltaTime) {
    // 渲染系统不需要每帧更新逻辑
}

void RenderSystem::render(Scene& scene) {
    if (!m_pipeline || !m_commandBuffer || !m_descriptorSets || !m_camera) {
        return;
    }

    // 更新 Scene UBO
    SceneUBO sceneUBO{};
    sceneUBO.view = m_camera->getViewMatrix();
    float aspectRatio = m_viewportWidth / m_viewportHeight;
    sceneUBO.projection = m_camera->getProjectionMatrix(aspectRatio);
    sceneUBO.projection[1][1] *= -1.0f;
    sceneUBO.cameraPos = m_camera->getPosition();
    sceneUBO.ambientColor = m_ambientColor;
    m_sceneUBO->update(m_currentFrame, &sceneUBO, sizeof(SceneUBO));

    // 更新 Light UBO
    m_lightUBO->update(m_currentFrame, &m_lightData, sizeof(LightUBO));

    // 绑定管线
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipeline());

    // 设置视口和裁剪
    VkViewport viewport{};
    viewport.width = m_viewportWidth;
    viewport.height = m_viewportHeight;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = {static_cast<uint32_t>(m_viewportWidth), static_cast<uint32_t>(m_viewportHeight)};
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

    // 遍历所有有 TransformComponent + MeshRendererComponent 的实体
    scene.view<TransformComponent, MeshRendererComponent>([&](Entity& entity) {
        auto& transform = entity.getComponent<TransformComponent>();
        auto& meshRenderer = entity.getComponent<MeshRendererComponent>();

        if (!meshRenderer.visible || !meshRenderer.mesh) {
            return;
        }

        // 更新 Material UBO
        MaterialUBO materialUBO{};
        materialUBO.albedo = meshRenderer.albedo;
        materialUBO.metallic = meshRenderer.metallic;
        materialUBO.roughness = meshRenderer.roughness;
        materialUBO.ao = meshRenderer.ao;
        materialUBO.emission = meshRenderer.emission;
        m_materialUBO->update(m_currentFrame, &materialUBO, sizeof(MaterialUBO));

        // 绑定描述符集
        VkDescriptorSet descSet = m_descriptorSets->getSet(m_currentFrame);
        vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipeline->getLayout(), 0, 1, &descSet, 0, nullptr);

        // Push model matrix
        PushConstants pushConstants{};
        pushConstants.model = transform.getTransformMatrix();
        vkCmdPushConstants(m_commandBuffer, m_pipeline->getLayout(),
                          VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);

        // 绘制网格
        meshRenderer.mesh->draw(m_commandBuffer);
    });
}

} // namespace spark
