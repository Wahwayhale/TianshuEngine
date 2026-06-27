#include "audio_system.h"
#include "audio_engine.h"
#include "audio_source.h"
#include "renderer/camera.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "core/log.h"

namespace spark {

AudioSystem::AudioSystem() {
    // 初始化音频引擎
    AudioEngine::get().initialize();
    SPARK_CORE_INFO("Audio system initialized.");
}

AudioSystem::~AudioSystem() {
    // 清理所有音频源
    m_activeSources.clear();

    // 关闭音频引擎
    AudioEngine::get().shutdown();
}

void AudioSystem::update(Scene& scene, float deltaTime) {
    // 更新监听器位置（相机）
    if (m_camera) {
        AudioEngine::get().setListenerPosition(m_camera->getPosition());
        AudioEngine::get().setListenerOrientation(m_camera->getFront(), m_camera->getUp());
    }

    // 遍历所有有 AudioSourceComponent 的实体
    scene.view<TransformComponent, AudioSourceComponent>([&](Entity& entity) {
        auto& transform = entity.getComponent<TransformComponent>();
        auto& audioComp = entity.getComponent<AudioSourceComponent>();

        // 首次更新时创建音频源
        if (!audioComp.source && !audioComp.audioPath.empty()) {
            audioComp.source = AudioEngine::get().createSource(audioComp.audioPath);
            if (audioComp.source) {
                audioComp.source->setVolume(audioComp.volume);
                audioComp.source->setPitch(audioComp.pitch);
                audioComp.source->setLooping(audioComp.looping);
                audioComp.source->setRange(audioComp.range);

                if (audioComp.playOnStart) {
                    audioComp.source->play();
                    audioComp.isPlaying = true;
                }

                m_activeSources[entity.getID()] = audioComp.source;
            }
        }

        // 更新音频源位置
        if (audioComp.source) {
            audioComp.source->setPosition(transform.position);
            audioComp.source->setVolume(audioComp.volume);
            audioComp.source->setPitch(audioComp.pitch);
            audioComp.source->setLooping(audioComp.looping);
            audioComp.source->setRange(audioComp.range);

            // 同步播放状态
            if (audioComp.isPlaying && !audioComp.source->isPlaying()) {
                audioComp.isPlaying = false;
            }
        }
    });
}

} // namespace spark
