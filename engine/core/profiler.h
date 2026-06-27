#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <mutex>

namespace spark {

struct ProfileResult {
    std::string name;
    float startTime;
    float endTime;
    uint32_t threadID;
};

class Profiler {
public:
    static Profiler& get();

    void beginSession(const std::string& filepath = "profile.json");
    void endSession();

    void writeProfile(const ProfileResult& result);

    // Scoped timer for automatic profiling
    class Timer {
    public:
        Timer(const char* name);
        ~Timer();

    private:
        const char* m_name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    };

private:
    Profiler();
    ~Profiler();

    std::ofstream m_outputFile;
    int m_profileCount = 0;
    std::mutex m_mutex;
    bool m_active = false;
};

// Macros for easy profiling
#define SPARK_PROFILE_SCOPE(name) Profiler::Timer timer##__LINE__(name)
#define SPARK_PROFILE_FUNCTION() SPARK_PROFILE_SCOPE(__FUNCTION__)

// Performance statistics
struct PerformanceStats {
    float fps = 0.0f;
    float frameTime = 0.0f;
    float cpuTime = 0.0f;
    float gpuTime = 0.0f;
    uint32_t drawCalls = 0;
    uint32_t triangles = 0;
    uint32_t vertices = 0;
    uint32_t textureBindings = 0;
    uint32_t shaderSwitches = 0;
    size_t memoryUsage = 0;
    size_t gpuMemoryUsage = 0;
};

class PerformanceMonitor {
public:
    static PerformanceMonitor& get();

    void beginFrame();
    void endFrame();

    void addDrawCall(uint32_t triangleCount);
    void addTextureBinding();
    void addShaderSwitch();

    const PerformanceStats& getStats() const { return m_stats; }
    float getAverageFPS() const { return m_averageFPS; }

    void setMemoryUsage(size_t cpu, size_t gpu);

private:
    PerformanceMonitor();

    PerformanceStats m_stats;
    std::vector<float> m_frameTimes;
    float m_averageFPS = 0.0f;
    uint32_t m_frameCount = 0;
};

} // namespace spark
