/**
 * 最小窗口测试
 */

#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "Initializing GLFW..." << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    std::cout << "Creating window..." << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Window created! Running..." << std::endl;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    std::cout << "Closing..." << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
