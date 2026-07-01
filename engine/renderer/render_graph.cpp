#include "render_graph.h"
#include "core/log.h"
#include <algorithm>
#include <stack>

namespace spark {

RenderGraph::RenderGraph() = default;
RenderGraph::~RenderGraph() = default;

int RenderGraph::addPass(const RenderPassDesc& desc) {
    RenderGraphNode node;
    node.desc = desc;
    node.executed = false;

    m_nodes.push_back(node);
    m_compiled = false;

    return static_cast<int>(m_nodes.size() - 1);
}

void RenderGraph::addDependency(int pass, int dependsOn) {
    if (pass >= 0 && pass < m_nodes.size() && dependsOn >= 0 && dependsOn < m_nodes.size()) {
        m_nodes[pass].dependencies.push_back(dependsOn);
        m_compiled = false;
    }
}

bool RenderGraph::compile() {
    m_executionOrder.clear();

    // 拓扑排序
    std::vector<int> inDegree(m_nodes.size(), 0);
    for (const auto& node : m_nodes) {
        for (int dep : node.dependencies) {
            inDegree[dep]++;
        }
    }

    std::stack<int> stack;
    for (size_t i = 0; i < m_nodes.size(); i++) {
        if (inDegree[i] == 0) {
            stack.push(static_cast<int>(i));
        }
    }

    while (!stack.empty()) {
        int current = stack.top();
        stack.pop();
        m_executionOrder.push_back(current);

        for (int dep : m_nodes[current].dependencies) {
            inDegree[dep]--;
            if (inDegree[dep] == 0) {
                stack.push(dep);
            }
        }
    }

    if (m_executionOrder.size() != m_nodes.size()) {
        SPARK_CORE_ERROR("Render graph has circular dependencies!");
        return false;
    }

    m_compiled = true;
    SPARK_CORE_INFO("Render graph compiled: {0} passes", m_nodes.size());
    return true;
}

void RenderGraph::execute() {
    if (!m_compiled) {
        if (!compile()) return;
    }

    // 重置执行状态
    for (auto& node : m_nodes) {
        node.executed = false;
    }

    // 按顺序执行
    for (int index : m_executionOrder) {
        executeNode(index);
    }
}

void RenderGraph::clear() {
    m_nodes.clear();
    m_executionOrder.clear();
    m_compiled = false;
}

void RenderGraph::executeNode(int index) {
    if (index < 0 || index >= m_nodes.size()) return;

    auto& node = m_nodes[index];
    if (node.executed) return;

    // 先执行依赖
    for (int dep : node.dependencies) {
        executeNode(dep);
    }

    // 执行当前节点
    if (node.desc.execute) {
        node.desc.execute();
    }

    node.executed = true;
}

// =============================================
// RenderGraphManager
// =============================================

RenderGraphManager& RenderGraphManager::get() {
    static RenderGraphManager instance;
    return instance;
}

std::shared_ptr<RenderGraph> RenderGraphManager::createGraph(const std::string& name) {
    auto graph = std::make_shared<RenderGraph>();
    m_graphs[name] = graph;
    SPARK_CORE_INFO("Render graph created: {0}", name);
    return graph;
}

std::shared_ptr<RenderGraph> RenderGraphManager::getGraph(const std::string& name) const {
    auto it = m_graphs.find(name);
    if (it != m_graphs.end()) {
        return it->second;
    }
    return nullptr;
}

void RenderGraphManager::removeGraph(const std::string& name) {
    m_graphs.erase(name);
}

} // namespace spark
