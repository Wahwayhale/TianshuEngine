/**
 * 最小 Application 测试
 */

#include "core/application.h"
#include "core/log.h"
#include <iostream>

using namespace spark;

class TestApp : public Application {
public:
    TestApp() : Application("Test", 800, 600) {
        std::cout << "TestApp created!" << std::endl;
    }

    ~TestApp() {
        std::cout << "TestApp destroyed!" << std::endl;
    }

protected:
    void onUpdate(float deltaTime) override {}
    void onRender() override {}
};

int main() {
    std::cout << "Starting test..." << std::endl;
    try {
        TestApp app;
        std::cout << "App created, running..." << std::endl;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
