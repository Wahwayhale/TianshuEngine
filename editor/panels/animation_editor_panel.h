#pragma once

#include <string>
#include <vector>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Animation;
class Animator;

// 关键帧类型
enum class KeyframeType {
    Position,
    Rotation,
    Scale
};

// 关键帧
struct Keyframe {
    float time;
    Vec3 value;
    KeyframeType type;
};

// 动画编辑器面板
class AnimationEditorPanel {
public:
    AnimationEditorPanel();
    ~AnimationEditorPanel();

    void render();

    void setAnimation(std::shared_ptr<Animation> animation) { m_animation = animation; }
    void setAnimator(std::shared_ptr<Animator> animator) { m_animator = animator; }

private:
    void renderToolbar();
    void renderTimeline();
    void renderKeyframeEditor();
    void renderPreview();
    void renderProperties();

    std::shared_ptr<Animation> m_animation;
    std::shared_ptr<Animator> m_animator;

    // 时间线状态
    float m_currentTime = 0.0f;
    float m_duration = 1.0f;
    float m_zoom = 1.0f;
    float m_scrollX = 0.0f;

    // 播放状态
    bool m_isPlaying = false;
    bool m_isLooping = true;
    float m_playbackSpeed = 1.0f;

    // 关键帧编辑
    int m_selectedKeyframe = -1;
    KeyframeType m_selectedType = KeyframeType::Position;

    // 预览
    bool m_showPreview = true;
    float m_previewRotation = 0.0f;
};

} // namespace spark
