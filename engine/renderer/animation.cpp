#include "animation.h"
#include "core/log.h"

// 使用 tinygltf 加载动画（与引擎其他部分一致）
#include <tiny_gltf.h>

namespace spark {

// =============================================
// Bone
// =============================================

Bone::Bone(const std::string& name, int id, const std::vector<KeyPosition>& positions,
           const std::vector<KeyRotation>& rotations, const std::vector<KeyScale>& scales)
    : m_name(name), m_id(id), m_positions(positions), m_rotations(rotations), m_scales(scales) {
    m_numPositions = static_cast<int>(positions.size());
    m_numRotations = static_cast<int>(rotations.size());
    m_numScalings = static_cast<int>(scales.size());
}

void Bone::update(float animationTime) {
    Mat4 translation = interpolatePosition(animationTime);
    Mat4 rotation = interpolateRotation(animationTime);
    Mat4 scale = interpolateScaling(animationTime);
    m_localTransform = translation * rotation * scale;
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
}

Mat4 Bone::interpolatePosition(float animationTime) {
    if (m_numPositions == 0) return Mat4(1.0f);
    if (m_numPositions == 1) return glm::translate(Mat4(1.0f), m_positions[0].position);

    int p0Index = 0;
    for (int i = 0; i < m_numPositions - 1; i++) {
        if (animationTime < m_positions[i + 1].timeStamp) {
            p0Index = i;
            break;
        }
    }

    int p1Index = p0Index + 1;
    if (p1Index >= m_numPositions) p1Index = m_numPositions - 1;

    float scaleFactor = getScaleFactor(m_positions[p0Index].timeStamp,
                                        m_positions[p1Index].timeStamp, animationTime);
    Vec3 finalPosition = glm::mix(m_positions[p0Index].position, m_positions[p1Index].position, scaleFactor);
    return glm::translate(Mat4(1.0f), finalPosition);
}

Mat4 Bone::interpolateRotation(float animationTime) {
    if (m_numRotations == 0) return Mat4(1.0f);
    if (m_numRotations == 1) return glm::mat4_cast(glm::normalize(m_rotations[0].orientation));

    int p0Index = 0;
    for (int i = 0; i < m_numRotations - 1; i++) {
        if (animationTime < m_rotations[i + 1].timeStamp) {
            p0Index = i;
            break;
        }
    }

    int p1Index = p0Index + 1;
    if (p1Index >= m_numRotations) p1Index = m_numRotations - 1;

    float scaleFactor = getScaleFactor(m_rotations[p0Index].timeStamp,
                                        m_rotations[p1Index].timeStamp, animationTime);
    Quat finalRotation = glm::slerp(m_rotations[p0Index].orientation, m_rotations[p1Index].orientation, scaleFactor);
    return glm::mat4_cast(glm::normalize(finalRotation));
}

Mat4 Bone::interpolateScaling(float animationTime) {
    if (m_numScalings == 0) return Mat4(1.0f);
    if (m_numScalings == 1) return glm::scale(Mat4(1.0f), m_scales[0].scale);

    int p0Index = 0;
    for (int i = 0; i < m_numScalings - 1; i++) {
        if (animationTime < m_scales[i + 1].timeStamp) {
            p0Index = i;
            break;
        }
    }

    int p1Index = p0Index + 1;
    if (p1Index >= m_numScalings) p1Index = m_numScalings - 1;

    float scaleFactor = getScaleFactor(m_scales[p0Index].timeStamp,
                                        m_scales[p1Index].timeStamp, animationTime);
    Vec3 finalScale = glm::mix(m_scales[p0Index].scale, m_scales[p1Index].scale, scaleFactor);
    return glm::scale(Mat4(1.0f), finalScale);
}

// =============================================
// Animation
// =============================================

Animation::Animation(const std::string& animationPath) {
    // 使用 tinygltf 加载动画
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool result;
    if (animationPath.ends_with(".glb")) {
        result = loader.LoadBinaryFromFile(&model, &err, &warn, animationPath);
    } else {
        result = loader.LoadASCIIFromFile(&model, &err, &warn, animationPath);
    }

    if (!result) {
        SPARK_CORE_ERROR("Failed to load animation: {0} - {1}", animationPath, err);
        return;
    }

    if (!warn.empty()) {
        SPARK_CORE_WARN("Animation loading warning: {0}", warn);
    }

    // 加载第一个动画
    if (model.animations.empty()) {
        SPARK_CORE_WARN("No animations found in: {0}", animationPath);
        return;
    }

    const auto& anim = model.animations[0];
    m_duration = static_cast<float>(anim.channels.end()->target_node);  // 简化
    m_ticksPerSecond = 30.0f;  // 默认 30fps

    // 加载骨骼信息
    for (const auto& skin : model.skins) {
        for (size_t i = 0; i < skin.joints.size(); i++) {
            int jointIndex = skin.joints[i];
            const auto& node = model.nodes[jointIndex];

            BoneInfo info;
            info.id = static_cast<int>(i);

            // 获取逆绑定矩阵
            if (skin.inverseBindMatrices >= 0) {
                const auto& accessor = model.accessors[skin.inverseBindMatrices];
                const auto& bufferView = model.bufferViews[accessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];

                if (i < accessor.count) {
                    const float* matData = reinterpret_cast<const float*>(
                        &buffer.data[accessor.byteOffset + bufferView.byteOffset + i * 16 * sizeof(float)]);

                    for (int col = 0; col < 4; col++) {
                        for (int row = 0; row < 4; row++) {
                            info.offset[col][row] = matData[col * 4 + row];
                        }
                    }
                }
            }

            m_boneInfoMap[node.name] = info;
        }
    }

    // 加载动画通道
    for (const auto& channel : anim.channels) {
        const auto& sampler = anim.samplers[channel.sampler];

        // 读取时间戳
        const auto& timeAccessor = model.accessors[sampler.input];
        const auto& timeBufferView = model.bufferViews[timeAccessor.bufferView];
        const auto& timeBuffer = model.buffers[timeBufferView.buffer];
        const float* timestamps = reinterpret_cast<const float*>(
            &timeBuffer.data[timeAccessor.byteOffset + timeBufferView.byteOffset]);

        // 读取数据
        const auto& valueAccessor = model.accessors[sampler.output];
        const auto& valueBufferView = model.bufferViews[valueAccessor.bufferView];
        const auto& valueBuffer = model.buffers[valueBufferView.buffer];

        std::vector<KeyPosition> positions;
        std::vector<KeyRotation> rotations;
        std::vector<KeyScale> scales;

        const std::string& nodeName = model.nodes[channel.target_node].name;

        if (channel.target_path == "translation") {
            const float* values = reinterpret_cast<const float*>(
                &valueBuffer.data[valueAccessor.byteOffset + valueBufferView.byteOffset]);

            for (size_t i = 0; i < timeAccessor.count; i++) {
                KeyPosition key;
                key.timeStamp = timestamps[i];
                key.position = Vec3(values[i * 3], values[i * 3 + 1], values[i * 3 + 2]);
                positions.push_back(key);
            }
        } else if (channel.target_path == "rotation") {
            const float* values = reinterpret_cast<const float*>(
                &valueBuffer.data[valueAccessor.byteOffset + valueBufferView.byteOffset]);

            for (size_t i = 0; i < timeAccessor.count; i++) {
                KeyRotation key;
                key.timeStamp = timestamps[i];
                key.orientation = Quat(values[i * 4 + 3], values[i * 4], values[i * 4 + 1], values[i * 4 + 2]);
                rotations.push_back(key);
            }
        } else if (channel.target_path == "scale") {
            const float* values = reinterpret_cast<const float*>(
                &valueBuffer.data[valueAccessor.byteOffset + valueBufferView.byteOffset]);

            for (size_t i = 0; i < timeAccessor.count; i++) {
                KeyScale key;
                key.timeStamp = timestamps[i];
                key.scale = Vec3(values[i * 3], values[i * 3 + 1], values[i * 3 + 2]);
                scales.push_back(key);
            }
        }

        // 创建或更新骨骼
        if (m_boneInfoMap.find(nodeName) != m_boneInfoMap.end()) {
            int boneId = m_boneInfoMap[nodeName].id;
            m_bones.emplace_back(nodeName, boneId, positions, rotations, scales);
        }
    }

    // 设置根节点
    m_rootNode.name = model.nodes[0].name;
    m_rootNode.transformation = Mat4(1.0f);
    m_rootNode.childrenCount = 0;

    SPARK_CORE_INFO("Animation loaded: {0} (duration: {1:.2f}s, bones: {2})",
                     animationPath, m_duration / m_ticksPerSecond, m_bones.size());
}

Bone* Animation::findBone(const std::string& name) {
    for (auto& bone : m_bones) {
        if (bone.getName() == name) {
            return &bone;
        }
    }
    return nullptr;
}

// =============================================
// Animator
// =============================================

Animator::Animator(Animation* animation) {
    m_currentTime = 0.0f;
    m_currentAnimation = animation;
    m_finalBoneMatrices.resize(MAX_BONES, Mat4(1.0f));
}

void Animator::updateAnimation(float deltaTime) {
    if (!m_currentAnimation || !m_playing) return;

    m_deltaTime = deltaTime;
    m_currentTime += m_currentAnimation->getTicksPerSecond() * deltaTime * m_speed;

    if (m_looping) {
        m_currentTime = fmod(m_currentTime, m_currentAnimation->getDuration());
    } else {
        if (m_currentTime >= m_currentAnimation->getDuration()) {
            m_currentTime = m_currentAnimation->getDuration();
            m_playing = false;
        }
    }

    calculateBoneTransform(&m_currentAnimation->getRootNode(), Mat4(1.0f));
}

void Animator::playAnimation(Animation* animation) {
    m_currentAnimation = animation;
    m_currentTime = 0.0f;
    m_playing = true;
}

void Animator::blendAnimations(Animation* animA, Animation* animB, float blendFactor) {
    // 简化实现：实际应该分别计算两套骨骼矩阵然后插值
    m_blendStateA.animation = animA;
    m_blendStateB.animation = animB;
    m_blendFactor = blendFactor;
    m_blending = true;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, const Mat4& parentTransform) {
    std::string nodeName = node->name;
    Mat4 nodeTransform = node->transformation;

    Bone* bone = m_currentAnimation->findBone(nodeName);
    if (bone) {
        bone->update(m_currentTime);
        nodeTransform = bone->getLocalTransform();
    }

    Mat4 globalTransformation = parentTransform * nodeTransform;

    auto& boneInfoMap = m_currentAnimation->getBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap.at(nodeName).id;
        Mat4 offset = boneInfoMap.at(nodeName).offset;
        if (index < MAX_BONES) {
            m_finalBoneMatrices[index] = globalTransformation * offset;
        }
    }

    for (int i = 0; i < node->childrenCount; i++) {
        calculateBoneTransform(&node->children[i], globalTransformation);
    }
}

} // namespace spark
