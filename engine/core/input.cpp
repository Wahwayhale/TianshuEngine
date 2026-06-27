#include "input.h"

namespace spark {

GLFWwindow* Input::s_window = nullptr;
std::array<bool, GLFW_KEY_LAST> Input::s_currentKeys = {};
std::array<bool, GLFW_KEY_LAST> Input::s_previousKeys = {};
std::array<bool, GLFW_MOUSE_BUTTON_LAST> Input::s_currentMouseButtons = {};
std::array<bool, GLFW_MOUSE_BUTTON_LAST> Input::s_previousMouseButtons = {};
double Input::s_mouseX = 0.0;
double Input::s_mouseY = 0.0;
double Input::s_lastMouseX = 0.0;
double Input::s_lastMouseY = 0.0;
double Input::s_mouseDeltaX = 0.0;
double Input::s_mouseDeltaY = 0.0;
double Input::s_scrollDelta = 0.0;

void Input::init(GLFWwindow* window) {
    s_window = window;
    s_currentKeys.fill(false);
    s_previousKeys.fill(false);
    s_currentMouseButtons.fill(false);
    s_previousMouseButtons.fill(false);
}

void Input::update() {
    // Save previous state
    s_previousKeys = s_currentKeys;
    s_previousMouseButtons = s_currentMouseButtons;

    // Update keyboard state
    for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; ++key) {
        s_currentKeys[key] = glfwGetKey(s_window, key) == GLFW_PRESS;
    }

    // Update mouse button state
    for (int button = GLFW_MOUSE_BUTTON_1; button < GLFW_MOUSE_BUTTON_LAST; ++button) {
        s_currentMouseButtons[button] = glfwGetMouseButton(s_window, button) == GLFW_PRESS;
    }

    // Update mouse position
    s_lastMouseX = s_mouseX;
    s_lastMouseY = s_mouseY;
    glfwGetCursorPos(s_window, &s_mouseX, &s_mouseY);
    s_mouseDeltaX = s_mouseX - s_lastMouseX;
    s_mouseDeltaY = s_mouseY - s_lastMouseY;

    // Reset scroll delta
    s_scrollDelta = 0.0;
}

bool Input::isKeyPressed(int key) {
    return s_currentKeys[key];
}

bool Input::isKeyJustPressed(int key) {
    return s_currentKeys[key] && !s_previousKeys[key];
}

bool Input::isMouseButtonPressed(int button) {
    return s_currentMouseButtons[button];
}

bool Input::isMouseButtonJustPressed(int button) {
    return s_currentMouseButtons[button] && !s_previousMouseButtons[button];
}

double Input::getMouseX() { return s_mouseX; }
double Input::getMouseY() { return s_mouseY; }
double Input::getMouseDeltaX() { return s_mouseDeltaX; }
double Input::getMouseDeltaY() { return s_mouseDeltaY; }
double Input::getScrollDelta() { return s_scrollDelta; }

void Input::setCursorMode(int mode) {
    glfwSetInputMode(s_window, GLFW_CURSOR, mode);
}

} // namespace spark
