#pragma once

#include <string>

namespace spark {

class StatusBar {
public:
    StatusBar();
    ~StatusBar();

    void render();

    // 更新统计信息
    void updateStats(float fps, int entityCount, size_t memoryUsage);
    void setSelectedEntity(const std::string& name);

private:
    void renderLeft();
    void renderCenter();
    void renderRight();

    // 统计数据
    float m_fps = 0.0f;
    int m_entityCount = 0;
    size_t m_memoryUsage = 0;
    std::string m_selectedEntityName;
};

} // namespace spark
