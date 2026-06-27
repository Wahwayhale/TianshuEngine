#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include "math/math_types.h"

namespace spark {

// 节点类型
enum class NodeType {
    // 流程控制
    Start,
    Update,
    If,
    ForLoop,
    WhileLoop,
    Sequence,
    Branch,

    // 数学
    Add,
    Subtract,
    Multiply,
    Divide,
    Sin,
    Cos,
    Lerp,
    Clamp,
    Random,

    // 向量
    Vec3Create,
    Vec3Add,
    Vec3Multiply,
    Vec3Normalize,
    Vec3Length,
    Vec3Dot,
    Vec3Cross,

    // 变换
    GetPosition,
    SetPosition,
    GetRotation,
    SetRotation,
    GetScale,
    SetScale,
    Translate,
    Rotate,

    // 输入
    IsKeyPressed,
    GetMousePosition,
    IsMouseButtonPressed,

    // 渲染
    DrawLine,
    DrawSphere,
    DrawBox,

    // 逻辑
    And,
    Or,
    Not,
    Equal,
    Greater,
    Less,

    // 值
    Float,
    Int,
    Bool,
    String,
    Vec3,

    // 事件
    OnCollision,
    OnTrigger,
    OnTimer,

    // 动作
    Log,
    SpawnObject,
    DestroyObject,
    PlaySound
};

// 引脚类型
enum class PinType {
    Flow,       // 执行流
    Bool,
    Int,
    Float,
    Vec3,
    String,
    Object
};

// 引脚
struct Pin {
    std::string id;
    std::string name;
    PinType type;
    bool isInput;
    std::string connectedPinId;
};

// 节点
struct VisualNode {
    std::string id;
    std::string name;
    NodeType type;
    Vec2 position;
    Vec2 size;

    std::vector<Pin> inputs;
    std::vector<Pin> outputs;

    // 节点数据
    float floatValue = 0.0f;
    int intValue = 0;
    bool boolValue = false;
    std::string stringValue;
    Vec3 vec3Value = Vec3(0.0f);
};

// 连接
struct Connection {
    std::string id;
    std::string sourceNodeId;
    std::string sourcePinId;
    std::string targetNodeId;
    std::string targetPinId;
};

// 可视化脚本
class VisualScript {
public:
    VisualScript(const std::string& name = "Untitled");
    ~VisualScript();

    // 节点管理
    std::shared_ptr<VisualNode> addNode(NodeType type, Vec2 position);
    void removeNode(const std::string& nodeId);
    std::shared_ptr<VisualNode> getNode(const std::string& nodeId) const;

    // 连接管理
    bool connect(const std::string& sourceNodeId, const std::string& sourcePinId,
                 const std::string& targetNodeId, const std::string& targetPinId);
    void disconnect(const std::string& connectionId);

    // 执行
    void execute();

    // 获取所有节点和连接
    const std::vector<std::shared_ptr<VisualNode>>& getNodes() const { return m_nodes; }
    const std::vector<Connection>& getConnections() const { return m_connections; }

    // 属性
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

private:
    void executeNode(std::shared_ptr<VisualNode> node);

    std::string m_name;
    std::vector<std::shared_ptr<VisualNode>> m_nodes;
    std::vector<Connection> m_connections;
};

// 可视化脚本管理器
class VisualScriptManager {
public:
    static VisualScriptManager& get();

    // 脚本管理
    std::shared_ptr<VisualScript> createScript(const std::string& name);
    void deleteScript(const std::string& name);
    std::shared_ptr<VisualScript> getScript(const std::string& name) const;

    // 执行所有脚本
    void executeAll();

    // 获取所有脚本
    const std::unordered_map<std::string, std::shared_ptr<VisualScript>>& getScripts() const { return m_scripts; }

private:
    VisualScriptManager() = default;

    std::unordered_map<std::string, std::shared_ptr<VisualScript>> m_scripts;
};

} // namespace spark
