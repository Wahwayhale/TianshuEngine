#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace spark {

class Material;

// 材质节点类型
enum class MaterialNodeType {
    // 输入
    Constant,
    Texture,
    UV,
    Normal,

    // 数学
    Add,
    Multiply,
    Lerp,
    Clamp,

    // 输出
    Albedo,
    Metallic,
    Roughness,
    NormalOutput,
    Emission
};

// 材质节点
struct MaterialNode {
    std::string id;
    MaterialNodeType type;
    Vec2 position;
    Vec2 size;

    // 输入/输出引脚
    struct Pin {
        std::string id;
        std::string name;
        bool isInput;
        int connectedNodeId = -1;
        int connectedPinIndex = -1;
    };

    std::vector<Pin> inputs;
    std::vector<Pin> outputs;

    // 节点数据
    Vec4 constantValue = Vec4(1.0f);
    std::string texturePath;
    float floatValue = 0.0f;
};

// 材质编辑器
class MaterialEditor {
public:
    MaterialEditor();
    ~MaterialEditor();

    // 设置材质
    void setMaterial(std::shared_ptr<Material> material);
    std::shared_ptr<Material> getMaterial() const { return m_material; }

    // 节点操作
    std::shared_ptr<MaterialNode> addNode(MaterialNodeType type, Vec2 position);
    void removeNode(const std::string& nodeId);
    void moveNode(const std::string& nodeId, Vec2 position);

    // 连接操作
    bool connect(const std::string& sourceNodeId, int sourcePinIndex,
                 const std::string& targetNodeId, int targetPinIndex);
    void disconnect(const std::string& nodeId, int pinIndex);

    // 获取节点
    const std::vector<std::shared_ptr<MaterialNode>>& getNodes() const { return m_nodes; }
    std::shared_ptr<MaterialNode> getNode(const std::string& nodeId) const;

    // 编译材质
    bool compile();

private:
    std::shared_ptr<Material> m_material;
    std::vector<std::shared_ptr<MaterialNode>> m_nodes;
};

// 材质编辑器管理器
class MaterialEditorManager {
public:
    static MaterialEditorManager& get();

    // 创建编辑器
    std::shared_ptr<MaterialEditor> createEditor();

    // 获取编辑器
    const std::vector<std::shared_ptr<MaterialEditor>>& getEditors() const { return m_editors; }

private:
    MaterialEditorManager() = default;

    std::vector<std::shared_ptr<MaterialEditor>> m_editors;
};

} // namespace spark
