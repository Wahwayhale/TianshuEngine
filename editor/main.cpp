#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "renderer/pipeline.h"
#include "renderer/shader.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/descriptor_set.h"
#include "renderer/texture.h"
#include "renderer/shadow_map.h"
#include "renderer/post_process.h"
#include "renderer/render_system.h"
#include "physics/physics_system.h"
#include "audio/audio_system.h"
#include "script/script_system.h"
#include "scene/scene.h"
#include "ecs/components.h"

// 编辑器核心
#include "core/editor_state.h"
#include "core/command_system.h"

// 编辑器面板
#include "panels/viewport_panel.h"
#include "panels/inspector_panel.h"
#include "panels/scene_hierarchy_panel.h"
#include "panels/asset_browser_panel.h"
#include "panels/toolbar_panel.h"
#include "panels/status_bar.h"
#include "panels/console_panel.h"
#include "panels/settings_panel.h"

#include <memory>

using namespace spark;

class EditorApp : public Application {
public:
    EditorApp() : Application("Spark Editor", 1920, 1080) {
        // Initialize renderer
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        // 后处理系统
        m_postProcess = std::make_unique<PostProcess>(
            m_renderer->getDevice(),
            m_renderer->getSwapchainFormat(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        // 阴影系统
        m_shadowMap = std::make_unique<ShadowMap>(m_renderer->getDevice(), 2048, 2048);

        // 管线布局
        m_descriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_renderer->getDevice());

        // 主管线
        auto vertShader = std::make_shared<Shader>(
            m_renderer->getDevice(), "shaders/pbr_vert.spv", VK_SHADER_STAGE_VERTEX_BIT
        );
        auto fragShader = std::make_shared<Shader>(
            m_renderer->getDevice(), "shaders/pbr_frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT
        );
        std::vector<std::shared_ptr<Shader>> shaders = {vertShader, fragShader};
        m_pipeline = std::make_unique<Pipeline>(
            m_renderer->getDevice(),
            m_postProcess->getRenderPass(),
            shaders,
            m_descriptorSetLayout->getLayout(),
            true
        );

        // 相机
        m_camera = std::make_unique<Camera>(Vec3(0.0f, 5.0f, 12.0f));

        // UBOs
        m_sceneUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(SceneUBO), 2);
        m_lightUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(LightUBO), 2);
        m_materialUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(MaterialUBO), 2);
        m_shadowUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(ShadowUBO), 2);

        // 默认纹理
        uint32_t whitePixel = 0xFFFFFFFF;
        m_defaultTexture = std::make_unique<Texture>(m_renderer->getDevice(), 1, 1, &whitePixel);

        // 描述符集
        m_descriptorPool = std::make_unique<DescriptorPool>(m_renderer->getDevice(), 2);
        m_descriptorSets = std::make_unique<DescriptorSets>(
            m_renderer->getDevice(),
            m_descriptorSetLayout->getLayout(),
            m_descriptorPool->getPool(),
            2
        );

        for (uint32_t i = 0; i < 2; i++) {
            m_descriptorSets->bindSceneUBO(i, *m_sceneUBO);
            m_descriptorSets->bindLightUBO(i, *m_lightUBO);
            m_descriptorSets->bindMaterialUBO(i, *m_materialUBO);
            m_descriptorSets->bindShadowUBO(i, *m_shadowUBO);
            m_descriptorSets->bindTexture(i, *m_defaultTexture);
            m_descriptorSets->bindShadowMap(i, *m_shadowMap);
        }

        // 创建场景
        m_scene = std::make_unique<Scene>();

        // 创建默认实体
        auto cubeMesh = createCubeMesh();
        auto groundMesh = createGroundMesh();

        auto& ground = m_scene->createEntity("Ground");
        auto& groundTransform = ground.addComponent<TransformComponent>();
        groundTransform.scale = Vec3(30.0f, 0.1f, 30.0f);
        auto& groundRenderer = ground.addComponent<MeshRendererComponent>();
        groundRenderer.mesh = groundMesh;
        groundRenderer.albedo = Vec4(0.3f, 0.3f, 0.35f, 1.0f);
        groundRenderer.metallic = 0.0f;
        groundRenderer.roughness = 0.9f;

        auto& cube = m_scene->createEntity("Cube");
        cube.addComponent<TransformComponent>(Vec3(0.0f, 1.0f, 0.0f));
        auto& cubeRenderer = cube.addComponent<MeshRendererComponent>();
        cubeRenderer.mesh = cubeMesh;
        cubeRenderer.albedo = Vec4(0.8f, 0.2f, 0.2f, 1.0f);
        cubeRenderer.metallic = 0.3f;
        cubeRenderer.roughness = 0.4f;

        auto& sphere = m_scene->createEntity("Sphere");
        sphere.addComponent<TransformComponent>(Vec3(3.0f, 1.0f, 0.0f));
        auto& sphereRenderer = sphere.addComponent<MeshRendererComponent>();
        sphereRenderer.mesh = cubeMesh;  // 简化：使用立方体代替球体
        sphereRenderer.albedo = Vec4(0.2f, 0.5f, 0.8f, 1.0f);
        sphereRenderer.metallic = 0.8f;
        sphereRenderer.roughness = 0.2f;

        // 添加系统
        auto& renderSystem = m_scene->addSystem<RenderSystem>();
        renderSystem.setPipeline(m_pipeline.get());
        renderSystem.setDescriptorSets(m_descriptorSets.get());
        renderSystem.setSceneUBO(m_sceneUBO.get());
        renderSystem.setLightUBO(m_lightUBO.get());
        renderSystem.setMaterialUBO(m_materialUBO.get());
        renderSystem.setShadowUBO(m_shadowUBO.get());
        renderSystem.setCamera(m_camera.get());
        renderSystem.setLightDir(m_lightDir);
        renderSystem.setAmbientColor(Vec3(0.05f, 0.05f, 0.07f));
        m_renderSystem = &renderSystem;

        auto& physicsSystem = m_scene->addSystem<PhysicsSystem>();
        m_physicsSystem = &physicsSystem;

        // 光源设置
        m_lightData.directionalLightCount = 1;
        m_lightData.directionalLights[0].directionAndIntensity = Vec4(-0.5f, -1.0f, -0.3f, 2.5f);
        m_lightData.directionalLights[0].color = Vec4(1.0f, 0.95f, 0.9f, 1.0f);

        m_lightData.pointLightCount = 2;
        m_lightData.pointLights[0].positionAndRange = Vec4(5.0f, 4.0f, 5.0f, 15.0f);
        m_lightData.pointLights[0].colorAndIntensity = Vec4(1.0f, 0.7f, 0.4f, 80.0f);
        m_lightData.pointLights[1].positionAndRange = Vec4(-5.0f, 4.0f, -5.0f, 15.0f);
        m_lightData.pointLights[1].colorAndIntensity = Vec4(0.4f, 0.7f, 1.0f, 80.0f);

        m_lightData.spotLightCount = 1;
        m_lightData.spotLights[0].positionAndRange = Vec4(0.0f, 8.0f, 0.0f, 20.0f);
        m_lightData.spotLights[0].directionAndIntensity = Vec4(0.0f, -1.0f, 0.0f, 150.0f);
        m_lightData.spotLights[0].colorAndCutoff = Vec4(1.0f, 1.0f, 0.9f, 0.9f);
        m_lightData.spotLights[0].outerCutoff = Vec4(0.7f, 0.0f, 0.0f, 0.0f);

        m_lightDir = glm::normalize(Vec3(-0.5f, -1.0f, -0.3f));

        // 创建编辑器面板
        m_viewportPanel = std::make_unique<ViewportPanel>();
        m_inspectorPanel = std::make_unique<InspectorPanel>();
        m_sceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>();
        m_assetBrowserPanel = std::make_unique<AssetBrowserPanel>();
        m_toolbarPanel = std::make_unique<ToolbarPanel>();
        m_statusBar = std::make_unique<StatusBar>();
        m_consolePanel = std::make_unique<ConsolePanel>();
        m_settingsPanel = std::make_unique<SettingsPanel>();

        // 设置回调
        m_sceneHierarchyPanel->setEntitySelectedCallback([this](Entity* entity) {
            if (entity) {
                EditorState::get().selectEntity(entity->getID());
                m_inspectorPanel->setEntity(entity);
                if (entity->hasComponent<TagComponent>()) {
                    m_statusBar->setSelectedEntity(entity->getComponent<TagComponent>().name);
                }
            } else {
                EditorState::get().deselectEntity();
                m_inspectorPanel->setEntity(nullptr);
                m_statusBar->setSelectedEntity("");
            }
        });

        m_toolbarPanel->setPlayCallback([this]() {
            EditorState::get().play();
            m_consolePanel->addInfo("Scene playing...");
        });

        m_toolbarPanel->setPauseCallback([this]() {
            EditorState::get().pause();
            m_consolePanel->addInfo("Scene paused");
        });

        m_toolbarPanel->setStopCallback([this]() {
            EditorState::get().stop();
            m_consolePanel->addInfo("Scene stopped");
        });

        SPARK_INFO("Spark Editor started!");
        m_consolePanel->addInfo("Spark Editor v0.22.0 initialized");
    }

    ~EditorApp() {
        m_renderer->waitIdle();
        m_settingsPanel.reset();
        m_consolePanel.reset();
        m_statusBar.reset();
        m_toolbarPanel.reset();
        m_assetBrowserPanel.reset();
        m_sceneHierarchyPanel.reset();
        m_inspectorPanel.reset();
        m_viewportPanel.reset();
        m_scene.reset();
        m_postProcess.reset();
        m_defaultTexture.reset();
        m_descriptorSets.reset();
        m_descriptorPool.reset();
        m_sceneUBO.reset();
        m_lightUBO.reset();
        m_materialUBO.reset();
        m_shadowUBO.reset();
        m_pipeline.reset();
        m_descriptorSetLayout.reset();
        m_shadowMap.reset();
        m_renderer.reset();
    }

protected:
    void onUpdate(float deltaTime) override {
        // 更新相机（编辑模式下）
        if (EditorState::get().isEditing()) {
            m_camera->update(deltaTime);
        }

        // 更新场景
        m_scene->update(deltaTime);

        // 更新状态栏
        float fps = 1.0f / deltaTime;
        int entityCount = static_cast<int>(m_scene->getEntityCount());
        m_statusBar->updateStats(fps, entityCount, 0);
    }

    void onRender() override {
        uint32_t currentFrame = m_renderer->getCurrentFrame();
        VkCommandBuffer commandBuffer;

        // 第一遍：阴影
        {
            vkWaitForFences(m_renderer->getDevice().getDevice(), 1,
                           &m_renderer->getCurrentFence(), VK_TRUE, UINT64_MAX);

            Mat4 lightSpaceMatrix = m_shadowMap->getLightSpaceMatrix(m_lightDir);

            ShadowUBO shadowUBO{};
            shadowUBO.lightSpaceMatrix = lightSpaceMatrix;
            shadowUBO.shadowBias = 0.005f;
            shadowUBO.shadowStrength = 0.8f;
            m_shadowUBO->update(currentFrame, &shadowUBO, sizeof(ShadowUBO));

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            commandBuffer = m_renderer->getCurrentCommandBuffer();
            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VkRenderPassBeginInfo shadowPassInfo{};
            shadowPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            shadowPassInfo.renderPass = m_shadowMap->getRenderPass();
            shadowPassInfo.framebuffer = m_shadowMap->getFramebuffer();
            shadowPassInfo.renderArea.extent = {m_shadowMap->getWidth(), m_shadowMap->getHeight()};

            VkClearValue shadowClear{};
            shadowClear.depthStencil = {1.0f, 0};
            shadowPassInfo.clearValueCount = 1;
            shadowPassInfo.pClearValues = &shadowClear;

            vkCmdBeginRenderPass(commandBuffer, &shadowPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport shadowViewport{};
            shadowViewport.width = static_cast<float>(m_shadowMap->getWidth());
            shadowViewport.height = static_cast<float>(m_shadowMap->getHeight());
            shadowViewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &shadowViewport);

            VkRect2D shadowScissor{};
            shadowScissor.extent = {m_shadowMap->getWidth(), m_shadowMap->getHeight()};
            vkCmdSetScissor(commandBuffer, 0, 1, &shadowScissor);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPipeline->getPipeline());

            VkDescriptorSet shadowDescSet = m_descriptorSets->getSet(currentFrame);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_shadowPipeline->getLayout(), 0, 1, &shadowDescSet, 0, nullptr);

            m_scene->view<TransformComponent, MeshRendererComponent>([&](Entity& entity) {
                auto& transform = entity.getComponent<TransformComponent>();
                auto& meshRenderer = entity.getComponent<MeshRendererComponent>();

                if (!meshRenderer.visible || !meshRenderer.mesh || !meshRenderer.castShadow) return;

                PushConstants pushConstants{};
                pushConstants.model = transform.getTransformMatrix();
                vkCmdPushConstants(commandBuffer, m_shadowPipeline->getLayout(),
                                  VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);
                meshRenderer.mesh->draw(commandBuffer);
            });

            vkCmdEndRenderPass(commandBuffer);
            vkEndCommandBuffer(commandBuffer);
        }

        // 第二遍：主渲染
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            commandBuffer = m_renderer->getCurrentCommandBuffer();
            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VkRenderPassBeginInfo mainPassInfo{};
            mainPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            mainPassInfo.renderPass = m_postProcess->getRenderPass();
            mainPassInfo.framebuffer = m_postProcess->getFramebuffer();
            mainPassInfo.renderArea.extent = m_postProcess->getExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {{0.02f, 0.02f, 0.04f, 1.0f}};
            clearValues[1].depthStencil = {1.0f, 0};
            mainPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            mainPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffer, &mainPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            m_renderSystem->setCommandBuffer(commandBuffer);
            m_renderSystem->setCurrentFrame(currentFrame);
            m_renderSystem->setLightData(m_lightData);
            m_renderSystem->setLightDir(m_lightDir);
            m_renderSystem->setViewportSize(
                static_cast<float>(m_postProcess->getExtent().width),
                static_cast<float>(m_postProcess->getExtent().height)
            );

            m_renderSystem->render(*m_scene);

            vkCmdEndRenderPass(commandBuffer);
            vkEndCommandBuffer(commandBuffer);
        }

        // 第三遍：后处理 + UI
        m_renderer->beginFrame();
        commandBuffer = m_renderer->getCurrentCommandBuffer();

        VkViewport viewport{};
        viewport.width = static_cast<float>(getWindow().getWidth());
        viewport.height = static_cast<float>(getWindow().getHeight());
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.extent = {static_cast<uint32_t>(getWindow().getWidth()),
                         static_cast<uint32_t>(getWindow().getHeight())};
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        m_postProcess->draw(commandBuffer, currentFrame);

        // 渲染 UI
        m_uiSystem->beginFrame();

        // 工具栏
        m_toolbarPanel->render();

        // 主要面板
        m_viewportPanel->render();
        m_sceneHierarchyPanel->render(*m_scene);
        m_inspectorPanel->render();
        m_assetBrowserPanel->render();
        m_consolePanel->render();
        m_settingsPanel->render();

        // 状态栏
        m_statusBar->render();

        m_uiSystem->endFrame(commandBuffer);

        m_renderer->endFrame();
    }

    void onEvent(Event& event) override {
        if (auto* resizeEvent = dynamic_cast<WindowResizeEvent*>(&event)) {
            m_renderer->onResize(resizeEvent->width, resizeEvent->height);
            m_postProcess->recreate(resizeEvent->width, resizeEvent->height);
        }

        if (auto* mouseMoveEvent = dynamic_cast<MouseMoveEvent*>(&event)) {
            static double lastX = 0, lastY = 0;
            float xOffset = static_cast<float>(mouseMoveEvent->x - lastX);
            float yOffset = static_cast<float>(lastY - mouseMoveEvent->y);
            lastX = mouseMoveEvent->x;
            lastY = mouseMoveEvent->y;

            if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
                m_camera->processMouse(xOffset, yOffset);
            }
        }

        if (auto* scrollEvent = dynamic_cast<MouseScrollEvent*>(&event)) {
            m_camera->processScroll(static_cast<float>(scrollEvent->yOffset));
        }

        // 快捷键
        if (auto* keyEvent = dynamic_cast<KeyEvent*>(&event)) {
            if (keyEvent->action == GLFW_PRESS) {
                // Ctrl+Z: 撤销
                if (keyEvent->key == GLFW_KEY_Z && Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
                    CommandManager::get().undo();
                }
                // Ctrl+Y: 重做
                if (keyEvent->key == GLFW_KEY_Y && Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
                    CommandManager::get().redo();
                }
                // F5: Play/Stop
                if (keyEvent->key == GLFW_KEY_F5) {
                    if (EditorState::get().isPlaying()) {
                        EditorState::get().stop();
                    } else {
                        EditorState::get().play();
                    }
                }
                // W/E/R: Gizmo 模式
                if (keyEvent->key == GLFW_KEY_W) {
                    EditorState::get().setGizmoMode(EditorState::GizmoMode::Translate);
                }
                if (keyEvent->key == GLFW_KEY_E) {
                    EditorState::get().setGizmoMode(EditorState::GizmoMode::Rotate);
                }
                if (keyEvent->key == GLFW_KEY_R) {
                    EditorState::get().setGizmoMode(EditorState::GizmoMode::Scale);
                }
            }
        }
    }

private:
    std::shared_ptr<Mesh> createCubeMesh() {
        std::vector<PBRVertex> vertices = {
            {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        };
        std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0, 4, 6, 5, 6, 4, 7,
            8, 9, 10, 10, 11, 8, 12, 14, 13, 14, 12, 15,
            16, 17, 18, 18, 19, 16, 20, 22, 21, 22, 20, 23
        };
        return std::make_shared<Mesh>(m_renderer->getDevice(), vertices, indices);
    }

    std::shared_ptr<Mesh> createGroundMesh() {
        std::vector<PBRVertex> vertices = {
            {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        };
        std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
        return std::make_shared<Mesh>(m_renderer->getDevice(), vertices, indices);
    }

    // 渲染器
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<PostProcess> m_postProcess;
    std::unique_ptr<ShadowMap> m_shadowMap;
    std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout;
    std::unique_ptr<Pipeline> m_shadowPipeline;
    std::unique_ptr<Pipeline> m_pipeline;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<UniformBuffer> m_sceneUBO;
    std::unique_ptr<UniformBuffer> m_lightUBO;
    std::unique_ptr<UniformBuffer> m_materialUBO;
    std::unique_ptr<UniformBuffer> m_shadowUBO;
    std::unique_ptr<Texture> m_defaultTexture;
    std::unique_ptr<DescriptorPool> m_descriptorPool;
    std::unique_ptr<DescriptorSets> m_descriptorSets;

    // 场景
    std::unique_ptr<Scene> m_scene;
    RenderSystem* m_renderSystem = nullptr;
    PhysicsSystem* m_physicsSystem = nullptr;

    // UI 系统
    std::unique_ptr<UISystem> m_uiSystem;

    // 编辑器面板
    std::unique_ptr<ViewportPanel> m_viewportPanel;
    std::unique_ptr<InspectorPanel> m_inspectorPanel;
    std::unique_ptr<SceneHierarchyPanel> m_sceneHierarchyPanel;
    std::unique_ptr<AssetBrowserPanel> m_assetBrowserPanel;
    std::unique_ptr<ToolbarPanel> m_toolbarPanel;
    std::unique_ptr<StatusBar> m_statusBar;
    std::unique_ptr<ConsolePanel> m_consolePanel;
    std::unique_ptr<SettingsPanel> m_settingsPanel;

    // 光源
    LightUBO m_lightData;
    Vec3 m_lightDir;
};

int main() {
    try {
        EditorApp editor;
        editor.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Editor error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
