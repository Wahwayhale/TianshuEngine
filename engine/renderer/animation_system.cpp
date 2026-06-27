#include "animation_system.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "core/log.h"

namespace spark {

AnimationSystem::AnimationSystem() {
    SPARK_CORE_INFO("Animation system initialized.");
}

AnimationSystem::~AnimationSystem() = default;

void AnimationSystem::update(Scene& scene, float deltaTime) {
    // 遍历所有有 AnimatorComponent 的实体
    scene.view<TransformComponent, AnimatorComponent>([&](Entity& entity) {
        auto& animatorComp = entity.getComponent<AnimatorComponent>();

        // 首次初始化
        if (!animatorComp.initialized) {
            if (!animatorComp.animationPath.empty()) {
                animatorComp.animation = loadAnimation(animatorComp.animationPath);
                if (animatorComp.animation) {
                    animatorComp.animator = std::make_shared<Animator>(animatorComp.animation.get());
                    animatorComp.animator->setSpeed(animatorComp.speed);
                    animatorComp.animator->setLooping(animatorComp.looping);

                    if (animatorComp.playOnStart) {
                        animatorComp.animator->playAnimation(animatorComp.animation.get());
                        animatorComp.isPlaying = true;
                    }

                    // 初始化骨骼矩阵
                    animatorComp.boneMatrices.resize(MAX_BONES, Mat4(1.0f));
                }
                animatorComp.initialized = true;
            }
        }

        // 更新动画
        if (animatorComp.animator && animatorComp.isPlaying) {
            animatorComp.animator->updateAnimation(deltaTime);

            // 获取骨骼矩阵
            auto matrices = animatorComp.animator->getFinalBoneMatrices();
            size_t count = std::min(matrices.size(), animatorComp.boneMatrices.size());
            for (size_t i = 0; i < count; i++) {
                animatorComp.boneMatrices[i] = matrices[i];
            }
        }
    });
}

std::shared_ptr<Animation> AnimationSystem::loadAnimation(const std::string& path) {
    // 检查缓存
    auto it = m_animationCache.find(path);
    if (it != m_animationCache.end()) {
        return it->second;
    }

    // 加载动画
    auto animation = std::make_shared<Animation>(path);
    if (animation->isValid()) {
        m_animationCache[path] = animation;
        return animation;
    }

    return nullptr;
}

} // namespace spark
