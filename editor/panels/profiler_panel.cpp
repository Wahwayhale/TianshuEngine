#include "profiler_panel.h"
#include "ui/widgets.h"
#include <imgui.h>
#include <algorithm>

namespace spark {

ProfilerPanel::ProfilerPanel() = default;
ProfilerPanel::~ProfilerPanel() = default;

void ProfilerPanel::render() {
    ImGui::Begin("Profiler");

    // 工具栏
    if (ImGui::Button("Clear")) {
        m_samples.clear();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::SliderFloat("FPS Scale", &m_fpsScale, 30.0f, 300.0f, "%.0f");

    ImGui::Separator();

    // FPS 图表
    renderFPSGraph();

    ImGui::Spacing();

    // 帧时间图表
    renderFrameTimeGraph();

    ImGui::Spacing();

    // 统计信息
    if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
        renderRenderStats();
        renderPhysicsStats();
        renderSceneStats();
        renderMemoryUsage();
    }

    ImGui::End();
}

void ProfilerPanel::updateMetrics(const PerformanceMetrics& metrics) {
    m_metrics = metrics;

    // 添加采样
    PerformanceSample sample;
    sample.timestamp = ImGui::GetTime();
    sample.fps = metrics.fps;
    sample.frameTime = metrics.frameTime;
    sample.cpuTime = metrics.cpuTime;
    sample.gpuTime = metrics.gpuTime;

    m_samples.push_back(sample);

    // 限制采样数量
    while (m_samples.size() > m_maxSamples) {
        m_samples.pop_front();
    }
}

void ProfilerPanel::renderFPSGraph() {
    ImGui::Text("FPS: %.1f", m_metrics.fps);

    // 准备数据
    std::vector<float> fpsData;
    for (const auto& sample : m_samples) {
        fpsData.push_back(sample.fps);
    }

    if (!fpsData.empty()) {
        ImGui::PlotLines("##FPS", fpsData.data(), static_cast<int>(fpsData.size()),
                         0, nullptr, 0.0f, m_fpsScale, ImVec2(0, 80));
    }

    // FPS 颜色指示
    ImVec4 fpsColor;
    if (m_metrics.fps >= 60.0f) {
        fpsColor = ImVec4(0.2f, 0.8f, 0.4f, 1.0f);  // 绿色
    } else if (m_metrics.fps >= 30.0f) {
        fpsColor = ImVec4(0.8f, 0.8f, 0.2f, 1.0f);  // 黄色
    } else {
        fpsColor = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);  // 红色
    }

    ImGui::SameLine();
    ImGui::TextColored(fpsColor, "%.0f FPS", m_metrics.fps);
}

void ProfilerPanel::renderFrameTimeGraph() {
    ImGui::Text("Frame Time: %.2f ms", m_metrics.frameTime);

    // 准备数据
    std::vector<float> frameTimeData;
    for (const auto& sample : m_samples) {
        frameTimeData.push_back(sample.frameTime);
    }

    if (!frameTimeData.empty()) {
        ImGui::PlotLines("##FrameTime", frameTimeData.data(), static_cast<int>(frameTimeData.size()),
                         0, nullptr, 0.0f, m_frameTimeScale, ImVec2(0, 60));
    }

    // 帧时间分解
    ImGui::Text("CPU: %.2f ms", m_metrics.cpuTime);
    ImGui::SameLine();
    ImGui::Text("GPU: %.2f ms", m_metrics.gpuTime);
}

void ProfilerPanel::renderMemoryUsage() {
    ImGui::Text("Memory Usage:");

    float totalMB = m_metrics.totalMemory / (1024.0f * 1024.0f);
    float usedMB = m_metrics.usedMemory / (1024.0f * 1024.0f);
    float gpuMB = m_metrics.gpuMemory / (1024.0f * 1024.0f);

    ImGui::Text("  System: %.1f / %.1f MB", usedMB, totalMB);
    ImGui::Text("  GPU: %.1f MB", gpuMB);

    // 内存使用条
    float usage = (totalMB > 0) ? (usedMB / totalMB) : 0.0f;
    ImGui::ProgressBar(usage, ImVec2(-1, 0), "%.1f%%");
}

void ProfilerPanel::renderRenderStats() {
    ImGui::Text("Rendering:");
    ImGui::Text("  Draw Calls: %d", m_metrics.drawCalls);
    ImGui::Text("  Triangles: %d", m_metrics.triangles);
    ImGui::Text("  Vertices: %d", m_metrics.vertices);
    ImGui::Text("  Texture Binds: %d", m_metrics.textureBindings);
    ImGui::Text("  Shader Switches: %d", m_metrics.shaderSwitches);
}

void ProfilerPanel::renderPhysicsStats() {
    ImGui::Text("Physics:");
    ImGui::Text("  Bodies: %d", m_metrics.physicsBodies);
    ImGui::Text("  Collision Pairs: %d", m_metrics.collisionPairs);
    ImGui::Text("  Physics Time: %.2f ms", m_metrics.physicsTime);
}

void ProfilerPanel::renderSceneStats() {
    ImGui::Text("Scene:");
    ImGui::Text("  Total Entities: %d", m_metrics.entityCount);
    ImGui::Text("  Visible Entities: %d", m_metrics.visibleEntities);
}

} // namespace spark
