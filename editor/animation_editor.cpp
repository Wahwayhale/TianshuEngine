#include "animation_editor.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

AnimationEditor::AnimationEditor() = default;
AnimationEditor::~AnimationEditor() = default;

void AnimationEditor::setAnimation(std::shared_ptr<Animation> animation) {
    m_animation = animation;
    if (animation) {
        // TODO: 从动画中提取轨道和关键帧
    }
}

void AnimationEditor::play() {
    m_playing = true;
    m_paused = false;
}

void AnimationEditor::pause() {
    m_paused = true;
}

void AnimationEditor::stop() {
    m_playing = false;
    m_paused = false;
    m_currentTime = 0.0f;
}

void AnimationEditor::setCurrentTime(float time) {
    m_currentTime = std::clamp(time, 0.0f, m_duration);
}

void AnimationEditor::addKeyframe(const std::string& trackName, const Keyframe& keyframe) {
    for (auto& track : m_tracks) {
        if (track.name == trackName) {
            track.keyframes.push_back(keyframe);

            // 按时间排序
            std::sort(track.keyframes.begin(), track.keyframes.end(),
                [](const Keyframe& a, const Keyframe& b) {
                    return a.time < b.time;
                });

            SPARK_CORE_INFO("Keyframe added to track: {0} at time {1}", trackName, keyframe.time);
            return;
        }
    }

    SPARK_CORE_WARN("Track not found: {0}", trackName);
}

void AnimationEditor::removeKeyframe(const std::string& trackName, float time) {
    for (auto& track : m_tracks) {
        if (track.name == trackName) {
            track.keyframes.erase(
                std::remove_if(track.keyframes.begin(), track.keyframes.end(),
                    [time](const Keyframe& kf) {
                        return std::abs(kf.time - time) < 0.001f;
                    }),
                track.keyframes.end()
            );

            SPARK_CORE_INFO("Keyframe removed from track: {0} at time {1}", trackName, time);
            return;
        }
    }
}

void AnimationEditor::moveKeyframe(const std::string& trackName, float oldTime, float newTime) {
    for (auto& track : m_tracks) {
        if (track.name == trackName) {
            for (auto& kf : track.keyframes) {
                if (std::abs(kf.time - oldTime) < 0.001f) {
                    kf.time = newTime;
                    break;
                }
            }

            // 重新排序
            std::sort(track.keyframes.begin(), track.keyframes.end(),
                [](const Keyframe& a, const Keyframe& b) {
                    return a.time < b.time;
                });

            SPARK_CORE_INFO("Keyframe moved in track: {0} from {1} to {2}", trackName, oldTime, newTime);
            return;
        }
    }
}

void AnimationEditor::addTrack(const AnimationTrack& track) {
    m_tracks.push_back(track);
    SPARK_CORE_INFO("Animation track added: {0}", track.name);
}

void AnimationEditor::removeTrack(const std::string& trackName) {
    m_tracks.erase(
        std::remove_if(m_tracks.begin(), m_tracks.end(),
            [&](const AnimationTrack& track) {
                return track.name == trackName;
            }),
        m_tracks.end()
    );
}

// =============================================
// AnimationEditorManager
// =============================================

AnimationEditorManager& AnimationEditorManager::get() {
    static AnimationEditorManager instance;
    return instance;
}

std::shared_ptr<AnimationEditor> AnimationEditorManager::createEditor() {
    auto editor = std::make_shared<AnimationEditor>();
    m_editors.push_back(editor);
    SPARK_CORE_INFO("Animation editor created.");
    return editor;
}

} // namespace spark
