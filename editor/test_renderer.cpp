/**
 * 测试 Renderer 初始化
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include <iostream>

using namespace spark;

class TestApp : public Application {
public:
    TestApp() : Application("Test Renderer", 800, 600) {
        std::cout << "Creating renderer..." << std::endl;
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );
        std::cout << "Renderer created!" << std::endl;
    }

    ~TestApp() {
        if (m_renderer) {
            m_renderer->waitIdle();
        }
    }

protected:
    void onUpdate(float deltaTime) override {}
    void onRender() override {
        m_renderer->beginFrame();
        m_renderer->endFrame();
    }

private:
    std::unique_ptr<Renderer> m_renderer;
};

int main() {
    std::cout << "Starting test..." << std::endl;
    try {
        TestApp app;
        std::cout << "Running..." << std::endl;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
