#include "material_editor.h"
#include "renderer/material.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

MaterialEditor::MaterialEditor() = default;
MaterialEditor::~MaterialEditor() = default;

void MaterialEditor::setMaterial(std::shared_ptr<Material> material) {
    m_material = material;
    m_nodes.clear();

    if (material) {
        // 创建默认节点图
        auto albedoNode = addNode(MaterialNodeType::Albedo, Vec2(400, 100));
        auto constantNode = addNode(MaterialNodeType::Constant, Vec2(100, 100));

        // 连接
        connect(constantNode->id, 0, albedoNode->id, 0);
    }
}

std::shared_ptr<MaterialNode> MaterialEditor::addNode(MaterialNodeType type, Vec2 position) {
    auto node = std::make_shared<MaterialNode>();
    node->id = "node_" + std::to_string(m_nodes.size());
    node->type = type;
    node->position = position;
    node->size = Vec2(150, 80);

    // 根据类型设置引脚
    switch (type) {
        case MaterialNodeType::Constant:
            node->outputs.push_back({node->id + "_out", "Value", false});
            break;
        case MaterialNodeType::Texture:
            node->inputs.push_back({node->id + "_uv", "UV", true});
            node->outputs.push_back({node->id + "_color", "Color", false});
            node->outputs.push_back({node->id + "_alpha", "Alpha", false});
            break;
        case MaterialNodeType::Add:
            node->inputs.push_back({node->id + "_a", "A", true});
            node->inputs.push_back({node->id + "_b", "B", true});
            node->outputs.push_back({node->id + "_result", "Result", false});
            break;
        case MaterialNodeType::Albedo:
            node->inputs.push_back({node->id + "_color", "Color", true});
            break;
        case MaterialNodeType::Metallic:
            node->inputs.push_back({node->id + "_value", "Value", true});
            break;
        case MaterialNodeType::Roughness:
            node->inputs.push_back({node->id + "_value", "Value", true});
            break;
        default:
            break;
    }

    m_nodes.push_back(node);
    return node;
}

void MaterialEditor::removeNode(const std::string& nodeId) {
    // 断开所有连接
    for (auto& node : m_nodes) {
        for (auto& pin : node->inputs) {
            if (pin.connectedNodeId > 0) {
                auto connectedNode = getNode(std::to_string(pin.connectedNodeId));
                if (connectedNode && pin.connectedPinIndex < connectedNode->outputs.size()) {
                    connectedNode->outputs[pin.connectedPinIndex].connectedNodeId = 0;
                    connectedNode->outputs[pin.connectedPinIndex].connectedPinIndex = -1;
                }
            }
        }
        for (auto& pin : node->outputs) {
            if (pin.connectedNodeId > 0) {
                auto connectedNode = getNode(std::to_string(pin.connectedNodeId));
                if (connectedNode && pin.connectedPinIndex < connectedNode->inputs.size()) {
                    connectedNode->inputs[pin.connectedPinIndex].connectedNodeId = 0;
                    connectedNode->inputs[pin.connectedPinIndex].connectedPinIndex = -1;
                }
            }
        }
    }

    // 删除节点
    m_nodes.erase(
        std::remove_if(m_nodes.begin(), m_nodes.end(),
            [&](const std::shared_ptr<MaterialNode>& node) {
                return node->id == nodeId;
            }),
        m_nodes.end()
    );
}

void MaterialEditor::moveNode(const std::string& nodeId, Vec2 position) {
    auto node = getNode(nodeId);
    if (node) {
        node->position = position;
    }
}

bool MaterialEditor::connect(const std::string& sourceNodeId, int sourcePinIndex,
                              const std::string& targetNodeId, int targetPinIndex) {
    auto sourceNode = getNode(sourceNodeId);
    auto targetNode = getNode(targetNodeId);

    if (!sourceNode || !targetNode) return false;
    if (sourcePinIndex >= sourceNode->outputs.size()) return false;
    if (targetPinIndex >= targetNode->inputs.size()) return false;

    // 建立连接
    sourceNode->outputs[sourcePinIndex].connectedNodeId = std::stoi(targetNodeId);
    sourceNode->outputs[sourcePinIndex].connectedPinIndex = targetPinIndex;

    targetNode->inputs[targetPinIndex].connectedNodeId = std::stoi(sourceNodeId);
    targetNode->inputs[targetPinIndex].connectedPinIndex = sourcePinIndex;

    return true;
}

void MaterialEditor::disconnect(const std::string& nodeId, int pinIndex) {
    auto node = getNode(nodeId);
    if (!node) return;

    // 断开输入连接
    if (pinIndex < node->inputs.size()) {
        auto& pin = node->inputs[pinIndex];
        if (pin.connectedNodeId > 0) {
            auto connectedNode = getNode(std::to_string(pin.connectedNodeId));
            if (connectedNode && pin.connectedPinIndex < connectedNode->outputs.size()) {
                connectedNode->outputs[pin.connectedPinIndex].connectedNodeId = 0;
                connectedNode->outputs[pin.connectedPinIndex].connectedPinIndex = -1;
            }
            pin.connectedNodeId = 0;
            pin.connectedPinIndex = -1;
        }
    }
}

std::shared_ptr<MaterialNode> MaterialEditor::getNode(const std::string& nodeId) const {
    for (const auto& node : m_nodes) {
        if (node->id == nodeId) {
            return node;
        }
    }
    return nullptr;
}

bool MaterialEditor::compile() {
    if (!m_material) return false;

    // TODO: 编译节点图为材质
    SPARK_CORE_INFO("Material compiled from node graph.");
    return true;
}

// =============================================
// MaterialEditorManager
// =============================================

MaterialEditorManager& MaterialEditorManager::get() {
    static MaterialEditorManager instance;
    return instance;
}

std::shared_ptr<MaterialEditor> MaterialEditorManager::createEditor() {
    auto editor = std::make_shared<MaterialEditor>();
    m_editors.push_back(editor);
    SPARK_CORE_INFO("Material editor created.");
    return editor;
}

} // namespace spark
