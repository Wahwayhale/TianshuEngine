#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace spark {

// 决策树节点类型
enum class DecisionNodeType {
    Decision,    // 决策节点
    Action,      // 动作节点
    Condition    // 条件节点
};

// 决策树上下文
struct DecisionContext {
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
    std::unordered_map<std::string, float> floatValues;
    std::unordered_map<std::string, int> intValues;
    std::unordered_map<std::string, bool> boolValues;
    std::unordered_map<std::string, std::string> stringValues;
};

// 决策树节点
class DecisionNode {
public:
    DecisionNode(const std::string& name, DecisionNodeType type);
    virtual ~DecisionNode();

    virtual bool evaluate(DecisionContext& context) = 0;

    const std::string& getName() const { return m_name; }
    DecisionNodeType getType() const { return m_type; }

protected:
    std::string m_name;
    DecisionNodeType m_type;
};

// 决策节点
class DecisionNodeBranch : public DecisionNode {
public:
    DecisionNodeBranch(const std::string& name);
    ~DecisionNodeBranch() override;

    bool evaluate(DecisionContext& context) override;

    void setCondition(std::function<bool(DecisionContext&)> condition);
    void setTrueNode(std::shared_ptr<DecisionNode> node);
    void setFalseNode(std::shared_ptr<DecisionNode> node);

private:
    std::function<bool(DecisionContext&)> m_condition;
    std::shared_ptr<DecisionNode> m_trueNode;
    std::shared_ptr<DecisionNode> m_falseNode;
};

// 动作节点
class DecisionNodeAction : public DecisionNode {
public:
    DecisionNodeAction(const std::string& name);
    ~DecisionNodeAction() override;

    bool evaluate(DecisionContext& context) override;

    void setAction(std::function<void(DecisionContext&)> action);

private:
    std::function<void(DecisionContext&)> m_action;
};

// 条件节点
class DecisionNodeCondition : public DecisionNode {
public:
    DecisionNodeCondition(const std::string& name);
    ~DecisionNodeCondition() override;

    bool evaluate(DecisionContext& context) override;

    void setCondition(std::function<bool(DecisionContext&)> condition);

private:
    std::function<bool(DecisionContext&)> m_condition;
};

// 决策树
class DecisionTree {
public:
    DecisionTree(const std::string& name = "DecisionTree");
    ~DecisionTree();

    void setRoot(std::shared_ptr<DecisionNode> root);
    std::shared_ptr<DecisionNode> getRoot() const { return m_root; }

    bool evaluate(DecisionContext& context);

    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
    std::shared_ptr<DecisionNode> m_root;
};

// 决策树管理器
class DecisionTreeManager {
public:
    static DecisionTreeManager& get();

    std::shared_ptr<DecisionTree> createTree(const std::string& name);
    std::shared_ptr<DecisionTree> getTree(const std::string& name) const;
    void removeTree(const std::string& name);

    void update(float deltaTime);

private:
    DecisionTreeManager() = default;

    std::unordered_map<std::string, std::shared_ptr<DecisionTree>> m_trees;
};

} // namespace spark
