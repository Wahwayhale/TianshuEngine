/**
 * 天枢引擎编辑器 - 调试版
 */

#include <iostream>
#include <GLFW/glfw3.h>

int main() {
    std::cout << "=== Tianshu Editor Debug ===" << std::endl;
    std::cout << "Step 1: Initializing GLFW..." << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    std::cout << "Step 2: Creating window..." << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Tianshu Editor", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Step 3: Window created! Entering main loop..." << std::endl;

    int frameCount = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        frameCount++;

        if (frameCount % 100 == 0) {
            std::cout << "Frame: " << frameCount << std::endl;
        }
    }

    std::cout << "Step 4: Closing..." << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Done!" << std::endl;
    return 0;
}
