#include "profiler.h"
#include <sstream>
#include <iomanip>

namespace spark {

Profiler& Profiler::get() {
    static Profiler instance;
    return instance;
}

Profiler::Profiler() = default;

Profiler::~Profiler() {
    endSession();
}

void Profiler::beginSession(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_outputFile.open(filepath);
    m_outputFile << "{\"otherData\": {},\"traceEvents\":[";
    m_outputFile.flush();
    m_active = true;
    m_profileCount = 0;
}

void Profiler::endSession() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_active) {
        m_outputFile << "]}";
        m_outputFile.flush();
        m_outputFile.close();
        m_active = false;
    }
}

void Profiler::writeProfile(const ProfileResult& result) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_profileCount++ > 0) {
        m_outputFile << ",";
    }

    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');

    m_outputFile << "{";
    m_outputFile << "\"cat\":\"function\",";
    m_outputFile << "\"dur\":" << (result.endTime - result.startTime) << ",";
    m_outputFile << "\"name\":\"" << name << "\",";
    m_outputFile << "\"ph\":\"X\",";
    m_outputFile << "\"pid\":0,";
    m_outputFile << "\"tid\":" << result.threadID << ",";
    m_outputFile << "\"ts\":" << result.startTime;
    m_outputFile << "}";

    m_outputFile.flush();
}

Profiler::Timer::Timer(const char* name)
    : m_name(name) {
    m_startTime = std::chrono::high_resolution_clock::now();
}

Profiler::Timer::~Timer() {
    auto endTime = std::chrono::high_resolution_clock::now();

    float start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTime).time_since_epoch().count() * 0.001f;
    float end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count() * 0.001f;

    ProfileResult result;
    result.name = m_name;
    result.startTime = start;
    result.endTime = end;
    result.threadID = 0;  // Would use actual thread ID

    Profiler::get().writeProfile(result);
}

PerformanceMonitor& PerformanceMonitor::get() {
    static PerformanceMonitor instance;
    return instance;
}

PerformanceMonitor::PerformanceMonitor() {
    m_frameTimes.reserve(1000);
}

void PerformanceMonitor::beginFrame() {
    m_stats.drawCalls = 0;
    m_stats.triangles = 0;
    m_stats.vertices = 0;
    m_stats.textureBindings = 0;
    m_stats.shaderSwitches = 0;
}

void PerformanceMonitor::endFrame() {
    // Calculate FPS
    m_frameCount++;
    if (m_frameTimes.size() >= 100) {
        m_frameTimes.erase(m_frameTimes.begin());
    }
    m_frameTimes.push_back(m_stats.frameTime);

    float sum = 0.0f;
    for (float t : m_frameTimes) {
        sum += t;
    }
    m_averageFPS = m_frameTimes.size() / sum * 1000.0f;
}

void PerformanceMonitor::addDrawCall(uint32_t triangleCount) {
    m_stats.drawCalls++;
    m_stats.triangles += triangleCount;
    m_stats.vertices += triangleCount * 3;
}

void PerformanceMonitor::addTextureBinding() {
    m_stats.textureBindings++;
}

void PerformanceMonitor::addShaderSwitch() {
    m_stats.shaderSwitches++;
}

void PerformanceMonitor::setMemoryUsage(size_t cpu, size_t gpu) {
    m_stats.memoryUsage = cpu;
    m_stats.gpuMemoryUsage = gpu;
}

} // namespace spark
