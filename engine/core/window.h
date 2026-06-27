#pragma once

#include <string>
#include <functional>
#include <GLFW/glfw3.h>
#include "event.h"

namespace spark {

struct WindowProps {
    std::string title;
    int width, height;

    WindowProps(const std::string& t = "Spark Engine", int w = 1280, int h = 720)
        : title(t), width(w), height(h) {}
};

class Window {
public:
    Window(const WindowProps& props = WindowProps());
    ~Window();

    void update();

    int getWidth() const { return m_data.width; }
    int getHeight() const { return m_data.height; }
    bool shouldClose() const { return glfwWindowShouldClose(m_window); }

    GLFWwindow* getNativeWindow() const { return m_window; }

    void setEventCallback(std::function<void(Event&)> callback) {
        m_data.eventCallback = std::move(callback);
    }

private:
    void init(const WindowProps& props);
    void shutdown();

    GLFWwindow* m_window;

    struct WindowData {
        std::string title;
        int width, height;
        std::function<void(Event&)> eventCallback;
    };

    WindowData m_data;
};

} // namespace spark
