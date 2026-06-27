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
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

class SparkApp : public Application {
public:
    SparkApp() : Application("Spark Engine", 1280, 720) {
        // Initialize renderer
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        // ========== 后处理系统 ==========
        m_postProcess = std::make_unique<PostProcess>(
            m_renderer->getDevice(),
            m_renderer->getSwapchainFormat(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        // ========== 阴影系统 ==========
        m_shadowMap = std::make_unique<ShadowMap>(m_renderer->getDevice(), 2048, 2048);

        // 描述符集布局
        m_shadowDescriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_renderer->getDevice());
        m_descriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_renderer->getDevice());

        // 阴影管线
        auto shadowVertShader = std::make_shared<Shader>(
            m_renderer->getDevice(), "shaders/shadow_vert.spv", VK_SHADER_STAGE_VERTEX_BIT
        );
        std::vector<std::shared_ptr<Shader>> shadowShaders = {shadowVertShader};
        m_shadowPipeline = std::make_unique<Pipeline>(
            m_renderer->getDevice(),
            m_shadowMap->getRenderPass(),
            shadowShaders,
            m_shadowDescriptorSetLayout->getLayout(),
            true
        );

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

        // ========== Uniform Buffers ==========
        m_sceneUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(SceneUBO), 2);
        m_lightUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(LightUBO), 2);
        m_materialUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(MaterialUBO), 2);
        m_shadowUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(ShadowUBO), 2);

        // 默认纹理
        uint32_t whitePixel = 0xFFFFFFFF;
        m_defaultTexture = std::make_unique<Texture>(m_renderer->getDevice(), 1, 1, &whitePixel);

        // ========== 描述符集 ==========
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

        // ========== 创建场景 ==========
        m_scene = std::make_unique<Scene>();

        // 创建网格
        auto cubeMesh = createCubeMesh();
        auto sphereMesh = createSphereMesh(16, 16);
        auto groundMesh = createGroundMesh();

        // ========== 创建物理实体 ==========

        // 地面（静态）
        auto& ground = m_scene->createEntity("Ground");
        auto& groundTransform = ground.addComponent<TransformComponent>();
        groundTransform.position = Vec3(0.0f, -0.05f, 0.0f);
        groundTransform.scale = Vec3(30.0f, 0.1f, 30.0f);
        auto& groundRenderer = ground.addComponent<MeshRendererComponent>();
        groundRenderer.mesh = groundMesh;
        groundRenderer.albedo = Vec4(0.3f, 0.3f, 0.35f, 1.0f);
        groundRenderer.metallic = 0.0f;
        groundRenderer.roughness = 0.9f;
        auto& groundCollider = ground.addComponent<ColliderComponent>();
        groundCollider.colliderType = ColliderComponent::ColliderType::Box;
        groundCollider.size = Vec3(15.0f, 0.05f, 15.0f);
        auto& groundRb = ground.addComponent<RigidBodyComponent>();
        groundRb.isStatic = true;

        // 堆叠的立方体（动态）
        for (int i = 0; i < 5; i++) {
            auto& cube = m_scene->createEntity("Cube " + std::to_string(i));
            auto& transform = cube.addComponent<TransformComponent>();
            transform.position = Vec3(-2.0f + i * 0.05f, 0.5f + i * 1.1f, 0.0f);
            transform.scale = Vec3(1.0f);

            auto& renderer = cube.addComponent<MeshRendererComponent>();
            renderer.mesh = cubeMesh;
            renderer.albedo = Vec4(
                0.2f + (i * 0.15f),
                0.5f - (i * 0.08f),
                0.8f - (i * 0.1f),
                1.0f
            );
            renderer.metallic = 0.3f;
            renderer.roughness = 0.4f;

            auto& collider = cube.addComponent<ColliderComponent>();
            collider.colliderType = ColliderComponent::ColliderType::Box;
            collider.size = Vec3(0.5f);

            auto& rb = cube.addComponent<RigidBodyComponent>();
            rb.mass = 1.0f;
            rb.restitution = 0.3f;
            rb.friction = 0.5f;
        }

        // 弹跳球（动态）
        for (int i = 0; i < 3; i++) {
            auto& ball = m_scene->createEntity("Ball " + std::to_string(i));
            auto& transform = ball.addComponent<TransformComponent>();
            transform.position = Vec3(3.0f + i * 2.0f, 5.0f + i * 2.0f, 0.0f);

            auto& renderer = ball.addComponent<MeshRendererComponent>();
            renderer.mesh = sphereMesh;
            renderer.albedo = Vec4(0.9f, 0.3f, 0.2f + i * 0.2f, 1.0f);
            renderer.metallic = 0.8f;
            renderer.roughness = 0.2f;

            auto& collider = ball.addComponent<ColliderComponent>();
            collider.colliderType = ColliderComponent::ColliderType::Sphere;
            collider.radius = 0.5f;

            auto& rb = ball.addComponent<RigidBodyComponent>();
            rb.mass = 2.0f;
            rb.restitution = 0.6f;  // 高弹性
            rb.friction = 0.3f;
        }

        // 金属球（动态）
        auto& metalBall = m_scene->createEntity("Metal Ball");
        auto& metalTransform = metalBall.addComponent<TransformComponent>();
        metalTransform.position = Vec3(0.0f, 8.0f, 3.0f);

        auto& metalRenderer = metalBall.addComponent<MeshRendererComponent>();
        metalRenderer.mesh = sphereMesh;
        metalRenderer.albedo = Vec4(1.0f, 0.84f, 0.0f, 1.0f);
        metalRenderer.metallic = 1.0f;
        metalRenderer.roughness = 0.1f;

        auto& metalCollider = metalBall.addComponent<ColliderComponent>();
        metalCollider.colliderType = ColliderComponent::ColliderType::Sphere;
        metalCollider.radius = 0.75f;

        auto& metalRb = metalBall.addComponent<RigidBodyComponent>();
        metalRb.mass = 5.0f;
        metalRb.restitution = 0.4f;

        // ========== 添加系统 ==========
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
        physicsSystem.setGravity(Vec3(0.0f, -9.81f, 0.0f));
        m_physicsSystem = &physicsSystem;

        // ========== 光源设置 ==========
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

        SPARK_INFO("Physics Demo started!");
        SPARK_INFO("Controls: WASD - Move, Mouse Right Button + Drag - Look, Space/Shift - Up/Down");
    }

    ~SparkApp() {
        m_renderer->waitIdle();
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
        m_shadowPipeline.reset();
        m_descriptorSetLayout.reset();
        m_shadowDescriptorSetLayout.reset();
        m_shadowMap.reset();
        m_renderer.reset();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_camera->update(deltaTime);
        m_scene->update(deltaTime);
    }

    void onRender() override {
        uint32_t currentFrame = m_renderer->getCurrentFrame();
        VkCommandBuffer commandBuffer;

        // ========== 第一遍：阴影渲染通道 ==========
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

        // ========== 第二遍：主渲染通道 ==========
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

        // ========== 第三遍：后处理通道 ==========
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

    std::shared_ptr<Mesh> createSphereMesh(int rings, int sectors) {
        std::vector<PBRVertex> vertices;
        std::vector<uint32_t> indices;

        float radius = 0.5f;
        for (int r = 0; r <= rings; r++) {
            float phi = 3.14159f * r / rings;
            for (int s = 0; s <= sectors; s++) {
                float theta = 2.0f * 3.14159f * s / sectors;

                float x = std::sin(phi) * std::cos(theta);
                float y = std::cos(phi);
                float z = std::sin(phi) * std::sin(theta);

                PBRVertex vertex;
                vertex.position[0] = radius * x;
                vertex.position[1] = radius * y;
                vertex.position[2] = radius * z;
                vertex.normal[0] = x;
                vertex.normal[1] = y;
                vertex.normal[2] = z;
                vertex.texCoord[0] = (float)s / sectors;
                vertex.texCoord[1] = (float)r / rings;
                vertices.push_back(vertex);
            }
        }

        for (int r = 0; r < rings; r++) {
            for (int s = 0; s < sectors; s++) {
                int i1 = r * (sectors + 1) + s;
                int i2 = i1 + (sectors + 1);

                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i1 + 1);

                indices.push_back(i1 + 1);
                indices.push_back(i2);
                indices.push_back(i2 + 1);
            }
        }

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

    // 管线
    std::unique_ptr<DescriptorSetLayout> m_shadowDescriptorSetLayout;
    std::unique_ptr<Pipeline> m_shadowPipeline;
    std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout;
    std::unique_ptr<Pipeline> m_pipeline;

    // 相机
    std::unique_ptr<Camera> m_camera;

    // UBOs
    std::unique_ptr<UniformBuffer> m_sceneUBO;
    std::unique_ptr<UniformBuffer> m_lightUBO;
    std::unique_ptr<UniformBuffer> m_materialUBO;
    std::unique_ptr<UniformBuffer> m_shadowUBO;

    // 纹理和描述符
    std::unique_ptr<Texture> m_defaultTexture;
    std::unique_ptr<DescriptorPool> m_descriptorPool;
    std::unique_ptr<DescriptorSets> m_descriptorSets;

    // 场景和 ECS
    std::unique_ptr<Scene> m_scene;
    RenderSystem* m_renderSystem = nullptr;
    PhysicsSystem* m_physicsSystem = nullptr;

    // 光源
    LightUBO m_lightData;
    Vec3 m_lightDir;
};

int main() {
    try {
        SparkApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Fatal error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
