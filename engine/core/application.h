#pragma once

#include <memory>
#include <string>
#include "window.h"
#include "event.h"
#include "input.h"

namespace spark {

class Application {
public:
    Application(const std::string& name = "Spark Engine", int width = 1280, int height = 720);
    virtual ~Application();

    void run();

    Window& getWindow() { return *m_window; }
    EventDispatcher& getEventDispatcher() { return m_eventDispatcher; }

    static Application& get() { return *s_instance; }

protected:
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender() {}
    virtual void onEvent(Event& event) {}

private:
    bool onWindowResize(WindowResizeEvent& e);
    bool onWindowClose(WindowCloseEvent& e);

    std::unique_ptr<Window> m_window;
    EventDispatcher m_eventDispatcher;
    bool m_running = true;
    float m_lastFrameTime = 0.0f;

    static Application* s_instance;
};

} // namespace spark
