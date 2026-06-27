#include "audio_engine.h"
#include "audio_source.h"
#include "core/log.h"

// miniaudio 实现
// 注意：需要将 miniaudio.h 放到 third_party/miniaudio/ 目录
// 下载地址: https://github.com/mackron/miniaudio

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace spark {

AudioEngine& AudioEngine::get() {
    static AudioEngine instance;
    return instance;
}

bool AudioEngine::initialize() {
    if (m_initialized) return true;

    // 初始化 miniaudio 引擎
    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.listenerCount = 1;

    m_engine = new ma_engine;
    ma_result result = ma_engine_init(&engineConfig, static_cast<ma_engine*>(m_engine));

    if (result != MA_SUCCESS) {
        SPARK_CORE_ERROR("Failed to initialize audio engine: {0}", result);
        delete static_cast<ma_engine*>(m_engine);
        m_engine = nullptr;
        return false;
    }

    m_initialized = true;
    SPARK_CORE_INFO("Audio engine initialized with miniaudio.");
    return true;
}

void AudioEngine::shutdown() {
    if (!m_initialized) return;

    // 清理所有音频源
    m_sources.clear();

    // 关闭引擎
    if (m_engine) {
        ma_engine_uninit(static_cast<ma_engine*>(m_engine));
        delete static_cast<ma_engine*>(m_engine);
        m_engine = nullptr;
    }

    m_initialized = false;
    SPARK_CORE_INFO("Audio engine shutdown.");
}

void AudioEngine::update() {
    if (!m_initialized) return;

    // miniaudio 自动处理音频更新
    // 这里可以添加自定义的音频逻辑
}

std::shared_ptr<AudioSource> AudioEngine::createSource(const std::string& filepath) {
    if (!m_initialized) {
        SPARK_CORE_WARN("Audio engine not initialized!");
        return nullptr;
    }

    auto source = std::make_shared<AudioSource>(filepath);
    m_sources[filepath] = source;
    return source;
}

void AudioEngine::removeSource(std::shared_ptr<AudioSource> source) {
    if (!source) return;

    const auto& filepath = source->getFilepath();
    m_sources.erase(filepath);
}

void AudioEngine::setListenerPosition(Vec3 position) {
    if (!m_initialized || !m_engine) return;

    ma_engine_listener_set_position(static_cast<ma_engine*>(m_engine), 0,
                                     position.x, position.y, position.z);
}

void AudioEngine::setListenerOrientation(Vec3 forward, Vec3 up) {
    if (!m_initialized || !m_engine) return;

    ma_engine_listener_set_direction(static_cast<ma_engine*>(m_engine), 0,
                                      forward.x, forward.y, forward.z);
    // miniaudio 使用世界坐标系的上方向
    ma_engine_listener_set_world_up(static_cast<ma_engine*>(m_engine), 0,
                                     up.x, up.y, up.z);
}

void AudioEngine::setListenerVelocity(Vec3 velocity) {
    if (!m_initialized || !m_engine) return;

    ma_engine_listener_set_velocity(static_cast<ma_engine*>(m_engine), 0,
                                     velocity.x, velocity.y, velocity.z);
}

void AudioEngine::setMasterVolume(float volume) {
    m_masterVolume = volume;
    if (m_initialized && m_engine) {
        ma_engine_set_volume(static_cast<ma_engine*>(m_engine), volume);
    }
}

} // namespace spark
