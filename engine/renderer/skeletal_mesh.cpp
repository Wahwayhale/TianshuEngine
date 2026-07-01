#include "skeletal_mesh.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

// =============================================
// BoneAnimation
// =============================================

BoneAnimation::BoneAnimation(const std::string& name) : m_name(name) {}
BoneAnimation::~BoneAnimation() = default;

void BoneAnimation::addChannel(const BoneAnimationChannel& channel) {
    m_channels[channel.boneName] = channel;
}

Mat4 BoneAnimation::getBoneTransform(const std::string& boneName, float time) const {
    auto it = m_channels.find(boneName);
    if (it == m_channels.end()) {
        return Mat4(1.0f);
    }

    const auto& channel = it->second;
    if (channel.keyframes.empty()) {
        return Mat4(1.0f);
    }

    // 找到当前时间的关键帧
    int keyframeIndex = 0;
    for (size_t i = 0; i < channel.keyframes.size() - 1; i++) {
        if (time < channel.keyframes[i + 1].time) {
            keyframeIndex = static_cast<int>(i);
            break;
        }
    }

    // 插值
    const auto& kf1 = channel.keyframes[keyframeIndex];
    const auto& kf2 = channel.keyframes[std::min(keyframeIndex + 1, static_cast<int>(channel.keyframes.size()) - 1)];

    float t = 0.0f;
    if (kf2.time > kf1.time) {
        t = (time - kf1.time) / (kf2.time - kf1.time);
    }

    // 位置插值
    Vec3 position = glm::mix(kf1.position, kf2.position, t);

    // 旋转插值
    Quat rotation = glm::slerp(kf1.rotation, kf2.rotation, t);

    // 缩放插值
    Vec3 scale = glm::mix(kf1.scale, kf2.scale, t);

    // 构建变换矩阵
    Mat4 transform = glm::translate(Mat4(1.0f), position);
    transform *= glm::mat4_cast(rotation);
    transform = glm::scale(transform, scale);

    return transform;
}

// =============================================
// SkeletalMesh
// =============================================

SkeletalMesh::SkeletalMesh(Device& device) : m_device(device) {}
SkeletalMesh::~SkeletalMesh() = default;

void SkeletalMesh::setBones(const std::vector<Bone>& bones) {
    m_bones = bones;
    m_boneMatrices.resize(bones.size(), Mat4(1.0f));

    // 创建骨骼缓冲
    VkDeviceSize bufferSize = sizeof(Mat4) * m_bones.size();
    m_boneBuffer = std::make_unique<Buffer>(
        m_device, bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_boneBuffer->map();

    SPARK_CORE_INFO("Skeletal mesh: {0} bones", bones.size());
}

void SkeletalMesh::setVertices(const std::vector<SkinnedVertex>& vertices) {
    m_vertices = vertices;

    // 创建顶点缓冲
    VkDeviceSize bufferSize = sizeof(SkinnedVertex) * vertices.size();
    m_vertexBuffer = std::make_unique<Buffer>(
        m_device, bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_vertexBuffer->copyTo(vertices.data(), bufferSize);

    SPARK_CORE_INFO("Skeletal mesh: {0} vertices", vertices.size());
}

void SkeletalMesh::updateBoneMatrices(const std::vector<Mat4>& boneMatrices) {
    if (boneMatrices.size() != m_boneMatrices.size()) return;

    m_boneMatrices = boneMatrices;

    // 更新 GPU 缓冲
    if (m_boneBuffer) {
        m_boneBuffer->copyTo(m_boneMatrices.data(), sizeof(Mat4) * m_boneMatrices.size());
    }
}

void SkeletalMesh::render(VkCommandBuffer commandBuffer) {
    if (!m_vertexBuffer || m_vertices.empty()) return;

    // 绑定顶点缓冲
    VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // 绘制
    vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_vertices.size()), 1, 0, 0);
}

} // namespace spark
