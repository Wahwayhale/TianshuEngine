# 天枢引擎 - 架构文档

## 概述

天枢引擎采用模块化架构，各系统之间松耦合，便于扩展和维护。

## 核心架构

```
┌─────────────────────────────────────────────────────────────┐
│                      Application                            │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐         │
│  │  Scene  │ │  ECS    │ │ Systems │ │ Events  │         │
│  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘         │
│       │           │           │           │               │
├───────┴───────────┴───────────┴───────────┴───────────────┤
│                    Engine Core                             │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐         │
│  │Renderer │ │ Physics │ │  Audio  │ │ Scripts │         │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘         │
├───────────────────────────────────────────────────────────┤
│                    Platform Layer                          │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐         │
│  │ Windows │ │  Linux  │ │ Android │ │   Web   │         │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘         │
└───────────────────────────────────────────────────────────┘
```

## 模块说明

### 核心模块 (engine/core)

| 模块 | 说明 |
|------|------|
| Application | 应用基类，管理游戏循环 |
| Window | 窗口管理 |
| Input | 输入处理 |
| Event | 事件系统 |
| Log | 日志系统 |
| Memory | 内存管理 |

### 渲染模块 (engine/renderer)

| 模块 | 说明 |
|------|------|
| Renderer | 渲染器主类 |
| Pipeline | 图形管线 |
| Shader | 着色器管理 |
| Mesh | 网格管理 |
| Material | 材质系统 |
| Texture | 纹理管理 |
| Camera | 相机系统 |
| Light | 光照系统 |

### ECS 模块 (engine/ecs)

| 模块 | 说明 |
|------|------|
| Entity | 实体 |
| Component | 组件基类 |
| System | 系统基类 |
| Scene | 场景管理 |

### 物理模块 (engine/physics)

| 模块 | 说明 |
|------|------|
| PhysicsWorld | 物理世界 |
| RigidBody | 刚体 |
| Collider | 碰撞体 |
| Joint | 关节 |
| Vehicle | 车辆 |
| Cloth | 布料 |
| Destruction | 破碎 |

### 音频模块 (engine/audio)

| 模块 | 说明 |
|------|------|
| AudioEngine | 音频引擎 |
| AudioSource | 音频源 |
| AudioListener | 监听器 |

### 脚本模块 (engine/script)

| 模块 | 说明 |
|------|------|
| ScriptEngine | 脚本引擎 |
| ScriptComponent | 脚本组件 |

### 资源模块 (engine/resource)

| 模块 | 说明 |
|------|------|
| ResourceManager | 资源管理器 |
| AssetPipeline | 资产管线 |

## 数据流

### 渲染数据流

```
Scene
  ↓
Entity (TransformComponent + MeshRendererComponent)
  ↓
RenderSystem
  ↓
Renderer
  ↓
Vulkan Pipeline
  ↓
GPU
  ↓
Screen
```

### 物理数据流

```
Scene
  ↓
Entity (TransformComponent + RigidBodyComponent)
  ↓
PhysicsSystem
  ↓
PhysicsWorld
  ↓
Collision Detection
  ↓
Response
  ↓
Update TransformComponent
```

### 脚本数据流

```
Scene
  ↓
Entity (LuaScriptComponent)
  ↓
ScriptSystem
  ↓
ScriptEngine
  ↓
Lua Runtime
  ↓
Update Components
```

## 扩展指南

### 添加新组件

```cpp
struct MyComponent : public Component {
    float myValue = 0.0f;
    std::string myString;
};
```

### 添加新系统

```cpp
class MySystem : public System {
public:
    void update(Scene& scene, float deltaTime) override {
        scene.view<MyComponent>([&](Entity& entity) {
            auto& comp = entity.getComponent<MyComponent>();
            // 处理逻辑
        });
    }
};
```

### 添加新面板

```cpp
class MyPanel {
public:
    void render() {
        ImGui::Begin("My Panel");
        // UI 逻辑
        ImGui::End();
    }
};
```

## 性能考虑

### ECS 优化

- 使用 SoA 布局提升缓存命中率
- 脏标记避免不必要的更新
- 批量处理减少函数调用

### 渲染优化

- 视锥剔除减少绘制调用
- 实例化渲染批量绘制
- LOD 系统减少三角形数量
- 遮挡剔除减少 overdraw

### 物理优化

- 空间分区加速碰撞检测
- 碰撞休眠减少计算
- 多线程并行计算
