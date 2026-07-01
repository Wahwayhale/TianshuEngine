#include "behavior_tree.h"
#include "core/log.h"

namespace spark {

// =============================================
// BTNode
// =============================================

BTNode::BTNode(const std::string& name) : m_name(name) {}
BTNode::~BTNode() = default;

// =============================================
// CompositeNode
// =============================================

CompositeNode::CompositeNode(const std::string& name) : BTNode(name) {}
CompositeNode::~CompositeNode() = default;

void CompositeNode::addChild(std::shared_ptr<BTNode> child) {
    m_children.push_back(child);
}

// =============================================
// SequenceNode
// =============================================

SequenceNode::SequenceNode(const std::string& name) : CompositeNode(name) {
    m_type = NodeType::Sequence;
}

NodeStatus SequenceNode::tick(BTContext& context) {
    while (m_currentIndex < m_children.size()) {
        NodeStatus status = m_children[m_currentIndex]->tick(context);

        if (status == NodeStatus::Running) {
            return NodeStatus::Running;
        }

        if (status == NodeStatus::Failure) {
            m_currentIndex = 0;
            return NodeStatus::Failure;
        }

        m_currentIndex++;
    }

    m_currentIndex = 0;
    return NodeStatus::Success;
}

void SequenceNode::reset() {
    m_currentIndex = 0;
    for (auto& child : m_children) {
        child->reset();
    }
}

// =============================================
// SelectorNode
// =============================================

SelectorNode::SelectorNode(const std::string& name) : CompositeNode(name) {
    m_type = NodeType::Selector;
}

NodeStatus SelectorNode::tick(BTContext& context) {
    while (m_currentIndex < m_children.size()) {
        NodeStatus status = m_children[m_currentIndex]->tick(context);

        if (status == NodeStatus::Running) {
            return NodeStatus::Running;
        }

        if (status == NodeStatus::Success) {
            m_currentIndex = 0;
            return NodeStatus::Success;
        }

        m_currentIndex++;
    }

    m_currentIndex = 0;
    return NodeStatus::Failure;
}

void SelectorNode::reset() {
    m_currentIndex = 0;
    for (auto& child : m_children) {
        child->reset();
    }
}

// =============================================
// ParallelNode
// =============================================

ParallelNode::ParallelNode(const std::string& name, int successThreshold)
    : CompositeNode(name), m_successThreshold(successThreshold) {
    m_type = NodeType::Parallel;
}

NodeStatus ParallelNode::tick(BTContext& context) {
    int successCount = 0;
    int failureCount = 0;
    int runningCount = 0;

    for (auto& child : m_children) {
        NodeStatus status = child->tick(context);

        switch (status) {
            case NodeStatus::Success:
                successCount++;
                break;
            case NodeStatus::Failure:
                failureCount++;
                break;
            case NodeStatus::Running:
                runningCount++;
                break;
        }
    }

    int threshold = (m_successThreshold < 0) ? static_cast<int>(m_children.size()) : m_successThreshold;

    if (successCount >= threshold) {
        return NodeStatus::Success;
    }

    if (failureCount > 0) {
        return NodeStatus::Failure;
    }

    return NodeStatus::Running;
}

void ParallelNode::reset() {
    for (auto& child : m_children) {
        child->reset();
    }
}

// =============================================
// InverterNode
// =============================================

InverterNode::InverterNode(const std::string& name) : DecoratorNode(name) {
    m_type = NodeType::Inverter;
}

NodeStatus InverterNode::tick(BTContext& context) {
    if (!m_child) return NodeStatus::Failure;

    NodeStatus status = m_child->tick(context);

    switch (status) {
        case NodeStatus::Success:
            return NodeStatus::Failure;
        case NodeStatus::Failure:
            return NodeStatus::Success;
        case NodeStatus::Running:
            return NodeStatus::Running;
    }

    return NodeStatus::Failure;
}

void InverterNode::reset() {
    if (m_child) m_child->reset();
}

// =============================================
// RepeaterNode
// =============================================

RepeaterNode::RepeaterNode(const std::string& name, int repeatCount)
    : DecoratorNode(name), m_repeatCount(repeatCount) {
    m_type = NodeType::Repeater;
}

NodeStatus RepeaterNode::tick(BTContext& context) {
    if (!m_child) return NodeStatus::Failure;

    NodeStatus status = m_child->tick(context);

    if (status == NodeStatus::Running) {
        return NodeStatus::Running;
    }

    if (status == NodeStatus::Failure) {
        return NodeStatus::Failure;
    }

    m_currentCount++;

    if (m_repeatCount > 0 && m_currentCount >= m_repeatCount) {
        m_currentCount = 0;
        return NodeStatus::Success;
    }

    m_child->reset();
    return NodeStatus::Running;
}

void RepeaterNode::reset() {
    m_currentCount = 0;
    if (m_child) m_child->reset();
}

// =============================================
// SucceederNode
// =============================================

SucceederNode::SucceederNode(const std::string& name) : DecoratorNode(name) {
    m_type = NodeType::Succeeder;
}

NodeStatus SucceederNode::tick(BTContext& context) {
    if (m_child) {
        m_child->tick(context);
    }
    return NodeStatus::Success;
}

void SucceederNode::reset() {
    if (m_child) m_child->reset();
}

// =============================================
// FailerNode
// =============================================

FailerNode::FailerNode(const std::string& name) : DecoratorNode(name) {
    m_type = NodeType::Failer;
}

NodeStatus FailerNode::tick(BTContext& context) {
    if (m_child) {
        m_child->tick(context);
    }
    return NodeStatus::Failure;
}

void FailerNode::reset() {
    if (m_child) m_child->reset();
}

// =============================================
// ConditionNode
// =============================================

ConditionNode::ConditionNode(const std::string& name, ConditionFunc condition)
    : BTNode(name), m_condition(condition) {
    m_type = NodeType::Condition;
}

NodeStatus ConditionNode::tick(BTContext& context) {
    if (m_condition && m_condition(context)) {
        return NodeStatus::Success;
    }
    return NodeStatus::Failure;
}

// =============================================
// ActionNode
// =============================================

ActionNode::ActionNode(const std::string& name, ActionFunc action)
    : BTNode(name), m_action(action) {
    m_type = NodeType::Action;
}

NodeStatus ActionNode::tick(BTContext& context) {
    if (m_action) {
        return m_action(context);
    }
    return NodeStatus::Failure;
}

void ActionNode::reset() {
    // 动作节点通常不需要重置
}

// =============================================
// BehaviorTree
// =============================================

BehaviorTree::BehaviorTree(const std::string& name) : m_name(name) {}
BehaviorTree::~BehaviorTree() = default;

void BehaviorTree::setRoot(std::shared_ptr<BTNode> root) {
    m_root = root;
}

NodeStatus BehaviorTree::tick(BTContext& context) {
    if (!m_root) return NodeStatus::Failure;
    return m_root->tick(context);
}

void BehaviorTree::reset() {
    if (m_root) {
        m_root->reset();
    }
}

// =============================================
// BehaviorTreeManager
// =============================================

BehaviorTreeManager& BehaviorTreeManager::get() {
    static BehaviorTreeManager instance;
    return instance;
}

std::shared_ptr<BehaviorTree> BehaviorTreeManager::createTree(const std::string& name) {
    auto tree = std::make_shared<BehaviorTree>(name);
    m_trees[name] = tree;
    SPARK_CORE_INFO("Behavior tree created: {0}", name);
    return tree;
}

std::shared_ptr<BehaviorTree> BehaviorTreeManager::getTree(const std::string& name) const {
    auto it = m_trees.find(name);
    if (it != m_trees.end()) {
        return it->second;
    }
    return nullptr;
}

void BehaviorTreeManager::removeTree(const std::string& name) {
    m_trees.erase(name);
}

void BehaviorTreeManager::update(float deltaTime) {
    if (!m_currentEntity) return;

    BTContext context;
    context.entity = m_currentEntity;
    context.deltaTime = deltaTime;
    context.totalTime += deltaTime;

    for (auto& [name, tree] : m_trees) {
        tree->tick(context);
    }
}

} // namespace spark
