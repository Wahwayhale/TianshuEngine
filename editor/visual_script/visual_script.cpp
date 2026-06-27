#include "visual_script.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

// =============================================
// VisualScript
// =============================================

VisualScript::VisualScript(const std::string& name) : m_name(name) {
    // 添加开始节点
    auto startNode = addNode(NodeType::Start, Vec2(100, 100));
}

VisualScript::~VisualScript() = default;

std::shared_ptr<VisualNode> VisualScript::addNode(NodeType type, Vec2 position) {
    auto node = std::make_shared<VisualNode>();
    node->id = "node_" + std::to_string(m_nodes.size());
    node->type = type;
    node->position = position;

    // 根据类型设置节点属性
    switch (type) {
        case NodeType::Start:
            node->name = "Start";
            node->outputs.push_back({node->id + "_out", "Flow", PinType::Flow, false});
            break;

        case NodeType::Update:
            node->name = "Update";
            node->outputs.push_back({node->id + "_out", "Flow", PinType::Flow, false});
            node->outputs.push_back({node->id + "_dt", "Delta Time", PinType::Float, false});
            break;

        case NodeType::If:
            node->name = "If";
            node->inputs.push_back({node->id + "_in", "Flow", PinType::Flow, true});
            node->inputs.push_back({node->id + "_cond", "Condition", PinType::Bool, true});
            node->outputs.push_back({node->id + "_true", "True", PinType::Flow, false});
            node->outputs.push_back({node->id + "_false", "False", PinType::Flow, false});
            break;

        case NodeType::Add:
            node->name = "Add";
            node->inputs.push_back({node->id + "_a", "A", PinType::Float, true});
            node->inputs.push_back({node->id + "_b", "B", PinType::Float, true});
            node->outputs.push_back({node->id + "_result", "Result", PinType::Float, false});
            break;

        case NodeType::SetPosition:
            node->name = "Set Position";
            node->inputs.push_back({node->id + "_in", "Flow", PinType::Flow, true});
            node->inputs.push_back({node->id + "_obj", "Object", PinType::Object, true});
            node->inputs.push_back({node->id + "_pos", "Position", PinType::Vec3, true});
            node->outputs.push_back({node->id + "_out", "Flow", PinType::Flow, false});
            break;

        case NodeType::GetPosition:
            node->name = "Get Position";
            node->inputs.push_back({node->id + "_obj", "Object", PinType::Object, true});
            node->outputs.push_back({node->id + "_pos", "Position", PinType::Vec3, false});
            break;

        case NodeType::IsKeyPressed:
            node->name = "Is Key Pressed";
            node->inputs.push_back({node->id + "_key", "Key", PinType::Int, true});
            node->outputs.push_back({node->id + "_pressed", "Pressed", PinType::Bool, false});
            break;

        case NodeType::Log:
            node->name = "Log";
            node->inputs.push_back({node->id + "_in", "Flow", PinType::Flow, true});
            node->inputs.push_back({node->id + "_msg", "Message", PinType::String, true});
            node->outputs.push_back({node->id + "_out", "Flow", PinType::Flow, false});
            break;

        case NodeType::Float:
            node->name = "Float";
            node->outputs.push_back({node->id + "_value", "Value", PinType::Float, false});
            node->floatValue = 0.0f;
            break;

        case NodeType::Vec3:
            node->name = "Vec3";
            node->inputs.push_back({node->id + "_x", "X", PinType::Float, true});
            node->inputs.push_back({node->id + "_y", "Y", PinType::Float, true});
            node->inputs.push_back({node->id + "_z", "Z", PinType::Float, true});
            node->outputs.push_back({node->id + "_vec", "Vector", PinType::Vec3, false});
            break;

        case NodeType::Translate:
            node->name = "Translate";
            node->inputs.push_back({node->id + "_in", "Flow", PinType::Flow, true});
            node->inputs.push_back({node->id + "_obj", "Object", PinType::Object, true});
            node->inputs.push_back({node->id + "_offset", "Offset", PinType::Vec3, true});
            node->outputs.push_back({node->id + "_out", "Flow", PinType::Flow, false});
            break;

        default:
            node->name = "Unknown";
            break;
    }

    node->size = Vec2(150, 50 + node->inputs.size() * 20 + node->outputs.size() * 20);

    m_nodes.push_back(node);
    return node;
}

void VisualScript::removeNode(const std::string& nodeId) {
    // 删除相关连接
    m_connections.erase(
        std::remove_if(m_connections.begin(), m_connections.end(),
            [&](const Connection& conn) {
                return conn.sourceNodeId == nodeId || conn.targetNodeId == nodeId;
            }),
        m_connections.end()
    );

    // 删除节点
    m_nodes.erase(
        std::remove_if(m_nodes.begin(), m_nodes.end(),
            [&](const std::shared_ptr<VisualNode>& node) {
                return node->id == nodeId;
            }),
        m_nodes.end()
    );
}

std::shared_ptr<VisualNode> VisualScript::getNode(const std::string& nodeId) const {
    for (const auto& node : m_nodes) {
        if (node->id == nodeId) {
            return node;
        }
    }
    return nullptr;
}

bool VisualScript::connect(const std::string& sourceNodeId, const std::string& sourcePinId,
                            const std::string& targetNodeId, const std::string& targetPinId) {
    // 检查连接是否有效
    auto sourceNode = getNode(sourceNodeId);
    auto targetNode = getNode(targetNodeId);

    if (!sourceNode || !targetNode) {
        return false;
    }

    // 检查引脚类型兼容性
    Pin* sourcePin = nullptr;
    Pin* targetPin = nullptr;

    for (auto& pin : sourceNode->outputs) {
        if (pin.id == sourcePinId) {
            sourcePin = &pin;
            break;
        }
    }

    for (auto& pin : targetNode->inputs) {
        if (pin.id == targetPinId) {
            targetPin = &pin;
            break;
        }
    }

    if (!sourcePin || !targetPin) {
        return false;
    }

    if (sourcePin->type != targetPin->type) {
        return false;
    }

    // 创建连接
    Connection connection;
    connection.id = "conn_" + std::to_string(m_connections.size());
    connection.sourceNodeId = sourceNodeId;
    connection.sourcePinId = sourcePinId;
    connection.targetNodeId = targetNodeId;
    connection.targetPinId = targetPinId;

    m_connections.push_back(connection);

    // 更新引脚连接
    sourcePin->connectedPinId = targetPinId;
    targetPin->connectedPinId = sourcePinId;

    return true;
}

void VisualScript::disconnect(const std::string& connectionId) {
    m_connections.erase(
        std::remove_if(m_connections.begin(), m_connections.end(),
            [&](const Connection& conn) {
                return conn.id == connectionId;
            }),
        m_connections.end()
    );
}

void VisualScript::execute() {
    // 找到开始节点
    std::shared_ptr<VisualNode> startNode;
    for (const auto& node : m_nodes) {
        if (node->type == NodeType::Start) {
            startNode = node;
            break;
        }
    }

    if (startNode) {
        executeNode(startNode);
    }
}

void VisualScript::executeNode(std::shared_ptr<VisualNode> node) {
    // 执行当前节点
    switch (node->type) {
        case NodeType::Start:
        case NodeType::Update:
            // 流程控制：继续执行连接的下一个节点
            break;

        case NodeType::Log:
            // 输出日志
            break;

        default:
            break;
    }

    // 执行连接的下一个节点
    for (const auto& conn : m_connections) {
        if (conn.sourceNodeId == node->id) {
            auto nextNode = getNode(conn.targetNodeId);
            if (nextNode) {
                executeNode(nextNode);
            }
        }
    }
}

// =============================================
// VisualScriptManager
// =============================================

VisualScriptManager& VisualScriptManager::get() {
    static VisualScriptManager instance;
    return instance;
}

std::shared_ptr<VisualScript> VisualScriptManager::createScript(const std::string& name) {
    auto script = std::make_shared<VisualScript>(name);
    m_scripts[name] = script;
    SPARK_CORE_INFO("Created visual script: {0}", name);
    return script;
}

void VisualScriptManager::deleteScript(const std::string& name) {
    m_scripts.erase(name);
}

std::shared_ptr<VisualScript> VisualScriptManager::getScript(const std::string& name) const {
    auto it = m_scripts.find(name);
    if (it != m_scripts.end()) {
        return it->second;
    }
    return nullptr;
}

void VisualScriptManager::executeAll() {
    for (auto& [name, script] : m_scripts) {
        script->execute();
    }
}

} // namespace spark
