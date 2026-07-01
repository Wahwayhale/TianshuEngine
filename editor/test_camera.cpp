/**
 * 测试 Camera 初始化
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "renderer/camera.h"
#include <iostream>

using namespace spark;

class TestApp : public Application {
public:
    TestApp() : Application("Test Camera", 800, 600) {
        std::cout << "Creating renderer..." << std::endl;
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        std::cout << "Creating camera..." << std::endl;
        m_camera = std::make_unique<Camera>(Vec3(0.0f, 2.0f, 5.0f));

        std::cout << "All created!" << std::endl;
    }

    ~TestApp() {
        if (m_renderer) {
            m_renderer->waitIdle();
        }
    }

protected:
    void onUpdate(float deltaTime) override {
        m_camera->update(deltaTime);
    }

    void onRender() override {
        m_renderer->beginFrame();
        m_renderer->endFrame();
    }

    void onEvent(Event& event) override {
        if (auto* resizeEvent = dynamic_cast<WindowResizeEvent*>(&event)) {
            m_renderer->onResize(resizeEvent->width, resizeEvent->height);
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
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
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
