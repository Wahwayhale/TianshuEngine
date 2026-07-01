#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include "math/math_types.h"

namespace spark {

class Entity;

// 节点状态
enum class NodeStatus {
    Success,
    Failure,
    Running
};

// 节点类型
enum class NodeType {
    Sequence,      // 顺序执行
    Selector,      // 选择执行
    Parallel,      // 并行执行
    Inverter,      // 反转结果
    Repeater,      // 重复执行
    Succeeder,     // 总是成功
    Failer,        // 总是失败
    Condition,     // 条件节点
    Action         // 动作节点
};

// 行为树上下文
struct BTContext {
    Entity* entity = nullptr;
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
    std::unordered_map<std::string, void*> blackboard;
};

// 行为树节点基类
class BTNode {
public:
    BTNode(const std::string& name = "");
    virtual ~BTNode();

    virtual NodeStatus tick(BTContext& context) = 0;
    virtual void reset() {}

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    NodeType getType() const { return m_type; }

protected:
    std::string m_name;
    NodeType m_type;
};

// 组合节点基类
class CompositeNode : public BTNode {
public:
    CompositeNode(const std::string& name = "");
    virtual ~CompositeNode();

    void addChild(std::shared_ptr<BTNode> child);
    const std::vector<std::shared_ptr<BTNode>>& getChildren() const { return m_children; }

protected:
    std::vector<std::shared_ptr<BTNode>> m_children;
};

// 顺序节点
class SequenceNode : public CompositeNode {
public:
    SequenceNode(const std::string& name = "Sequence");
    NodeStatus tick(BTContext& context) override;
    void reset() override;

private:
    size_t m_currentIndex = 0;
};

// 选择节点
class SelectorNode : public CompositeNode {
public:
    SelectorNode(const std::string& name = "Selector");
    NodeStatus tick(BTContext& context) override;
    void reset() override;

private:
    size_t m_currentIndex = 0;
};

// 并行节点
class ParallelNode : public CompositeNode {
public:
    ParallelNode(const std::string& name = "Parallel", int successThreshold = -1);
    NodeStatus tick(BTContext& context) override;
    void reset() override;

private:
    int m_successThreshold;
};

// 装饰器节点基类
class DecoratorNode : public BTNode {
public:
    DecoratorNode(const std::string& name = "");
    virtual ~DecoratorNode();

    void setChild(std::shared_ptr<BTNode> child);
    std::shared_ptr<BTNode> getChild() const { return m_child; }

protected:
    std::shared_ptr<BTNode> m_child;
};

// 反转节点
class InverterNode : public DecoratorNode {
public:
    InverterNode(const std::string& name = "Inverter");
    NodeStatus tick(BTContext& context) override;
    void reset() override;
};

// 重复节点
class RepeaterNode : public DecoratorNode {
public:
    RepeaterNode(const std::string& name = "Repeater", int repeatCount = -1);
    NodeStatus tick(BTContext& context) override;
    void reset() override;

private:
    int m_repeatCount;
    int m_currentCount = 0;
};

// 成功节点
class SucceederNode : public DecoratorNode {
public:
    SucceederNode(const std::string& name = "Succeeder");
    NodeStatus tick(BTContext& context) override;
    void reset() override;
};

// 失败节点
class FailerNode : public DecoratorNode {
public:
    FailerNode(const std::string& name = "Failer");
    NodeStatus tick(BTContext& context) override;
    void reset() override;
};

// 条件节点
using ConditionFunc = std::function<bool(BTContext&)>;

class ConditionNode : public BTNode {
public:
    ConditionNode(const std::string& name, ConditionFunc condition);
    NodeStatus tick(BTContext& context) override;

private:
    ConditionFunc m_condition;
};

// 动作节点
using ActionFunc = std::function<NodeStatus(BTContext&)>;

class ActionNode : public BTNode {
public:
    ActionNode(const std::string& name, ActionFunc action);
    NodeStatus tick(BTContext& context) override;
    void reset() override;

private:
    ActionFunc m_action;
};

// 行为树
class BehaviorTree {
public:
    BehaviorTree(const std::string& name = "BehaviorTree");
    ~BehaviorTree();

    void setRoot(std::shared_ptr<BTNode> root);
    std::shared_ptr<BTNode> getRoot() const { return m_root; }

    NodeStatus tick(BTContext& context);
    void reset();

    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
    std::shared_ptr<BTNode> m_root;
};

// 行为树管理器
class BehaviorTreeManager {
public:
    static BehaviorTreeManager& get();

    // 创建行为树
    std::shared_ptr<BehaviorTree> createTree(const std::string& name);

    // 获取行为树
    std::shared_ptr<BehaviorTree> getTree(const std::string& name) const;

    // 删除行为树
    void removeTree(const std::string& name);

    // 更新所有行为树
    void update(float deltaTime);

    // 设置当前实体
    void setCurrentEntity(Entity* entity) { m_currentEntity = entity; }

private:
    BehaviorTreeManager() = default;

    std::unordered_map<std::string, std::shared_ptr<BehaviorTree>> m_trees;
    Entity* m_currentEntity = nullptr;
};

} // namespace spark
