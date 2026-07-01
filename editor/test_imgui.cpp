/**
 * ImGui 测试 - 使用单独渲染通道
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "ui/imgui_integration.h"
#include <imgui.h>
#include <iostream>
#include <array>

using namespace spark;

class TestApp : public Application {
public:
    TestApp() : Application("Test ImGui", 800, 600) {
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        m_imgui = std::make_unique<ImGuiIntegration>();
        if (!m_imgui->initialize(getWindow().getNativeWindow(), m_renderer.get())) {
            std::cerr << "Failed to init ImGui!" << std::endl;
            return;
        }

        std::cout << "ImGui initialized!" << std::endl;
        m_initialized = true;
    }

    ~TestApp() {
        if (m_renderer) m_renderer->waitIdle();
        if (m_imgui) m_imgui->shutdown();
    }

protected:
    void onUpdate(float deltaTime) override {}
    void onRender() override {
        if (!m_initialized) return;

        m_renderer->beginFrame();
        VkCommandBuffer cmd = m_renderer->getCurrentCommandBuffer();

        // 第一个渲染通道：清屏
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderer->getRenderPass();
        renderPassInfo.framebuffer = m_renderer->getCurrentFramebuffer();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_renderer->getSwapchainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.1f, 0.1f, 0.15f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdEndRenderPass(cmd);

        // 第二个渲染通道：ImGui
        m_imgui->newFrame();
        ImGui::ShowDemoWindow();
        m_imgui->renderInSeparatePass(
            cmd,
            m_renderer->getSwapchainImageView(m_renderer->getImageIndex()),
            m_renderer->getSwapchainExtent()
        );

        m_renderer->endFrame();
    }

private:
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<ImGuiIntegration> m_imgui;
    bool m_initialized = false;
};

int main() {
    try {
        TestApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
