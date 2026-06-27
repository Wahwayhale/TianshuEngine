#pragma once

#include <GLFW/glfw3.h>
#include <array>

namespace spark {

class Input {
public:
    static void init(GLFWwindow* window);
    static void update();

    // Keyboard
    static bool isKeyPressed(int key);
    static bool isKeyJustPressed(int key);

    // Mouse
    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonJustPressed(int button);
    static double getMouseX();
    static double getMouseY();
    static double getMouseDeltaX();
    static double getMouseDeltaY();
    static double getScrollDelta();

    // Cursor
    static void setCursorMode(int mode);

private:
    static GLFWwindow* s_window;
    static std::array<bool, GLFW_KEY_LAST> s_currentKeys;
    static std::array<bool, GLFW_KEY_LAST> s_previousKeys;
    static std::array<bool, GLFW_MOUSE_BUTTON_LAST> s_currentMouseButtons;
    static std::array<bool, GLFW_MOUSE_BUTTON_LAST> s_previousMouseButtons;
    static double s_mouseX, s_mouseY;
    static double s_lastMouseX, s_lastMouseY;
    static double s_mouseDeltaX, s_mouseDeltaY;
    static double s_scrollDelta;
};

} // namespace spark
