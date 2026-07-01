/**
 * 最小测试 - 只测试窗口创建
 */

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "Test 1: glfwInit..." << std::endl;
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return 1;
    }
    std::cout << "Test 2: glfwInit OK" << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    std::cout << "Test 3: Creating window..." << std::endl;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }
    std::cout << "Test 4: Window created!" << std::endl;

    int frame = 0;
    while (!glfwWindowShouldClose(window) && frame < 100) {
        glfwPollEvents();
        frame++;
    }
    std::cout << "Test 5: Ran " << frame << " frames" << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "Test 6: Done!" << std::endl;
    return 0;
}
