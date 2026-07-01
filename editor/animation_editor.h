#pragma once

#include <string>
#include <vector>
#include <memory>

namespace spark {

class Animation;
class Entity;

// 关键帧类型
enum class KeyframeType {
    Position,
    Rotation,
    Scale,
    Float,
    Color
};

// 关键帧数据
struct Keyframe {
    float time;
    Vec3 vec3Value;
    float floatValue;
    Vec4 colorValue;
    KeyframeType type;
};

// 动画轨道
struct AnimationTrack {
    std::string name;
    KeyframeType type;
    std::vector<Keyframe> keyframes;
};

// 动画编辑器
class AnimationEditor {
public:
    AnimationEditor();
    ~AnimationEditor();

    // 设置动画
    void setAnimation(std::shared_ptr<Animation> animation);
    std::shared_ptr<Animation> getAnimation() const { return m_animation; }

    // 播放控制
    void play();
    void pause();
    void stop();
    void setPlaybackSpeed(float speed) { m_playbackSpeed = speed; }

    // 时间控制
    void setCurrentTime(float time);
    float getCurrentTime() const { return m_currentTime; }
    float getDuration() const { return m_duration; }

    // 关键帧操作
    void addKeyframe(const std::string& trackName, const Keyframe& keyframe);
    void removeKeyframe(const std::string& trackName, float time);
    void moveKeyframe(const std::string& trackName, float oldTime, float newTime);

    // 轨道操作
    void addTrack(const AnimationTrack& track);
    void removeTrack(const std::string& trackName);

    // 获取轨道
    const std::vector<AnimationTrack>& getTracks() const { return m_tracks; }

    // 状态
    bool isPlaying() const { return m_playing; }
    bool isPaused() const { return m_paused; }

private:
    std::shared_ptr<Animation> m_animation;
    std::vector<AnimationTrack> m_tracks;

    float m_currentTime = 0.0f;
    float m_duration = 1.0f;
    float m_playbackSpeed = 1.0f;
    bool m_playing = false;
    bool m_paused = false;
};

// 动画编辑器管理器
class AnimationEditorManager {
public:
    static AnimationEditorManager& get();

    // 创建编辑器
    std::shared_ptr<AnimationEditor> createEditor();

    // 获取编辑器
    const std::vector<std::shared_ptr<AnimationEditor>>& getEditors() const { return m_editors; }

private:
    AnimationEditorManager() = default;

    std::vector<std::shared_ptr<AnimationEditor>> m_editors;
};

} // namespace spark
