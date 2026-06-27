/**
 * @file main.cpp
 * @brief Puzzle Game Example
 *
 * 益智游戏示例
 * - 方块匹配
 * - 计分系统
 * - 关卡系统
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>
#include <vector>

using namespace spark;

struct PuzzleBlockComponent : public Component {
    int color = 0;  // 0-4 不同颜色
    int gridX = 0;
    int gridY = 0;
    bool selected = false;
};

struct PuzzleGridComponent : public Component {
    int width = 8;
    int height = 8;
    std::vector<std::vector<int>> grid;
};

class PuzzleGameApp : public Application {
public:
    PuzzleGameApp() : Application("Puzzle Game", 800, 600) {
        m_scene = std::make_unique<Scene>();

        // 创建网格
        createGrid();

        SPARK_INFO("Puzzle Game started!");
    }

    ~PuzzleGameApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_scene->update(deltaTime);
    }

    void onRender() override {
        // 渲染场景
    }

private:
    void createGrid() {
        auto& grid = m_scene->createEntity("Grid");
        grid.addComponent<TransformComponent>(Vec3(0.0f, 0.0f, 0.0f));
        grid.addComponent<PuzzleGridComponent>();
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        PuzzleGameApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
