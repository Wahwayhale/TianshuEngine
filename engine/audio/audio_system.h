#pragma once

#include "ecs/system.h"
#include "ecs/components.h"
#include "math/math_types.h"
#include <memory>
#include <unordered_map>

namespace spark {

class AudioSource;
class Camera;

// 音频源组件
struct AudioSourceComponent : public Component {
    std::string audioPath;           // 音频文件路径
    float volume = 1.0f;             // 音量 (0-1)
    float pitch = 1.0f;              // 音调
    float range = 20.0f;             // 听到范围
    bool looping = false;            // 循环播放
    bool playOnStart = true;         // 开始时自动播放
    bool spatial = true;             // 3D 空间音频

    // 运行时状态
    std::shared_ptr<AudioSource> source;
    bool isPlaying = false;
};

// 音频系统 - 管理所有音频源
class AudioSystem : public System {
public:
    AudioSystem();
    ~AudioSystem() override;

    void update(Scene& scene, float deltaTime) override;

    // 设置相机（作为监听器）
    void setCamera(Camera* camera) { m_camera = camera; }

private:
    Camera* m_camera = nullptr;
    std::unordered_map<EntityID, std::shared_ptr<AudioSource>> m_activeSources;
};

} // namespace spark
