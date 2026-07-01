#include "decision_tree.h"
#include "core/log.h"

namespace spark {

// =============================================
// DecisionNode
// =============================================

DecisionNode::DecisionNode(const std::string& name, DecisionNodeType type)
    : m_name(name), m_type(type) {}

DecisionNode::~DecisionNode() = default;

// =============================================
// DecisionNodeBranch
// =============================================

DecisionNodeBranch::DecisionNodeBranch(const std::string& name)
    : DecisionNode(name, DecisionNodeType::Decision) {}

DecisionNodeBranch::~DecisionNodeBranch() = default;

bool DecisionNodeBranch::evaluate(DecisionContext& context) {
    if (!m_condition) return false;

    if (m_condition(context)) {
        if (m_trueNode) return m_trueNode->evaluate(context);
    } else {
        if (m_falseNode) return m_falseNode->evaluate(context);
    }

    return false;
}

void DecisionNodeBranch::setCondition(std::function<bool(DecisionContext&)> condition) {
    m_condition = condition;
}

void DecisionNodeBranch::setTrueNode(std::shared_ptr<DecisionNode> node) {
    m_trueNode = node;
}

void DecisionNodeBranch::setFalseNode(std::shared_ptr<DecisionNode> node) {
    m_falseNode = node;
}

// =============================================
// DecisionNodeAction
// =============================================

DecisionNodeAction::DecisionNodeAction(const std::string& name)
    : DecisionNode(name, DecisionNodeType::Action) {}

DecisionNodeAction::~DecisionNodeAction() = default;

bool DecisionNodeAction::evaluate(DecisionContext& context) {
    if (m_action) {
        m_action(context);
        return true;
    }
    return false;
}

void DecisionNodeAction::setAction(std::function<void(DecisionContext&)> action) {
    m_action = action;
}

// =============================================
// DecisionNodeCondition
// =============================================

DecisionNodeCondition::DecisionNodeCondition(const std::string& name)
    : DecisionNode(name, DecisionNodeType::Condition) {}

DecisionNodeCondition::~DecisionNodeCondition() = default;

bool DecisionNodeCondition::evaluate(DecisionContext& context) {
    if (m_condition) {
        return m_condition(context);
    }
    return false;
}

void DecisionNodeCondition::setCondition(std::function<bool(DecisionContext&)> condition) {
    m_condition = condition;
}

// =============================================
// DecisionTree
// =============================================

DecisionTree::DecisionTree(const std::string& name) : m_name(name) {}
DecisionTree::~DecisionTree() = default;

void DecisionTree::setRoot(std::shared_ptr<DecisionNode> root) {
    m_root = root;
}

bool DecisionTree::evaluate(DecisionContext& context) {
    if (!m_root) return false;
    return m_root->evaluate(context);
}

// =============================================
// DecisionTreeManager
// =============================================

DecisionTreeManager& DecisionTreeManager::get() {
    static DecisionTreeManager instance;
    return instance;
}

std::shared_ptr<DecisionTree> DecisionTreeManager::createTree(const std::string& name) {
    auto tree = std::make_shared<DecisionTree>(name);
    m_trees[name] = tree;
    SPARK_CORE_INFO("Decision tree created: {0}", name);
    return tree;
}

std::shared_ptr<DecisionTree> DecisionTreeManager::getTree(const std::string& name) const {
    auto it = m_trees.find(name);
    if (it != m_trees.end()) {
        return it->second;
    }
    return nullptr;
}

void DecisionTreeManager::removeTree(const std::string& name) {
    m_trees.erase(name);
}

void DecisionTreeManager::update(float deltaTime) {
    DecisionContext context;
    context.deltaTime = deltaTime;
    context.totalTime += deltaTime;

    for (auto& [name, tree] : m_trees) {
        tree->evaluate(context);
    }
}

} // namespace spark
