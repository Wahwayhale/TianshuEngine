#pragma once

#include "math/math_types.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace spark {

class AudioSource;

// 音频引擎 - 使用 miniaudio 后端
class AudioEngine {
public:
    static AudioEngine& get();

    bool initialize();
    void shutdown();

    void update();

    // 音频源管理
    std::shared_ptr<AudioSource> createSource(const std::string& filepath);
    void removeSource(std::shared_ptr<AudioSource> source);

    // 监听器（通常是相机）
    void setListenerPosition(Vec3 position);
    void setListenerOrientation(Vec3 forward, Vec3 up);
    void setListenerVelocity(Vec3 velocity);

    // 全局设置
    void setMasterVolume(float volume);
    float getMasterVolume() const { return m_masterVolume; }

    bool isInitialized() const { return m_initialized; }

private:
    AudioEngine() = default;
    ~AudioEngine() = default;

    bool m_initialized = false;
    float m_masterVolume = 1.0f;

    // miniaudio 设备（前向声明避免头文件依赖）
    void* m_device = nullptr;
    void* m_engine = nullptr;

    std::unordered_map<std::string, std::shared_ptr<AudioSource>> m_sources;
};

} // namespace spark
