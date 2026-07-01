#pragma once

#include "ecs/system.h"
#include "ecs/components.h"
#include "animation.h"
#include <memory>
#include <unordered_map>

namespace spark {

// 动画组件
struct AnimatorComponent : public Component {
    std::string animationPath;           // 动画文件路径
    float speed = 1.0f;                  // 播放速度
    bool looping = true;                 // 循环播放
    bool playOnStart = true;             // 开始时自动播放

    // 运行时状态
    std::shared_ptr<Animation> animation;
    std::shared_ptr<Animator> animator;
    bool isPlaying = false;
    bool initialized = false;

    // 骨骼矩阵（传给着色器）
    std::vector<Mat4> boneMatrices;
};

// 动画系统
class AnimationSystem : public System {
public:
    AnimationSystem();
    ~AnimationSystem() override;

    void update(Scene& scene, float deltaTime) override;

    // 加载动画
    std::shared_ptr<Animation> loadAnimation(const std::string& path);

private:
    std::unordered_map<std::string, std::shared_ptr<Animation>> m_animationCache;
};

} // namespace spark
