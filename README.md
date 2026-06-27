# Spark Engine - 3D 游戏引擎

一个使用 C++20 和 Vulkan 1.3 构建的现代 3D 游戏引擎，具有精美的编辑器 UI 系统和可运行的 Demo。

## 特性

### 渲染系统
- ✅ Vulkan 1.3 渲染后端
- ✅ 着色器加载 (GLSL → SPIR-V)
- ✅ 图形管线管理（含深度测试）
- ✅ 顶点/索引缓冲区
- ✅ Uniform Buffer (双缓冲)
- ✅ Descriptor Set 资源绑定
- ✅ 深度缓冲
- ✅ 相机系统 (FPS 风格)
- ✅ 光照系统 (方向光/点光/聚光灯)
- ✅ 材质系统 (PBR 基础)
- ✅ 纹理加载
- ✅ glTF 模型加载
- ✅ 阴影映射
- ✅ 后处理 (Bloom, Tone Mapping, Gamma)
- ✅ 粒子系统
- ✅ 地形系统
- ✅ 天空盒系统
- ✅ 骨骼动画系统
- ✅ 实例化渲染

### 核心系统
- ✅ GLFW 窗口和输入管理
- ✅ 事件驱动架构
- ✅ ECS (实体组件系统)
- ✅ 场景管理
- ✅ 资源管理系统
- ✅ 性能分析器
- ✅ 调试控制台
- ✅ 序列化系统 (JSON)
- ✅ 插件系统

### 物理系统
- ✅ 物理世界管理
- ✅ 刚体系统
- ✅ 碰撞检测 (Box/Sphere/Capsule)
- ✅ 物理调试绘制

### 音频系统
- ✅ 音频引擎框架
- ✅ 3D 音频支持
- ✅ 音频源/监听器

### 脚本系统
- ✅ Lua 脚本引擎框架
- ✅ 脚本组件

### 编辑器
- ✅ ImGui 集成
- ✅ 精美 UI 主题系统（暗色/亮色）
- ✅ 自定义控件库（Vec3、搜索框、图标按钮等）
- ✅ Viewport 面板（带 Overlay 工具栏）
- ✅ Inspector 面板（带组件图标和改进样式）
- ✅ Scene Hierarchy 面板（带搜索和拖拽）
- ✅ Asset Browser 面板（带网格/列表视图）
- ✅ 工具栏面板（场景控制、变换工具）
- ✅ 状态栏面板（FPS、实体数量、内存）
- ✅ 控制台面板（日志输出、命令执行）
- ✅ 设置面板（主题、渲染、快捷键）

## 项目结构

```
SparkEngine/
├── engine/              # 引擎核心库
│   ├── core/           # 核心系统 (窗口、输入、事件、日志)
│   ├── renderer/       # Vulkan 渲染系统
│   ├── ecs/            # 实体组件系统
│   ├── scene/          # 场景管理
│   ├── resource/       # 资源管理
│   └── math/           # 数学工具
├── runtime/            # 运行时可执行文件
├── editor/             # 编辑器可执行文件
├── assets/             # 资源文件
│   └── shaders/        # GLSL 着色器
└── third_party/        # 第三方库 (自动下载)
```

## 依赖项

- **Vulkan SDK** 1.3+
- **CMake** 3.20+
- **C++20** 兼容编译器 (MSVC 2022, GCC 12+, Clang 15+)

第三方库 (通过 FetchContent 自动下载):
- GLFW 3.4 - 窗口和输入
- GLM 1.0.1 - 数学库
- spdlog 1.14.1 - 日志库
- stb - 图像加载
- tinygltf 2.9.3 - glTF 模型加载

## 构建步骤

### 1. 安装前置依赖

1. 安装 [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
2. 安装 [CMake](https://cmake.org/download/)
3. 安装 Visual Studio 2022 或其他 C++20 编译器

### 2. 配置和构建

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目 (Windows)
cmake .. -G "Visual Studio 17 2022" -A x64

# 或者使用 Ninja
cmake .. -G Ninja

# 构建
cmake --build . --config Release

# 或者在 Visual Studio 中打开 build/SparkEngine.sln
```

### 3. 运行

```bash
# 运行时
./build/runtime/Release/SparkRuntime.exe

# 编辑器
./build/editor/Release/SparkEditor.exe
```

## 开发阶段

### Phase 1: 基础设施 ✅
- [x] CMake 构建系统
- [x] 日志系统 (spdlog)
- [x] 事件系统
- [x] 窗口管理 (GLFW)
- [x] 输入系统
- [x] Vulkan 实例和设备
- [x] 交换链
- [x] 渲染循环

### Phase 2: 渲染核心 (进行中)
- [ ] 着色器加载
- [ ] 图形管线
- [ ] 顶点缓冲区
- [ ] Uniform Buffer
- [ ] 纹理系统
- [ ] 材质系统

### Phase 3: 场景与实体
- [ ] 完整 ECS 实现
- [ ] 场景序列化
- [ ] 相机系统
- [ ] 光照系统

### Phase 4: 物理系统
- [ ] 物理引擎集成
- [ ] 碰撞检测
- [ ] 刚体物理

### Phase 5: 音频系统
- [ ] 音频引擎
- [ ] 3D 空间音频

### Phase 6: 脚本系统
- [ ] Lua 绑定
- [ ] 脚本组件

### Phase 7: 编辑器
- [ ] ImGui 集成
- [ ] 可视化编辑器

### Phase 8: 进阶特性
- [ ] 阴影映射
- [ ] 后处理效果
- [ ] 资源打包

## 代码示例

### 创建应用

```cpp
#include "core/application.h"

class MyApp : public spark::Application {
public:
    MyApp() : Application("My Game", 1280, 720) {}

protected:
    void onUpdate(float deltaTime) override {
        // 游戏逻辑
    }

    void onRender() override {
        // 渲染命令
    }
};

int main() {
    MyApp app;
    app.run();
    return 0;
}
```

### 使用 ECS

```cpp
auto& scene = ...;
auto& entity = scene.createEntity("Player");

// 添加组件
auto& transform = entity.addComponent<TransformComponent>();
transform.position = Vec3(0.0f, 1.0f, 0.0f);

// 获取组件
if (entity.hasComponent<TransformComponent>()) {
    auto& t = entity.getComponent<TransformComponent>();
    t.rotation.y += 45.0f * deltaTime;
}
```

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License
