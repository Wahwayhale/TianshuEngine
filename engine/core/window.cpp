#include "window.h"
#include "log.h"
#include <stdexcept>

namespace spark {

Window::Window(const WindowProps& props) {
    init(props);
}

Window::~Window() {
    shutdown();
}

void Window::init(const WindowProps& props) {
    m_data.title = props.title;
    m_data.width = props.width;
    m_data.height = props.height;

    SPARK_CORE_INFO("Creating window: {0} ({1}, {2})", props.title, props.width, props.height);

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window!");
    }

    glfwSetWindowUserPointer(m_window, &m_data);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        data.width = width;
        data.height = height;

        WindowResizeEvent event(width, height);
        if (data.eventCallback) {
            data.eventCallback(event);
        }
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        WindowCloseEvent event;
        if (data.eventCallback) {
            data.eventCallback(event);
        }
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        KeyEvent event(key, action);
        if (data.eventCallback) {
            data.eventCallback(event);
        }
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        MouseButtonEvent event(button, action);
        if (data.eventCallback) {
            data.eventCallback(event);
        }
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        MouseMoveEvent event(xpos, ypos);
        if (data.eventCallback) {
            data.eventCallback(event);
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        MouseScrollEvent event(xoffset, yoffset);
        if (data.eventCallback) {
            data.eventCallback(event);
        }
    });
}

void Window::shutdown() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::update() {
    glfwPollEvents();
}

} // namespace spark
