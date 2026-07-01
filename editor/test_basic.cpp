/**
 * 最基础测试 - 只渲染清屏
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include <iostream>

using namespace spark;

class TestApp : public Application {
public:
    TestApp() : Application("Test Basic", 800, 600) {
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );
        std::cout << "Renderer created!" << std::endl;
        m_initialized = true;
    }

    ~TestApp() {
        if (m_renderer) m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {}
    void onRender() override {
        if (!m_initialized) return;

        // beginFrame 已经开始了渲染通道
        m_renderer->beginFrame();

        // 这里可以添加渲染命令
        // 但不要再次开始渲染通道

        // endFrame 会结束渲染通道并提交
        m_renderer->endFrame();
    }

private:
    std::unique_ptr<Renderer> m_renderer;
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
