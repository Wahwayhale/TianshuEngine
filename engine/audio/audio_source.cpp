#include "audio_source.h"
#include "audio_engine.h"
#include "core/log.h"

#include <miniaudio.h>

namespace spark {

AudioSource::AudioSource(const std::string& filepath)
    : m_filepath(filepath) {

    if (!AudioEngine::get().isInitialized()) {
        SPARK_CORE_WARN("Audio engine not initialized, cannot load: {0}", filepath);
        return;
    }

    // 创建 miniaudio 声音对象
    m_sound = new ma_sound;

    // 获取引擎指针（通过 AudioEngine 单例）
    // 注意：这里需要访问引擎内部的 ma_engine
    // 简化实现：直接使用全局引擎配置

    SPARK_CORE_INFO("Loading audio: {0}", filepath);
}

AudioSource::~AudioSource() {
    stop();

    if (m_sound) {
        ma_sound_uninit(static_cast<ma_sound*>(m_sound));
        delete static_cast<ma_sound*>(m_sound);
        m_sound = nullptr;
    }
}

void AudioSource::play() {
    if (!m_sound) return;

    ma_result result = ma_sound_start(static_cast<ma_sound*>(m_sound));
    if (result != MA_SUCCESS) {
        SPARK_CORE_ERROR("Failed to play audio: {0}", result);
    }
}

void AudioSource::pause() {
    if (!m_sound) return;

    ma_sound_stop(static_cast<ma_sound*>(m_sound));
}

void AudioSource::stop() {
    if (!m_sound) return;

    ma_sound_stop(static_cast<ma_sound*>(m_sound));
    ma_sound_seek_to_pcm_frame(static_cast<ma_sound*>(m_sound), 0);
}

void AudioSource::restart() {
    stop();
    play();
}

bool AudioSource::isPlaying() const {
    if (!m_sound) return false;

    return ma_sound_is_playing(static_cast<ma_sound*>(m_sound));
}

void AudioSource::setPosition(Vec3 position) {
    m_position = position;

    if (m_sound) {
        ma_sound_set_position(static_cast<ma_sound*>(m_sound),
                              position.x, position.y, position.z);
    }
}

void AudioSource::setVelocity(Vec3 velocity) {
    m_velocity = velocity;

    if (m_sound) {
        ma_sound_set_velocity(static_cast<ma_sound*>(m_sound),
                              velocity.x, velocity.y, velocity.z);
    }
}

void AudioSource::setVolume(float volume) {
    m_volume = volume;

    if (m_sound) {
        ma_sound_set_volume(static_cast<ma_sound*>(m_sound), volume);
    }
}

void AudioSource::setPitch(float pitch) {
    m_pitch = pitch;

    if (m_sound) {
        ma_sound_set_pitch(static_cast<ma_sound*>(m_sound), pitch);
    }
}

void AudioSource::setLooping(bool loop) {
    m_looping = loop;

    if (m_sound) {
        ma_sound_set_looping(static_cast<ma_sound*>(m_sound), loop ? MA_TRUE : MA_FALSE);
    }
}

void AudioSource::setRange(float range) {
    m_range = range;

    // miniaudio 使用衰减模型，这里设置最大距离
    if (m_sound) {
        // 设置空间化参数
        ma_sound_set_attenuation_model(static_cast<ma_sound*>(m_sound),
                                        ma_attenuation_model_inverse);
        ma_sound_set_min_distance(static_cast<ma_sound*>(m_sound), 1.0f);
        ma_sound_set_max_distance(static_cast<ma_sound*>(m_sound), range);
    }
}

} // namespace spark
