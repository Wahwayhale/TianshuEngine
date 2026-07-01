#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace spark {

class Device;
class Buffer;

// 骨骼信息
struct BoneInfo {
    std::string name;
    int id = -1;
    Mat4 offset = Mat4(1.0f);
};

// 骨骼
struct Bone {
    std::string name;
    int id = -1;
    Mat4 localTransform = Mat4(1.0f);
    Mat4 globalTransform = Mat4(1.0f);
    int parentIndex = -1;
    std::vector<int> children;
};

// 蒙皮顶点
struct SkinnedVertex {
    float position[3];
    float normal[3];
    float texCoord[2];
    float weights[4];
    uint32_t boneIndices[4];
};

// 骨骼动画关键帧
struct BoneKeyframe {
    float time;
    Vec3 position;
    Quat rotation;
    Vec3 scale;
};

// 骨骼动画通道
struct BoneAnimationChannel {
    std::string boneName;
    std::vector<BoneKeyframe> keyframes;
};

// 骨骼动画
class BoneAnimation {
public:
    BoneAnimation(const std::string& name = "");
    ~BoneAnimation();

    // 添加通道
    void addChannel(const BoneAnimationChannel& channel);

    // 获取变换
    Mat4 getBoneTransform(const std::string& boneName, float time) const;

    // 属性
    const std::string& getName() const { return m_name; }
    float getDuration() const { return m_duration; }
    void setDuration(float duration) { m_duration = duration; }

private:
    std::string m_name;
    float m_duration = 1.0f;
    std::unordered_map<std::string, BoneAnimationChannel> m_channels;
};

// 骨骼网格
class SkeletalMesh {
public:
    SkeletalMesh(Device& device);
    ~SkeletalMesh();

    // 设置骨骼
    void setBones(const std::vector<Bone>& bones);

    // 设置蒙皮顶点
    void setVertices(const std::vector<SkinnedVertex>& vertices);

    // 更新骨骼矩阵
    void updateBoneMatrices(const std::vector<Mat4>& boneMatrices);

    // 渲染
    void render(VkCommandBuffer commandBuffer);

    // 获取骨骼信息
    const std::vector<Bone>& getBones() const { return m_bones; }
    const std::vector<Mat4>& getBoneMatrices() const { return m_boneMatrices; }

private:
    Device& m_device;

    std::vector<Bone> m_bones;
    std::vector<Mat4> m_boneMatrices;
    std::vector<SkinnedVertex> m_vertices;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_boneBuffer;
};

} // namespace spark
