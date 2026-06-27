#pragma once

#include "math/math_types.h"
#include <string>
#include <memory>

namespace spark {

// 音频源 - 封装单个音频文件的播放
class AudioSource {
public:
    AudioSource(const std::string& filepath);
    ~AudioSource();

    // 播放控制
    void play();
    void pause();
    void stop();
    void restart();

    bool isPlaying() const;

    // 属性设置
    void setPosition(Vec3 position);
    void setVelocity(Vec3 velocity);
    void setVolume(float volume);
    void setPitch(float pitch);
    void setLooping(bool loop);
    void setRange(float range);

    // 属性获取
    Vec3 getPosition() const { return m_position; }
    float getVolume() const { return m_volume; }
    float getPitch() const { return m_pitch; }
    bool isLooping() const { return m_looping; }
    float getRange() const { return m_range; }
    const std::string& getFilepath() const { return m_filepath; }

private:
    Vec3 m_position = Vec3(0.0f);
    Vec3 m_velocity = Vec3(0.0f);
    float m_volume = 1.0f;
    float m_pitch = 1.0f;
    float m_range = 20.0f;
    bool m_looping = false;
    std::string m_filepath;

    // miniaudio 声音对象（前向声明）
    void* m_sound = nullptr;
};

} // namespace spark
