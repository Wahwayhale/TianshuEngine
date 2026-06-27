# 天枢引擎 - 入门指南

## 简介

天枢引擎是一个现代化的 C++20 游戏引擎，使用 Vulkan 1.3 渲染后端，支持 PBR 渲染、物理模拟、音频系统、脚本系统等功能。

## 系统要求

### 最低配置
- **操作系统**: Windows 10+ / Linux (Ubuntu 20.04+) / macOS 12+
- **CPU**: 4 核处理器
- **内存**: 8 GB RAM
- **显卡**: 支持 Vulkan 1.3 的 GPU
- **存储**: 2 GB 可用空间

### 推荐配置
- **操作系统**: Windows 11 / Linux (Ubuntu 22.04+) / macOS 14+
- **CPU**: 8 核处理器
- **内存**: 16 GB RAM
- **显卡**: NVIDIA RTX 2060 / AMD RX 5700 或更高
- **存储**: 10 GB 可用空间

## 安装

### 1. 下载引擎

从 GitHub Releases 下载对应平台的版本：
- Windows: `TianshuEngine-x.x.x-windows.zip`
- Linux: `TianshuEngine-x.x.x-linux.tar.gz`

### 2. 解压

```bash
# Windows
Expand-Archive TianshuEngine-x.x.x-windows.zip

# Linux
tar -xzf TianshuEngine-x.x.x-linux.tar.gz
```

### 3. 设置环境变量（可选）

```bash
# Windows
set TIANSHU_ENGINE=C:\path\to\TianshuEngine

# Linux
export TIANSHU_ENGINE=/path/to/TianshuEngine
```

## 创建第一个项目

### 1. 使用项目模板

```bash
# 复制模板
cp -r TianshuEngine/templates MyGame
cd MyGame
```

### 2. 修改 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# 设置引擎路径
set(TIANSHU_ENGINE_DIR "C:/path/to/TianshuEngine")

# 包含引擎
include_directories(${TIANSHU_ENGINE_DIR}/include)
link_directories(${TIANSHU_ENGINE_DIR}/lib)

# 查找依赖
find_package(Vulkan REQUIRED)

# 游戏源文件
set(GAME_SOURCES src/main.cpp)

# 创建可执行文件
add_executable(MyGame ${GAME_SOURCES})

# 链接引擎
target_link_libraries(MyGame PRIVATE TianshuEngine Vulkan::Vulkan)
```

### 3. 编写游戏代码

```cpp
// src/main.cpp
#include "core/application.h"
#include "scene/scene.h"
#include "ecs/components.h"

using namespace spark;

class MyGame : public Application {
public:
    MyGame() : Application("My Game", 1280, 720) {
        // 创建场景
        m_scene = std::make_unique<Scene>();

        // 创建玩家实体
        auto& player = m_scene->createEntity("Player");
        player.addComponent<TransformComponent>(Vec3(0.0f, 0.0f, 0.0f));
        player.addComponent<MeshRendererComponent>();

        // 创建相机
        auto& camera = m_scene->createEntity("Camera");
        camera.addComponent<TransformComponent>(Vec3(0.0f, 2.0f, 5.0f));
        camera.addComponent<CameraComponent>();
    }

protected:
    void onUpdate(float deltaTime) override {
        // 更新场景
        m_scene->update(deltaTime);
    }

    void onRender() override {
        // 渲染逻辑
    }

private:
    std::unique_ptr<Scene> m_scene;
};

int main() {
    MyGame game;
    game.run();
    return 0;
}
```

### 4. 构建项目

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 5. 运行

```bash
./MyGame
```

## 核心概念

### 场景 (Scene)

场景是游戏世界的容器，管理所有实体。

```cpp
auto scene = std::make_unique<Scene>();
auto& entity = scene->createEntity("MyEntity");
```

### 实体 (Entity)

实体是游戏对象，由组件组成。

```cpp
auto& entity = scene->createEntity("Player");
entity.addComponent<TransformComponent>();
entity.addComponent<MeshRendererComponent>();
```

### 组件 (Component)

组件是数据容器，定义实体的属性。

```cpp
struct TransformComponent : public Component {
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
};
```

### 系统 (System)

系统处理具有特定组件的实体。

```cpp
class MySystem : public System {
    void update(Scene& scene, float deltaTime) override {
        scene.view<TransformComponent>([&](Entity& entity) {
            auto& transform = entity.getComponent<TransformComponent>();
            // 处理逻辑
        });
    }
};
```

## 下一步

- 阅读 [API 文档](api_reference.md)
- 查看 [示例项目](../examples/)
- 了解 [渲染系统](rendering.md)
- 了解 [物理系统](physics.md)
- 了解 [脚本系统](scripting.md)
