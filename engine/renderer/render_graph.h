#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace spark {

// 渲染通道类型
enum class RenderPassType {
    Geometry,
    Lighting,
    PostProcess,
    UI,
    Shadow,
    Custom
};

// 渲染通道描述
struct RenderPassDesc {
    std::string name;
    RenderPassType type;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::function<void()> execute;
};

// 渲染图节点
struct RenderGraphNode {
    RenderPassDesc desc;
    std::vector<int> dependencies;
    bool executed = false;
};

// 渲染图
class RenderGraph {
public:
    RenderGraph();
    ~RenderGraph();

    // 添加渲染通道
    int addPass(const RenderPassDesc& desc);

    // 设置依赖
    void addDependency(int pass, int dependsOn);

    // 编译图
    bool compile();

    // 执行图
    void execute();

    // 清空
    void clear();

    // 获取信息
    size_t getPassCount() const { return m_nodes.size(); }

private:
    void executeNode(int index);

    std::vector<RenderGraphNode> m_nodes;
    std::vector<int> m_executionOrder;
    bool m_compiled = false;
};

// 渲染图管理器
class RenderGraphManager {
public:
    static RenderGraphManager& get();

    // 创建渲染图
    std::shared_ptr<RenderGraph> createGraph(const std::string& name);

    // 获取渲染图
    std::shared_ptr<RenderGraph> getGraph(const std::string& name) const;

    // 删除渲染图
    void removeGraph(const std::string& name);

private:
    RenderGraphManager() = default;

    std::unordered_map<std::string, std::shared_ptr<RenderGraph>> m_graphs;
};

} // namespace spark
