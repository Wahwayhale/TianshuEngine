#pragma once

#include <string>
#include <vector>
#include <deque>

namespace spark {

// 性能指标
struct PerformanceMetrics {
    float fps = 0.0f;
    float frameTime = 0.0f;
    float cpuTime = 0.0f;
    float gpuTime = 0.0f;

    // 内存
    size_t totalMemory = 0;
    size_t usedMemory = 0;
    size_t gpuMemory = 0;

    // 渲染
    int drawCalls = 0;
    int triangles = 0;
    int vertices = 0;
    int textureBindings = 0;
    int shaderSwitches = 0;

    // 物理
    int physicsBodies = 0;
    int collisionPairs = 0;
    float physicsTime = 0.0f;

    // 场景
    int entityCount = 0;
    int visibleEntities = 0;
};

// 性能采样
struct PerformanceSample {
    float timestamp;
    float fps;
    float frameTime;
    float cpuTime;
    float gpuTime;
};

// 性能分析器面板
class ProfilerPanel {
public:
    ProfilerPanel();
    ~ProfilerPanel();

    void render();

    // 更新指标
    void updateMetrics(const PerformanceMetrics& metrics);

private:
    void renderFPSGraph();
    void renderFrameTimeGraph();
    void renderMemoryUsage();
    void renderRenderStats();
    void renderPhysicsStats();
    void renderSceneStats();

    PerformanceMetrics m_metrics;
    std::deque<PerformanceSample> m_samples;
    int m_maxSamples = 120;  // 2秒 @ 60fps

    // 图表设置
    float m_fpsScale = 120.0f;
    float m_frameTimeScale = 33.3f;  // 30fps
};

} // namespace spark
