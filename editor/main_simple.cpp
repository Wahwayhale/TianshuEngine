/**
 * 天枢引擎编辑器 - 最简版
 * 只测试窗口和渲染器是否工作
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "renderer/camera.h"
#include <memory>
#include <iostream>

using namespace spark;

class SimpleEditor : public Application {
public:
    SimpleEditor() : Application("Tianshu Editor", 1280, 720) {
        SPARK_INFO("Creating renderer...");
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );
        SPARK_INFO("Renderer created!");
        SPARK_INFO("Simple Editor started!");
    }

    ~SimpleEditor() {
        if (m_renderer) {
            m_renderer->waitIdle();
        }
    }

protected:
    void onUpdate(float deltaTime) override {
        // 更新逻辑
    }

    void onRender() override {
        m_renderer->beginFrame();
        // 渲染逻辑将在这里
        m_renderer->endFrame();
    }

    void onEvent(Event& event) override {
        if (auto* resizeEvent = dynamic_cast<WindowResizeEvent*>(&event)) {
            m_renderer->onResize(resizeEvent->width, resizeEvent->height);
        }
    }

private:
    std::unique_ptr<Renderer> m_renderer;
};

int main() {
    std::cout << "=== Starting Simple Editor ===" << std::endl;
    std::cout << "Step 1: Before try block" << std::endl;

    try {
        std::cout << "Step 2: Creating SimpleEditor..." << std::endl;
        SimpleEditor editor;
        std::cout << "Step 3: SimpleEditor created!" << std::endl;
        std::cout << "Step 4: Entering main loop..." << std::endl;
        editor.run();
        std::cout << "Step 5: Main loop ended" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "=== Editor exited normally ===" << std::endl;
    return EXIT_SUCCESS;
}
