/**
 * 天枢引擎编辑器 - 极简高端风格
 *
 * 设计规范：
 * - 深色模式 (#161618 背景)
 * - 6-8px 统一圆角
 * - 扁平化设计 + 微透明边框
 * - 充足留白
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "renderer/camera.h"
#include "renderer/mesh.h"
#include "renderer/pipeline.h"
#include "renderer/shader.h"
#include "renderer/descriptor_set.h"
#include "ui/imgui_integration.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <imgui.h>
#include <memory>
#include <iostream>

using namespace spark;

enum class EditorMode { Edit, Play, Pause };

class EditorApp : public Application {
public:
    EditorApp() : Application("Tianshu Engine", 1920, 1080) {
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );
        m_camera = std::make_unique<Camera>(Vec3(0.0f, 5.0f, 10.0f));
        createMeshes();
        m_scene = std::make_unique<Scene>();
        createDefaultScene();
        setupPipeline();

        m_imgui = std::make_unique<ImGuiIntegration>();
        if (!m_imgui->initialize(getWindow().getNativeWindow(), m_renderer.get())) {
            SPARK_ERROR("ImGui init failed!");
            return;
        }
        setupProfessionalTheme();
        m_initialized = true;
        SPARK_INFO("Tianshu Engine started!");
    }

    ~EditorApp() {
        if (m_renderer) m_renderer->waitIdle();
        if (m_imgui) m_imgui->shutdown();
    }

protected:
    void onUpdate(float dt) override {
        if (!m_initialized) return;
        if (m_mode == EditorMode::Edit) m_camera->update(dt);
        m_rotation += dt * 45.0f;
        m_scene->update(dt);
    }

    void onRender() override {
        if (!m_initialized) return;
        m_renderer->beginFrame();
        VkCommandBuffer cmd = m_renderer->getCurrentCommandBuffer();
        drawScene(cmd);
        renderUI(cmd);
        m_renderer->endFrame();
    }

    void onEvent(Event& event) override {
        if (!m_initialized) return;
        if (auto* e = dynamic_cast<WindowResizeEvent*>(&event))
            m_renderer->onResize(e->width, e->height);
        if (auto* e = dynamic_cast<KeyEvent*>(&event)) {
            if (e->action == GLFW_PRESS) {
                if (e->key == GLFW_KEY_ESCAPE)
                    glfwSetWindowShouldClose(getWindow().getNativeWindow(), GLFW_TRUE);
                if (e->key == GLFW_KEY_F5)
                    m_mode = (m_mode == EditorMode::Edit) ? EditorMode::Play : EditorMode::Edit;
            }
        }
        if (m_mode == EditorMode::Edit) {
            if (auto* e = dynamic_cast<MouseMoveEvent*>(&event)) {
                static double lx = 0, ly = 0;
                if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
                    m_camera->processMouse(e->x - lx, ly - e->y);
                lx = e->x; ly = e->y;
            }
            if (auto* e = dynamic_cast<MouseScrollEvent*>(&event))
                m_camera->processScroll(e->yOffset);
        }
    }

private:
    // ========== 极简高端主题 ==========
    void setupProfessionalTheme() {
        ImGuiIO& io = ImGui::GetIO();

        // 加载中文字体
        const char* fonts[] = {"C:/Windows/Fonts/msyh.ttc", "C:/Windows/Fonts/simhei.ttf"};
        for (auto& f : fonts) {
            FILE* file = fopen(f, "rb");
            if (file) { fclose(file); io.Fonts->AddFontFromFileTTF(f, 14.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull()); break; }
        }

        ImGuiStyle& s = ImGui::GetStyle();

        // === 圆角：统一 6px ===
        s.WindowRounding = 6.0f;
        s.FrameRounding = 6.0f;
        s.GrabRounding = 6.0f;
        s.ScrollbarRounding = 6.0f;
        s.TabRounding = 6.0f;
        s.ChildRounding = 6.0f;

        // === 留白：增加呼吸感 ===
        s.WindowPadding = ImVec2(12, 12);
        s.FramePadding = ImVec2(10, 6);
        s.ItemSpacing = ImVec2(10, 8);
        s.ItemInnerSpacing = ImVec2(8, 6);
        s.ScrollbarSize = 10.0f;
        s.GrabMinSize = 10.0f;

        // === 扁平化：无边框 ===
        s.WindowBorderSize = 0.0f;
        s.ChildBorderSize = 0.0f;
        s.PopupBorderSize = 1.0f;
        s.FrameBorderSize = 0.0f;
        s.TabBorderSize = 0.0f;

        // === 极简深色配色 ===
        ImVec4* c = s.Colors;

        // 背景 #161618
        c[ImGuiCol_WindowBg]        = ImVec4(0.086f, 0.086f, 0.094f, 1.00f);
        c[ImGuiCol_ChildBg]         = ImVec4(0.078f, 0.078f, 0.086f, 1.00f);
        c[ImGuiCol_PopupBg]         = ImVec4(0.10f, 0.10f, 0.12f, 0.98f);

        // 边框：微透明
        c[ImGuiCol_Border]          = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
        c[ImGuiCol_BorderShadow]    = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        // 框架
        c[ImGuiCol_FrameBg]         = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        c[ImGuiCol_FrameBgHovered]  = ImVec4(0.14f, 0.14f, 0.18f, 1.00f);
        c[ImGuiCol_FrameBgActive]   = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);

        // 标题栏
        c[ImGuiCol_TitleBg]         = ImVec4(0.06f, 0.06f, 0.08f, 1.00f);
        c[ImGuiCol_TitleBgActive]   = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
        c[ImGuiCol_TitleBgCollapsed]= ImVec4(0.04f, 0.04f, 0.06f, 1.00f);

        // 菜单栏
        c[ImGuiCol_MenuBarBg]       = ImVec4(0.06f, 0.06f, 0.08f, 1.00f);

        // 滚动条
        c[ImGuiCol_ScrollbarBg]     = ImVec4(0.06f, 0.06f, 0.08f, 1.00f);
        c[ImGuiCol_ScrollbarGrab]   = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
        c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.28f, 0.34f, 1.00f);
        c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.35f, 0.35f, 0.42f, 1.00f);

        // 复选框 / 滑块
        c[ImGuiCol_CheckMark]       = ImVec4(0.30f, 0.70f, 1.00f, 1.00f);
        c[ImGuiCol_SliderGrab]      = ImVec4(0.30f, 0.70f, 1.00f, 1.00f);
        c[ImGuiCol_SliderGrabActive]= ImVec4(0.40f, 0.80f, 1.00f, 1.00f);

        // 按钮
        c[ImGuiCol_Button]          = ImVec4(0.14f, 0.14f, 0.18f, 1.00f);
        c[ImGuiCol_ButtonHovered]   = ImVec4(0.20f, 0.20f, 0.26f, 1.00f);
        c[ImGuiCol_ButtonActive]    = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);

        // 标题
        c[ImGuiCol_Header]          = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
        c[ImGuiCol_HeaderHovered]   = ImVec4(0.18f, 0.18f, 0.24f, 1.00f);
        c[ImGuiCol_HeaderActive]    = ImVec4(0.30f, 0.70f, 1.00f, 0.60f);

        // 分割线
        c[ImGuiCol_Separator]       = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
        c[ImGuiCol_SeparatorHovered]= ImVec4(0.30f, 0.70f, 1.00f, 0.40f);
        c[ImGuiCol_SeparatorActive] = ImVec4(0.30f, 0.70f, 1.00f, 0.80f);

        // 文本
        c[ImGuiCol_Text]            = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
        c[ImGuiCol_TextDisabled]    = ImVec4(0.42f, 0.42f, 0.50f, 1.00f);

        // 高亮
        c[ImGuiCol_TextSelectedBg]  = ImVec4(0.30f, 0.70f, 1.00f, 0.25f);
    }

    // ========== UI 渲染 ==========
    void renderUI(VkCommandBuffer cmd) {
        m_imgui->newFrame();

        renderMenuBar();
        renderViewport();
        renderHierarchy();
        renderInspector();
        renderConsole();
        renderStatusBar();

        m_imgui->render(cmd);
    }

    void renderMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            // Logo
            ImGui::TextColored(ImVec4(0.30f, 0.70f, 1.00f, 1.00f), " 天枢引擎 ");
            ImGui::Separator();

            if (ImGui::BeginMenu("文件")) {
                ImGui::MenuItem("新建场景", "Ctrl+N");
                ImGui::MenuItem("打开场景", "Ctrl+O");
                ImGui::MenuItem("保存场景", "Ctrl+S");
                ImGui::Separator();
                ImGui::MenuItem("导出游戏", "Ctrl+B");
                ImGui::Separator();
                if (ImGui::MenuItem("退出", "ESC"))
                    glfwSetWindowShouldClose(getWindow().getNativeWindow(), GLFW_TRUE);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("编辑")) {
                ImGui::MenuItem("撤销", "Ctrl+Z");
                ImGui::MenuItem("重做", "Ctrl+Y");
                ImGui::Separator();
                ImGui::MenuItem("全选", "Ctrl+A");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("游戏对象")) {
                if (ImGui::MenuItem("创建空物体")) m_scene->createEntity("新物体");
                ImGui::Separator();
                if (ImGui::MenuItem("立方体")) {
                    auto& e = m_scene->createEntity("立方体");
                    e.addComponent<TransformComponent>();
                    auto& r = e.addComponent<MeshRendererComponent>();
                    r.albedo = Vec4(0.8f, 0.2f, 0.2f, 1.0f);
                }
                if (ImGui::MenuItem("球体")) {
                    auto& e = m_scene->createEntity("球体");
                    e.addComponent<TransformComponent>();
                    auto& r = e.addComponent<MeshRendererComponent>();
                    r.albedo = Vec4(0.2f, 0.5f, 0.8f, 1.0f);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("帮助")) {
                ImGui::MenuItem("关于天枢引擎");
                ImGui::EndMenu();
            }

            // 右侧运行按钮
            ImGui::Separator();
            bool playing = (m_mode == EditorMode::Play);
            if (playing) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.15f, 0.15f, 0.9f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.2f, 0.2f, 1.0f));
            }
            if (ImGui::Button(playing ? "停止" : "运行", ImVec2(48, 20)))
                m_mode = playing ? EditorMode::Edit : EditorMode::Play;
            if (playing) ImGui::PopStyleColor(2);

            ImGui::EndMainMenuBar();
        }
    }

    void renderViewport() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("视口", nullptr, ImGuiWindowFlags_NoScrollbar);

        ImVec2 sz = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::Text("视口 %.0fx%.0f", sz.x, sz.y);

        ImGui::Dummy(sz);
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void renderHierarchy() {
        ImGui::Begin("场景层级");

        static char search[256] = "";
        ImGui::InputTextWithHint("##搜索", "搜索物体...", search, sizeof(search));
        ImGui::Spacing();

        for (const auto& [id, entity] : m_scene->getEntities()) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (m_selectedEntityID == id) flags |= ImGuiTreeNodeFlags_Selected;

            std::string name = entity->hasComponent<TagComponent>() ?
                entity->getComponent<TagComponent>().name : "物体";

            bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(id), flags, "%s", name.c_str());
            if (ImGui::IsItemClicked()) {
                m_selectedEntityID = id;
                m_selectedEntity = entity.get();
            }
            if (opened) ImGui::TreePop();
        }

        ImGui::End();
    }

    void renderInspector() {
        ImGui::Begin("属性检查器");

        if (m_selectedEntity) {
            if (m_selectedEntity->hasComponent<TagComponent>()) {
                auto& tag = m_selectedEntity->getComponent<TagComponent>();
                char buf[256];
                strncpy(buf, tag.name.c_str(), sizeof(buf));
                if (ImGui::InputText("名称", buf, sizeof(buf))) tag.name = buf;
            }

            ImGui::Spacing();

            if (m_selectedEntity->hasComponent<TransformComponent>()) {
                if (ImGui::CollapsingHeader("变换", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& t = m_selectedEntity->getComponent<TransformComponent>();
                    ImGui::DragFloat3("位置", &t.position.x, 0.1f);
                    ImGui::DragFloat3("旋转", &t.rotation.x, 1.0f);
                    ImGui::DragFloat3("缩放", &t.scale.x, 0.1f, 0.0f, 100.0f);
                }
            }

            if (m_selectedEntity->hasComponent<MeshRendererComponent>()) {
                if (ImGui::CollapsingHeader("网格渲染器", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& r = m_selectedEntity->getComponent<MeshRendererComponent>();
                    ImGui::ColorEdit4("基础颜色", &r.albedo.x);
                    ImGui::SliderFloat("金属度", &r.metallic, 0.0f, 1.0f);
                    ImGui::SliderFloat("粗糙度", &r.roughness, 0.0f, 1.0f);
                    ImGui::Checkbox("可见", &r.visible);
                }
            }

            if (m_selectedEntity->hasComponent<LightComponent>()) {
                if (ImGui::CollapsingHeader("光源", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& l = m_selectedEntity->getComponent<LightComponent>();
                    ImGui::ColorEdit3("颜色", &l.color.x);
                    ImGui::SliderFloat("强度", &l.intensity, 0.0f, 100.0f);
                }
            }

            ImGui::Spacing();
            if (ImGui::Button("添加组件", ImVec2(-1, 0)))
                ImGui::OpenPopup("添加组件");
            if (ImGui::BeginPopup("添加组件")) {
                if (ImGui::MenuItem("变换")) m_selectedEntity->addComponent<TransformComponent>();
                if (ImGui::MenuItem("网格渲染器")) m_selectedEntity->addComponent<MeshRendererComponent>();
                if (ImGui::MenuItem("光源")) m_selectedEntity->addComponent<LightComponent>();
                ImGui::EndPopup();
            }
        } else {
            ImGui::TextColored(ImVec4(0.42f, 0.42f, 0.50f, 1.0f), "未选择物体");
        }

        ImGui::End();
    }

    void renderConsole() {
        ImGui::Begin("控制台");

        ImGui::Text("输出");
        ImGui::Spacing();

        ImGui::BeginChild("日志", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        ImGui::TextColored(ImVec4(0.30f, 0.70f, 1.0f, 1.0f), "[信息] 编辑器已启动");
        ImGui::TextColored(ImVec4(0.30f, 0.70f, 1.0f, 1.0f), "[信息] 场景已加载，包含 %d 个物体", m_scene->getEntityCount());
        ImGui::EndChild();

        static char input[256] = "";
        ImGui::InputText("##输入", input, sizeof(input));
        ImGui::SameLine();
        if (ImGui::Button("发送")) memset(input, 0, sizeof(input));

        ImGui::End();
    }

    void renderStatusBar() {
        ImGui::Begin("##状态栏", nullptr,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);

        ImGui::Text("物体: %d", m_scene->getEntityCount());
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        ImGui::Text("模式: %s", m_mode == EditorMode::Edit ? "编辑" : "运行");
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        ImGui::Text("天枢引擎 v0.32.0 | ESC 退出");

        ImGui::End();
    }

    // ========== 场景和渲染 ==========
    void createMeshes() {
        std::vector<PBRVertex> cubeV = {
            {{-0.5f,-0.5f,0.5f},{0,0,1},{0,0}}, {{0.5f,-0.5f,0.5f},{0,0,1},{1,0}},
            {{0.5f,0.5f,0.5f},{0,0,1},{1,1}}, {{-0.5f,0.5f,0.5f},{0,0,1},{0,1}},
            {{-0.5f,-0.5f,-0.5f},{0,0,-1},{0,0}}, {{0.5f,-0.5f,-0.5f},{0,0,-1},{1,0}},
            {{0.5f,0.5f,-0.5f},{0,0,-1},{1,1}}, {{-0.5f,0.5f,-0.5f},{0,0,-1},{0,1}},
            {{-0.5f,-0.5f,-0.5f},{-1,0,0},{0,0}}, {{-0.5f,-0.5f,0.5f},{-1,0,0},{1,0}},
            {{-0.5f,0.5f,0.5f},{-1,0,0},{1,1}}, {{-0.5f,0.5f,-0.5f},{-1,0,0},{0,1}},
            {{0.5f,-0.5f,-0.5f},{1,0,0},{0,0}}, {{0.5f,-0.5f,0.5f},{1,0,0},{1,0}},
            {{0.5f,0.5f,0.5f},{1,0,0},{1,1}}, {{0.5f,0.5f,-0.5f},{1,0,0},{0,1}},
            {{-0.5f,0.5f,0.5f},{0,1,0},{0,0}}, {{0.5f,0.5f,0.5f},{0,1,0},{1,0}},
            {{0.5f,0.5f,-0.5f},{0,1,0},{1,1}}, {{-0.5f,0.5f,-0.5f},{0,1,0},{0,1}},
            {{-0.5f,-0.5f,0.5f},{0,-1,0},{0,0}}, {{0.5f,-0.5f,0.5f},{0,-1,0},{1,0}},
            {{0.5f,-0.5f,-0.5f},{0,-1,0},{1,1}}, {{-0.5f,-0.5f,-0.5f},{0,-1,0},{0,1}},
        };
        std::vector<uint32_t> cubeI = {
            0,1,2,2,3,0, 4,6,5,6,4,7, 8,9,10,10,11,8, 12,14,13,14,12,15,
            16,17,18,18,19,16, 20,22,21,22,20,23
        };
        m_cubeMesh = std::make_unique<Mesh>(m_renderer->getDevice(), cubeV, cubeI);

        std::vector<PBRVertex> groundV = {
            {{-0.5f,0,-0.5f},{0,1,0},{0,0}}, {{0.5f,0,-0.5f},{0,1,0},{1,0}},
            {{0.5f,0,0.5f},{0,1,0},{1,1}}, {{-0.5f,0,0.5f},{0,1,0},{0,1}},
        };
        std::vector<uint32_t> groundI = {0,1,2,2,3,0};
        m_groundMesh = std::make_unique<Mesh>(m_renderer->getDevice(), groundV, groundI);
        m_sphereMesh = std::make_unique<Mesh>(m_renderer->getDevice(), cubeV, cubeI);
    }

    void setupPipeline() {
        m_descriptorSetLayout = std::make_unique<DescriptorSetLayout>(m_renderer->getDevice());
        auto vs = std::make_shared<Shader>(m_renderer->getDevice(), "shaders/pbr_vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
        auto fs = std::make_shared<Shader>(m_renderer->getDevice(), "shaders/pbr_frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
        std::vector<std::shared_ptr<Shader>> shaders = {vs, fs};
        m_pipeline = std::make_unique<Pipeline>(m_renderer->getDevice(), m_renderer->getRenderPass(), shaders, m_descriptorSetLayout->getLayout(), true);

        m_sceneUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(SceneUBO), 2);
        m_lightUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(LightUBO), 2);
        m_materialUBO = std::make_unique<UniformBuffer>(m_renderer->getDevice(), sizeof(MaterialUBO), 2);

        m_descriptorPool = std::make_unique<DescriptorPool>(m_renderer->getDevice(), 2);
        m_descriptorSets = std::make_unique<DescriptorSets>(m_renderer->getDevice(), m_descriptorSetLayout->getLayout(), m_descriptorPool->getPool(), 2);

        for (uint32_t i = 0; i < 2; i++) {
            m_descriptorSets->bindSceneUBO(i, *m_sceneUBO);
            m_descriptorSets->bindLightUBO(i, *m_lightUBO);
            m_descriptorSets->bindMaterialUBO(i, *m_materialUBO);
        }

        m_lightData.directionalLightCount = 1;
        m_lightData.directionalLights[0].directionAndIntensity = Vec4(-0.5f, -1.0f, -0.3f, 2.0f);
        m_lightData.directionalLights[0].color = Vec4(1.0f, 0.95f, 0.9f, 1.0f);
        m_lightData.pointLightCount = 2;
        m_lightData.pointLights[0].positionAndRange = Vec4(3, 3, 3, 10);
        m_lightData.pointLights[0].colorAndIntensity = Vec4(1, 0.6f, 0.3f, 50);
        m_lightData.pointLights[1].positionAndRange = Vec4(-3, 3, -3, 10);
        m_lightData.pointLights[1].colorAndIntensity = Vec4(0.3f, 0.6f, 1, 50);
    }

    void createDefaultScene() {
        auto& g = m_scene->createEntity("地面");
        auto& gt = g.addComponent<TransformComponent>();
        gt.scale = Vec3(20, 0.1f, 20);
        auto& gr = g.addComponent<MeshRendererComponent>();
        gr.albedo = Vec4(0.4f, 0.4f, 0.45f, 1); gr.metallic = 0; gr.roughness = 0.9f;

        auto& c1 = m_scene->createEntity("红色立方体");
        c1.addComponent<TransformComponent>(Vec3(-2, 1, 0));
        auto& r1 = c1.addComponent<MeshRendererComponent>();
        r1.albedo = Vec4(0.8f, 0.2f, 0.2f, 1); r1.metallic = 0.3f; r1.roughness = 0.4f;

        auto& c2 = m_scene->createEntity("蓝色立方体");
        c2.addComponent<TransformComponent>(Vec3(0, 1, 0));
        auto& r2 = c2.addComponent<MeshRendererComponent>();
        r2.albedo = Vec4(0.2f, 0.5f, 0.8f, 1); r2.metallic = 0.8f; r2.roughness = 0.2f;

        auto& s = m_scene->createEntity("金色球体");
        s.addComponent<TransformComponent>(Vec3(2, 1, 0));
        auto& sr = s.addComponent<MeshRendererComponent>();
        sr.albedo = Vec4(1, 0.84f, 0, 1); sr.metallic = 1.0f; sr.roughness = 0.1f;
    }

    void drawScene(VkCommandBuffer cmd) {
        uint32_t f = m_renderer->getCurrentFrame();

        SceneUBO su{};
        su.view = m_camera->getViewMatrix();
        float ar = (float)getWindow().getWidth() / (float)getWindow().getHeight();
        su.projection = m_camera->getProjectionMatrix(ar);
        su.projection[1][1] *= -1;
        su.cameraPos = m_camera->getPosition();
        su.ambientColor = Vec3(0.05f, 0.05f, 0.07f);
        m_sceneUBO->update(f, &su, sizeof(su));
        m_lightUBO->update(f, &m_lightData, sizeof(m_lightData));

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipeline());

        VkViewport vp{0, 0, (float)getWindow().getWidth(), (float)getWindow().getHeight(), 0, 1};
        vkCmdSetViewport(cmd, 0, 1, &vp);
        VkRect2D sc{{0, 0}, {(uint32_t)getWindow().getWidth(), (uint32_t)getWindow().getHeight()}};
        vkCmdSetScissor(cmd, 0, 1, &sc);

        m_scene->view<TransformComponent, MeshRendererComponent>([&](Entity& e) {
            auto& t = e.getComponent<TransformComponent>();
            auto& r = e.getComponent<MeshRendererComponent>();
            if (!r.visible) return;

            MaterialUBO mu{};
            mu.albedo = r.albedo; mu.metallic = r.metallic; mu.roughness = r.roughness; mu.ao = r.ao;
            m_materialUBO->update(f, &mu, sizeof(mu));

            VkDescriptorSet ds = m_descriptorSets->getSet(f);
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getLayout(), 0, 1, &ds, 0, nullptr);

            PushConstants pc{};
            pc.model = t.getTransformMatrix();
            vkCmdPushConstants(cmd, m_pipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);

            auto& name = e.getComponent<TagComponent>().name;
            if (name.find("球体") != std::string::npos) m_sphereMesh->draw(cmd);
            else if (name.find("地面") != std::string::npos) m_groundMesh->draw(cmd);
            else m_cubeMesh->draw(cmd);
        });
    }

    // 成员
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<ImGuiIntegration> m_imgui;
    std::unique_ptr<Mesh> m_cubeMesh, m_groundMesh, m_sphereMesh;
    std::unique_ptr<DescriptorSetLayout> m_descriptorSetLayout;
    std::unique_ptr<Pipeline> m_pipeline;
    std::unique_ptr<UniformBuffer> m_sceneUBO, m_lightUBO, m_materialUBO;
    std::unique_ptr<DescriptorPool> m_descriptorPool;
    std::unique_ptr<DescriptorSets> m_descriptorSets;
    LightUBO m_lightData;
    bool m_initialized = false;
    EditorMode m_mode = EditorMode::Edit;
    float m_rotation = 0;
    EntityID m_selectedEntityID = 0;
    Entity* m_selectedEntity = nullptr;
};

int main() {
    try {
        EditorApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
