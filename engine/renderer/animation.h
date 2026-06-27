#pragma once

#include "math/math_types.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace spark {

// 最大骨骼数量
constexpr int MAX_BONES = 100;

// 骨骼信息
struct BoneInfo {
    int id = -1;
    Mat4 offset = Mat4(1.0f);
};

// 关键帧数据
struct KeyPosition {
    Vec3 position;
    float timeStamp;
};

struct KeyRotation {
    Quat orientation;
    float timeStamp;
};

struct KeyScale {
    Vec3 scale;
    float timeStamp;
};

// 骨骼类
class Bone {
public:
    Bone() = default;
    Bone(const std::string& name, int id, const std::vector<KeyPosition>& positions,
         const std::vector<KeyRotation>& rotations, const std::vector<KeyScale>& scales);

    void update(float animationTime);

    Mat4 getLocalTransform() const { return m_localTransform; }
    std::string getName() const { return m_name; }
    int getID() const { return m_id; }

private:
    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    Mat4 interpolatePosition(float animationTime);
    Mat4 interpolateRotation(float animationTime);
    Mat4 interpolateScaling(float animationTime);

    std::vector<KeyPosition> m_positions;
    std::vector<KeyRotation> m_rotations;
    std::vector<KeyScale> m_scales;
    int m_numPositions = 0, m_numRotations = 0, m_numScalings = 0;

    Mat4 m_localTransform = Mat4(1.0f);
    std::string m_name;
    int m_id = -1;
};

// 节点层次结构
struct AssimpNodeData {
    Mat4 transformation = Mat4(1.0f);
    std::string name;
    int childrenCount = 0;
    std::vector<AssimpNodeData> children;
};

// 动画类
class Animation {
public:
    Animation() = default;
    Animation(const std::string& animationPath);

    Bone* findBone(const std::string& name);

    float getTicksPerSecond() const { return m_ticksPerSecond; }
    float getDuration() const { return m_duration; }
    const AssimpNodeData& getRootNode() const { return m_rootNode; }
    const std::unordered_map<std::string, BoneInfo>& getBoneIDMap() const { return m_boneInfoMap; }

    bool isValid() const { return m_duration > 0.0f; }

private:
    float m_duration = 0.0f;
    float m_ticksPerSecond = 0.0f;
    std::vector<Bone> m_bones;
    AssimpNodeData m_rootNode;
    std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
};

// 动画混合状态
enum class AnimationBlendMode {
    Replace,    // 替换
    Additive    // 叠加
};

// 动画播放状态
struct AnimationState {
    Animation* animation = nullptr;
    float currentTime = 0.0f;
    float speed = 1.0f;
    float weight = 1.0f;
    bool looping = true;
    bool playing = true;
};

// 动画器类（支持混合）
class Animator {
public:
    Animator() = default;
    Animator(Animation* animation);

    void updateAnimation(float deltaTime);
    void playAnimation(Animation* animation);
    void blendAnimations(Animation* animA, Animation* animB, float blendFactor);

    void calculateBoneTransform(const AssimpNodeData* node, const Mat4& parentTransform);

    std::vector<Mat4> getFinalBoneMatrices() const { return m_finalBoneMatrices; }
    int getBoneCount() const { return static_cast<int>(m_finalBoneMatrices.size()); }

    // 播放控制
    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }
    void setLooping(bool loop) { m_looping = loop; }
    bool isLooping() const { return m_looping; }
    bool isPlaying() const { return m_playing; }
    void pause() { m_playing = false; }
    void resume() { m_playing = true; }

private:
    std::vector<Mat4> m_finalBoneMatrices;
    Animation* m_currentAnimation = nullptr;
    float m_currentTime = 0.0f;
    float m_deltaTime = 0.0f;
    float m_speed = 1.0f;
    bool m_looping = true;
    bool m_playing = true;

    // 混合状态
    AnimationState m_blendStateA;
    AnimationState m_blendStateB;
    float m_blendFactor = 0.0f;
    bool m_blending = false;
};

} // namespace spark
