#include "application.h"
#include "log.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace spark {

Application* Application::s_instance = nullptr;

Application::Application(const std::string& name, int width, int height) {
    if (s_instance) {
        throw std::runtime_error("Application already exists!");
    }
    s_instance = this;

    // Initialize logging
    Log::init();
    SPARK_CORE_INFO("Initializing Spark Engine...");

    // Create window
    m_window = std::make_unique<Window>(WindowProps(name, width, height));

    // Set up input
    Input::init(m_window->getNativeWindow());

    // Set up event callbacks
    m_window->setEventCallback([this](Event& e) {
        m_eventDispatcher.dispatch(e);
        onEvent(e);
    });

    // Register internal event handlers
    m_eventDispatcher.addListener<WindowResizeEvent>(
        [this](WindowResizeEvent& e) { return onWindowResize(e); }
    );
    m_eventDispatcher.addListener<WindowCloseEvent>(
        [this](WindowCloseEvent& e) { return onWindowClose(e); }
    );

    SPARK_CORE_INFO("Spark Engine initialized successfully.");
}

Application::~Application() {
    SPARK_CORE_INFO("Shutting down Spark Engine...");
}

void Application::run() {
    m_lastFrameTime = static_cast<float>(glfwGetTime());

    while (m_running) {
        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        // Update input
        Input::update();

        // Poll window events
        m_window->update();

        // Application update
        onUpdate(deltaTime);

        // Application render
        onRender();
    }
}

bool Application::onWindowResize(WindowResizeEvent& e) {
    SPARK_CORE_INFO("Window resized: {0}x{1}", e.width, e.height);
    return false;
}

bool Application::onWindowClose(WindowCloseEvent& e) {
    m_running = false;
    return true;
}

} // namespace spark
