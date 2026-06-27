# Changelog

## [0.31.0] - 2026-06-27

### AI 深度集成 Phase 1 & 2

#### Phase 1: AI 代码生成增强
- **AICodeGenerator** — 增强的代码生成器
- **代码补全** — 根据上下文补全代码
- **代码解释** — 解释代码功能
- **代码重构** — 按指令重构代码
- **错误修复** — 根据错误信息修复代码
- **代码优化** — 优化代码性能
- **添加注释** — 自动添加中文注释
- **代码翻译** — 跨语言翻译
- **代码模板库** — 内置常用脚本模板

#### 代码模板

| 模板 | 说明 |
|------|------|
| WASD Movement | WASD 键控制移动 |
| Rotation | 物体旋转 |
| Bounce | 上下弹跳 |
| NPC Patrol | NPC 巡逻 |

#### Phase 2: AI 资产生成
- **AIAssetGenerator** — 资产生成器
- **纹理生成** — 文字描述 → 纹理
- **材质生成** — 文字描述 → PBR 材质
- **音频生成** — 文字描述 → 音效描述
- **3D 模型生成** — 文字描述 → 模型描述

#### 纹理生成选项

```cpp
TextureGenOptions options;
options.width = 1024;
options.height = 1024;
options.style = "cartoon";  // realistic, cartoon, pixel, handpainted
options.seamless = true;
options.generateNormalMap = true;
```

#### 材质参数生成

```cpp
auto params = AIAssetGenerator::get().generateMaterialParams("金属质感的剑");
// params.metallic = 1.0
// params.roughness = 0.2
// params.albedo = Vec4(0.8, 0.8, 0.8, 1.0)
```

#### 新增文件
- `engine/ai/ai_code_generator.h/cpp` — 代码生成器
- `engine/ai/ai_asset_generator.h/cpp` — 资产生成器

### 设计原理
- 代码模板库提供常用脚本起点
- 资产生成支持多种风格
- 材质参数自动生成减少手动调整

## [0.30.0] - 2026-06-27

### AI 系统增强

#### 自动获取模型列表
- 输入 API Key 后自动调用 `/models` 接口
- 获取该账号可用的所有模型
- 从下拉列表中选择模型
- 支持手动刷新模型列表

#### 支持 GET 请求
- HTTP 请求支持 GET/POST 两种方式
- 用于获取模型列表等 API

#### 修改文件
- `engine/ai/ai_manager.h/cpp` — 支持获取模型列表
- `editor/panels/ai_panel.h/cpp` — 自动获取并显示模型列表

## [0.29.0] - 2026-06-27

### AI 集成 — 国产大模型支持

#### AI 系统
- **AIManager** — 统一 AI 接口管理
- **AIConfig** — 灵活的 AI 配置系统
- **支持国产大模型** — 通义千问、ChatGLM、百川、DeepSeek、Kimi、讯飞星火、百度文心
- **本地模型支持** — Ollama 等本地部署
- **自定义 API** — 兼容 OpenAI API 格式

#### 支持的 AI 提供商

| 提供商 | 模型 | 特点 |
|--------|------|------|
| 通义千问 (Qwen) | qwen-turbo/plus/max | 阿里云，稳定 |
| 智谱 ChatGLM | glm-4-flash | 清华，免费额度 |
| 百川 Baichuan | Baichuan4 | 国产，效果好 |
| 深度求索 DeepSeek | deepseek-chat | 便宜，代码强 |
| 月之暗面 Kimi | moonshot-v1-8k | 长文本 |
| 讯飞星火 | general | 讯飞，中文好 |
| 百度文心 | ernie-speed-128k | 百度，免费 |
| 本地模型 | qwen2:7b 等 | Ollama，免费离线 |

#### AI 功能

| 功能 | 说明 |
|------|------|
| 代码生成 | 自然语言 → Lua 脚本 |
| 资产描述 | 生成资产描述 |
| 场景生成 | 生成场景布局 |
| NPC 行为 | 生成 NPC 脚本 |
| 对话生成 | 生成 NPC 对话 |

#### AI 助手面板
- 配置界面（选择提供商、输入 API Key）
- 聊天界面（与 AI 对话）
- 快捷操作（一键生成常用脚本）
- 连接测试

#### 使用示例

```
用户输入："让角色按 WASD 移动，按空格跳跃"
    ↓
AI 生成 Lua 脚本
    ↓
自动添加到实体
```

#### 新增文件
- `engine/ai/ai_config.h` — AI 配置
- `engine/ai/ai_manager.h/cpp` — AI 管理器
- `editor/panels/ai_panel.h/cpp` — AI 助手面板

#### 配置示例

```cpp
// 使用 DeepSeek
auto config = ai_presets::deepseek("your-api-key");
AIManager::get().setConfig(config);

// 使用本地模型
auto config = ai_presets::local("http://localhost:11434/v1", "qwen2:7b");
AIManager::get().setConfig(config);

// 生成代码
auto response = AIManager::get().generateCode("让物体旋转");
if (response.success) {
    // 使用 response.content
}
```

### 设计原理
- 支持国产大模型，无需翻墙
- 用户自填 API Key，灵活选择
- 兼容 OpenAI API 格式，易于扩展
- 本地模型支持，可离线使用

## [0.28.0] - 2026-06-27

### 4星目标 Phase 2: 平台与生态

#### Phase D: 平台支持
- **Platform 抽象层** — 跨平台 API 封装
- **平台检测** — Windows/Linux/macOS 自动识别
- **文件系统** — 跨平台文件操作
- **动态库** — 跨平台插件加载
- **时间/线程** — 跨平台时间查询和线程管理
- **剪贴板** — 跨平台剪贴板操作
- **消息框** — 跨平台对话框

#### Phase E: 生态文档
- **示例项目** — 8 个完整示例
  - basic_cube — 基础渲染
  - physics_demo — 物理模拟
  - scripting_demo — Lua 脚本
  - pbr_materials — PBR 材质展示
  - lighting_demo — 光照系统
  - ecs_demo — ECS 架构
  - animation_demo — 动画系统
  - deferred_rendering_demo — 延迟渲染

#### 新增文件
- `engine/platform/platform.h/cpp` — 平台抽象层
- `examples/pbr_materials/main.cpp` — PBR 材质示例
- `examples/lighting_demo/main.cpp` — 光照示例
- `examples/ecs_demo/main.cpp` — ECS 示例
- `examples/animation_demo/main.cpp` — 动画示例
- `examples/deferred_rendering_demo/main.cpp` — 延迟渲染示例

#### 平台 API
```cpp
// 平台信息
Platform::get().getType()          // Windows/Linux/macOS
Platform::get().getTypeName()      // "Windows"
Platform::get().getTotalMemory()   // 总内存

// 文件系统
Platform::get().fileExists(path)
Platform::get().createDirectory(path)

// 动态库
void* lib = Platform::get().loadLibrary("plugin.dll");
auto func = Platform::get().getSymbol(lib, "init");

// 时间
double time = Platform::get().getTime();
```

### 设计原理
- 平台抽象层隔离平台差异
- 示例项目展示引擎各模块功能
- 8 个示例覆盖主要功能

## [0.27.0] - 2026-06-27

### 4星目标 Phase 1: 核心能力补齐

#### Phase A: Jolt Physics 集成
- **Jolt Physics** — 集成专业物理引擎
- **JoltPhysicsWorld** — 封装 Jolt 物理系统
- **PhysicsSystem** — ECS 物理系统，自动同步实体
- **关节约束** — 支持铰链、滑块、弹簧等
- **触发器事件** — 碰撞触发器支持
- **射线检测** — 使用 Jolt 的精确射线检测

#### Phase B: 延迟渲染管线
- **DeferredRenderer** — G-Buffer 和延迟着色
- **G-Buffer** — Albedo+Metallic, Normal+Roughness, Position+AO
- **几何通道** — 写入 G-Buffer
- **光照通道** — 从 G-Buffer 计算光照
- **PBR 延迟着色** — Cook-Torrance BRDF

#### Phase C: 编辑器交互
- **EntityPicker** — 射线拾取系统
- **屏幕到世界射线** — 从鼠标位置计算射线
- **AABB 相交测试** — 快速包围盒检测
- **球体相交测试** — 精确球体检测

#### 新增文件
- `engine/physics/jolt_physics.h/cpp` — Jolt Physics 封装
- `engine/renderer/deferred_renderer.h/cpp` — 延迟渲染器
- `editor/core/entity_picker.h/cpp` — 实体拾取系统
- `assets/shaders/gbuffer_vert.glsl` — G-Buffer 顶点着色器
- `assets/shaders/gbuffer_frag.glsl` — G-Buffer 片段着色器
- `assets/shaders/deferred_lighting_frag.glsl` — 延迟光照着色器

#### 依赖更新
- **Jolt Physics** — 通过 FetchContent 集成

#### 渲染管线流程（更新）
```
几何通道：渲染实体到 G-Buffer
    ↓ Albedo+Metallic, Normal+Roughness, Position+AO, Depth
光照通道：从 G-Buffer 计算 PBR 光照
    ↓ HDR 颜色
后处理通道：ACES + Bloom + Fog + Gamma
    ↓ 交换链图像
输出到屏幕
```

### 设计原理
- Jolt Physics 比 PhysX 更轻量，适合独立引擎
- 延迟渲染支持大量光源
- 射线拾取支持精确实体选择

## [0.26.0] - 2026-06-27

### Phase 10: 平台与工具链

#### 构建系统
- **CMake Presets** — 预定义构建配置
  - `windows-debug` / `windows-release` / `windows-relwithdebinfo`
  - `linux-debug` / `linux-release`
  - `clang-tidy` 静态分析
- **跨平台支持** — Windows (MSVC), Linux (GCC/Clang), macOS

#### CI/CD 配置
- **GitHub Actions** — 自动化构建和测试
  - Windows 构建 (MSVC)
  - Linux 构建 (GCC + Ninja)
  - macOS 构建 (Clang)
  - 代码静态分析 (clang-tidy)
  - 文档生成 (Doxygen)
  - 自动发布 (Tag 触发)

#### API 文档
- **Doxygen 配置** — 自动生成 API 文档
  - HTML 输出
  - 类图和调用图
  - 搜索功能
  - Markdown 支持

#### 示例项目
- **basic_cube** — 基础渲染示例
- **physics_demo** — 物理系统示例
- **scripting_demo** — 脚本系统示例
  - `rotate.lua` — 旋转脚本
  - `bounce.lua` — 弹跳脚本

#### 新增文件
- `CMakePresets.json` — CMake 预设配置
- `.github/workflows/build.yml` — CI/CD 配置
- `Doxyfile` — Doxygen 配置
- `examples/basic_cube/main.cpp` — 基础示例
- `examples/physics_demo/main.cpp` — 物理示例
- `examples/scripting_demo/main.cpp` — 脚本示例
- `examples/scripting_demo/scripts/rotate.lua` — 旋转脚本
- `examples/scripting_demo/scripts/bounce.lua` — 弹跳脚本

#### 使用方式

```bash
# Windows
cmake --preset windows-release
cmake --build --preset windows-release

# Linux
cmake --preset linux-release
cmake --build --preset linux-release

# 生成文档
doxygen Doxyfile
```

### 设计原理
- CMake Presets 简化跨平台构建
- GitHub Actions 自动化 CI/CD
- Doxygen 自动生成 API 文档
- 示例项目展示引擎功能

## [0.25.0] - 2026-06-27

### Phase 9: 渲染进阶

#### SSAO (屏幕空间环境光遮蔽)
- **SSAO 类** — 管理 SSAO 渲染资源和管线
- **采样核** — 64 个随机采样点（聚集在原点附近）
- **噪声纹理** — 4x4 随机旋转向量
- **模糊** - 降低 SSAO 噪声

#### SSAO 设置
```cpp
struct SSAOSettings {
    float radius = 0.5f;      // 采样半径
    float bias = 0.025f;      // 深度偏移
    float power = 2.0f;       // 遮蔽强度
    int kernelSize = 64;      // 采样数量
    int blurSize = 4;         // 模糊大小
    bool enabled = true;      // 启用
};
```

#### 雾效果着色器
- **线性雾** — 距离线性衰减
- **指数雾** — 指数衰减
- **指数平方雾** — 更快衰减
- **高度雾** — 基于高度的雾效果

#### 雾设置
```cpp
struct FogSettings {
    Vec4 fogColor;           // 雾颜色
    float fogDensity;        // 密度
    float fogStart;          // 开始距离
    float fogEnd;            // 结束距离
    int fogType;             // 0=线性, 1=指数, 2=指数平方
    float fogHeight;         // 高度雾高度
    float fogHeightFalloff;  // 高度雾衰减
};
```

#### 新增文件
- `engine/renderer/ssao.h/cpp` — SSAO 系统
- `assets/shaders/ssao_frag.glsl` — SSAO 着色器
- `assets/shaders/ssao_blur_frag.glsl` — SSAO 模糊着色器
- `assets/shaders/fog_frag.glsl` — 雾效果着色器

#### 渲染管线流程（更新）
```
第一遍：阴影渲染通道
    ↓ 深度贴图
第二遍：主渲染通道（PBR + 多光源 + 阴影）
    ↓ HDR 离屏纹理 + 深度缓冲
第三遍：SSAO 通道
    ↓ SSAO 纹理
第四遍：后处理通道（ACES + Bloom + Fog + Gamma）
    ↓ 交换链图像
输出到屏幕
```

### 设计原理
- SSAO 使用深度缓冲重建视空间位置
- 噪声纹理减少带状伪迹
- 模糊降低 SSAO 噪声
- 雾效果增加场景深度感

## [0.24.0] - 2026-06-27

### Phase 8: 动画系统

#### 骨骼动画
- **Bone 类** — 骨骼关键帧插值（位置/旋转/缩放）
- **Animation 类** — glTF 动画加载
- **Animator 类** — 动画播放和骨骼矩阵计算

#### 关键帧插值
- **位置** — 线性插值 (lerp)
- **旋转** — 球面线性插值 (slerp)
- **缩放** — 线性插值 (lerp)

#### AnimatorComponent
```cpp
struct AnimatorComponent : public Component {
    std::string animationPath;    // 动画文件路径
    float speed = 1.0f;           // 播放速度
    bool looping = true;          // 循环播放
    bool playOnStart = true;      // 自动播放

    std::shared_ptr<Animation> animation;
    std::shared_ptr<Animator> animator;
    bool isPlaying = false;
    std::vector<Mat4> boneMatrices;  // 骨骼矩阵
};
```

#### AnimationSystem
- 自动加载动画文件
- 每帧更新骨骼矩阵
- 支持播放速度控制
- 支持循环/单次播放
- 动画缓存

#### 使用示例
```cpp
// 添加动画组件
auto& animator = entity.addComponent<AnimatorComponent>();
animator.animationPath = "assets/animations/idle.glb";
animator.speed = 1.0f;
animator.looping = true;
animator.playOnStart = true;
```

#### 新增文件
- `engine/renderer/animation_system.h/cpp` — ECS 动画系统

#### 修改文件
- `engine/renderer/animation.h/cpp` — 增强动画类

#### glTF 动画加载
- 从 glTF 文件加载动画数据
- 支持多个动画通道（translation/rotation/scale）
- 支持骨骼逆绑定矩阵
- 自动解析骨骼层次结构

### 设计原理
- glTF 作为标准动画格式
- 关键帧插值提供平滑动画
- 骨骼矩阵传给着色器实现顶点蒙皮

## [0.23.0] - 2026-06-27

### Phase 7: 编辑器完善

#### 撤销/重做系统
- **Command 模式** — 所有操作封装为可撤销命令
- **CommandManager** — 管理撤销/重做栈
- **具体命令**：
  - SetPositionCommand — 修改位置
  - SetRotationCommand — 修改旋转
  - SetScaleCommand — 修改缩放
  - CreateEntityCommand — 创建实体
  - DeleteEntityCommand — 删除实体
  - AddComponentCommand — 添加组件
  - RemoveComponentCommand — 删除组件

#### 编辑器状态管理
- **EditorMode** — Edit/Play/Pause 模式
- **EditorState** — 单例状态管理
- **Gizmo 模式** — Translate/Rotate/Scale (W/E/R 切换)
- **Gizmo 空间** — World/Local

#### 快捷键

| 快捷键 | 功能 |
|--------|------|
| Ctrl+Z | 撤销 |
| Ctrl+Y | 重做 |
| F5 | Play/Stop |
| W | 移动工具 |
| E | 旋转工具 |
| R | 缩放工具 |

#### Play/Stop 模式
- **Edit 模式** — 编辑场景，物理/脚本不运行
- **Play 模式** — 运行游戏逻辑（物理、脚本）
- **Pause 模式** — 暂停游戏逻辑

#### 新增文件
- `editor/core/editor_state.h/cpp` — 编辑器状态管理
- `editor/core/command_system.h/cpp` — 撤销/重做系统

#### 修改文件
- `editor/main.cpp` — 集成新功能
- `editor/CMakeLists.txt` — 添加新源文件

#### 编辑器架构
```
EditorApp
├── EditorState (Edit/Play/Pause 模式)
├── CommandManager (撤销/重做)
├── Scene (实体管理)
├── Systems
│   ├── RenderSystem
│   ├── PhysicsSystem
│   └── ScriptSystem
└── Panels
    ├── Viewport
    ├── Inspector
    ├── Hierarchy
    ├── AssetBrowser
    ├── Toolbar
    ├── StatusBar
    ├── Console
    └── Settings
```

### 设计原理
- Command 模式支持任意操作的撤销/重做
- EditorState 集中管理编辑器状态
- Play/Stop 模式分离编辑和运行逻辑

## [0.22.0] - 2026-06-27

### Phase 6: 资源管理系统

#### 资源管理器重写
- **真正的资源加载** — 纹理、模型实际加载到 GPU
- **资源缓存** — 相同路径只加载一次
- **引用计数** — 自动管理资源生命周期
- **线程安全** — mutex 保护并发访问

#### 资源类型

| 类型 | 说明 |
|------|------|
| Resource | 资源基类，包含路径、ID、引用计数 |
| TextureResource | 纹理资源，加载图片到 GPU |
| ModelResource | 模型资源，加载 glTF 网格 |

#### ResourceManager API

```cpp
// 初始化
ResourceManager::get().initialize(&device);

// 加载资源（自动缓存）
auto texture = ResourceManager::get().loadTexture("assets/textures/brick.png");
auto model = ResourceManager::get().loadModel("assets/models/character.glb");

// 获取已加载资源
auto texture = ResourceManager::get().getTexture("assets/textures/brick.png");

// 卸载资源
ResourceManager::get().unload("assets/textures/brick.png");
ResourceManager::get().unloadAll();

// 统计
size_t count = ResourceManager::get().getResourceCount();
```

#### 功能特性
- **自动缓存** — 相同路径返回同一资源
- **引用计数** — addRef()/release() 管理生命周期
- **线程安全** — mutex 保护并发访问
- **错误处理** — 加载失败返回 nullptr

#### 修改文件
- `engine/resource/resource_manager.h/cpp` — 完整重写

#### 使用示例
```cpp
// 在应用初始化时
ResourceManager::get().initialize(&device);

// 加载纹理
auto brickTexture = ResourceManager::get().loadTexture("assets/textures/brick.png");
if (brickTexture) {
    // 使用纹理
    descriptorSets->bindTexture(0, *brickTexture->getTexture());
}

// 加载模型
auto characterModel = ResourceManager::get().loadModel("assets/models/character.glb");
if (characterModel) {
    // 使用模型
    auto& meshes = characterModel->getMeshes();
}
```

### 设计原理
- 缓存避免重复加载
- 引用计数自动释放未使用资源
- 线程安全支持异步加载

## [0.21.0] - 2026-06-27

### Phase 5: 脚本系统

#### Lua + sol2 集成
- **ScriptEngine** — 单例脚本引擎，管理 Lua 状态
- **ScriptSystem** — ECS 脚本系统，管理脚本生命周期
- **LuaScriptComponent** — 脚本组件，附加到实体

#### API 绑定

| 类别 | 绑定内容 |
|------|---------|
| 数学 | Vec2/Vec3/Vec4 类型，length/normalize/dot/cross/lerp/clamp |
| 输入 | Input.isKeyPressed/isKeyJustPressed/isMouseButtonPressed/getMousePosition |
| 日志 | Log.info/warn/error |
| 时间 | Time.deltaTime/totalTime |
| 组件 | TransformComponent/TagComponent/RigidBodyComponent/MeshRendererComponent |

#### 脚本生命周期

```lua
-- 脚本示例
function OnInit()
    Log.info("Entity initialized!")
end

function OnUpdate(deltaTime)
    local pos = entity.position
    pos.y = pos.y + math.sin(Time.totalTime * 2) * deltaTime
    entity.position = pos
end

function OnDestroy()
    Log.info("Entity destroyed!")
end
```

#### LuaScriptComponent

```cpp
struct LuaScriptComponent : public Component {
    std::string scriptPath;     // 脚本文件路径
    bool loaded = false;        // 是否已加载
    bool hasOnInit = false;     // 是否有 OnInit
    bool hasOnUpdate = false;   // 是否有 OnUpdate
    bool hasOnDestroy = false;  // 是否有 OnDestroy
    void* scriptEnv = nullptr;  // Lua 环境
};
```

#### ScriptSystem 功能
- 自动加载脚本文件
- 每个实体独立的 Lua 环境
- 生命周期回调 (OnInit/OnUpdate/OnDestroy)
- 错误处理和日志输出

#### 新增文件
- `engine/script/script_system.h/cpp` — ECS 脚本系统

#### 修改文件
- `engine/script/script_engine.h/cpp` — sol2 实现

#### 依赖
- **Lua 5.4** — 脚本语言运行时
- **sol2** — C++/Lua 绑定库
  - 下载: https://github.com/ThePhD/sol2
  - 放置: `third_party/sol2/`

#### 使用示例
```cpp
// 添加脚本组件
auto& script = entity.addComponent<LuaScriptComponent>();
script.scriptPath = "assets/scripts/player.lua";
```

### 设计原理
- sol2 提供类型安全的 C++/Lua 绑定
- 每个实体独立环境，避免脚本间干扰
- 生命周期回调便于管理脚本逻辑

## [0.20.0] - 2026-06-27

### Phase 4: 音频系统

#### miniaudio 集成
- **AudioEngine** — 单例音频引擎，管理所有音频源
- **AudioSource** — 封装单个音频文件的播放
- **AudioSystem** — ECS 音频系统，自动同步音频源位置

#### 功能
- 音频文件加载和播放
- 3D 空间音频（位置、距离衰减）
- 音量/音调控制
- 循环播放
- 听听器（相机）位置同步

#### AudioSystem
- 自动创建音频源
- 每帧同步实体位置到音频源
- 监听器跟随相机

#### AudioSourceComponent
```cpp
struct AudioSourceComponent : public Component {
    std::string audioPath;    // 音频文件路径
    float volume = 1.0f;      // 音量
    float pitch = 1.0f;       // 音调
    float range = 20.0f;      // 听到范围
    bool looping = false;     // 循环播放
    bool playOnStart = true;  // 自动播放
    bool spatial = true;      // 3D 空间音频
};
```

#### 新增文件
- `engine/audio/audio_system.h/cpp` — ECS 音频系统

#### 修改文件
- `engine/audio/audio_engine.h/cpp` — miniaudio 实现
- `engine/audio/audio_source.h/cpp` — miniaudio 实现

#### 依赖
- **miniaudio** — 单头文件音频库
  - 下载: https://github.com/mackron/miniaudio
  - 放置: `third_party/miniaudio/miniaudio.h`

#### 使用示例
```cpp
// 添加音频源组件
auto& audio = entity.addComponent<AudioSourceComponent>();
audio.audioPath = "assets/audio/background.wav";
audio.looping = true;
audio.volume = 0.5f;
```

### 设计原理
- miniaudio 单头文件，零依赖，跨平台
- ECS 驱动：音频源位置自动从 TransformComponent 同步
- 3D 空间音频：距离衰减、方向性

## [0.19.0] - 2026-06-27

### Phase 3: 物理系统

#### PhysicsSystem
- **重力模拟** — 可配置重力向量
- **碰撞检测** — Sphere-Sphere、Box-Box (AABB)、Sphere-Box
- **碰撞响应** — 冲量法分离 + 弹性系数
- **速度积分** — 欧拉积分
- **阻尼** — 线性阻尼 + 角阻尼
- **射线检测** — 支持 Sphere 和 Box 碰撞体

#### RigidBodyComponent 增强
```cpp
struct RigidBodyComponent : public Component {
    Vec3 velocity, acceleration, force;
    Vec3 angularVelocity, torque;
    float mass, inverseMass;
    float restitution;  // 弹性系数
    float friction;     // 摩擦系数
    float linearDamping, angularDamping;
    bool useGravity, isStatic, isKinematic;

    void setMass(float m);
    void applyForce(const Vec3& f);
    void applyImpulse(const Vec3& impulse);
    void applyTorque(const Vec3& t);
};
```

#### ColliderComponent 增强
```cpp
struct ColliderComponent : public Component {
    enum class ColliderType { Box, Sphere, Capsule, Plane };
    ColliderType colliderType;
    Vec3 size;        // Box 半尺寸
    float radius;     // Sphere/Capsule 半径
    float height;     // Capsule 高度
    bool isTrigger;   // 触发器
    Vec3 offset;      // 偏移
};
```

#### 新增文件
- `engine/physics/physics_system.h/cpp` — 物理系统

#### 修改文件
- `engine/ecs/components.h` — RigidBody/Collider 增强
- `runtime/main.cpp` — 物理 Demo

#### Demo 场景
- ⬜ 静态地面（Box 碰撞体）
- 🟦 5 个堆叠立方体（动态，不同颜色）
- 🔴 3 个弹跳球（不同弹性系数）
- 🟡 金属球（高质量，高弹性）

#### 物理行为
- 物体受重力下落
- 碰撞后弹跳（弹性系数控制）
- 堆叠稳定（阻尼 + 摩擦）
- 地面碰撞响应

### 设计原理
- 冲量法碰撞响应（简单高效）
- 欧拉积分（足够 Demo 用）
- O(n²) 碰撞检测（简单实现，生产环境需空间分区）

## [0.18.0] - 2026-06-27

### Phase 2: ECS 驱动渲染

#### ECS 系统增强
- **组件迭代查询** — `scene.view<T1, T2, ...>()` 遍历具有特定组件的实体
- **getEntitiesWith<T1, T2, ...>()** — 返回实体列表
- **自动 TagComponent** — `createEntity()` 自动添加名称组件
- **SystemManager 集成** — Scene 管理所有系统

#### RenderSystem
- **ECS 驱动渲染** — 遍历所有 `TransformComponent + MeshRendererComponent` 实体
- **自动材质更新** — 从 `MeshRendererComponent` 读取 PBR 参数
- **自动模型矩阵** — 从 `TransformComponent` 计算变换矩阵
- **可见性控制** — `MeshRendererComponent::visible` 控制是否渲染
- **阴影投射** — `MeshRendererComponent::castShadow` 控制是否投射阴影

#### MeshRendererComponent 增强
```cpp
struct MeshRendererComponent : public Component {
    std::shared_ptr<Mesh> mesh;
    Vec4 albedo = Vec4(1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    float emission = 0.0f;
    bool visible = true;
    bool castShadow = true;
};
```

#### 新增文件
- `engine/renderer/render_system.h/cpp` — ECS 渲染系统

#### 修改文件
- `engine/ecs/components.h` — MeshRendererComponent 增强
- `engine/scene/scene.h/cpp` — 组件迭代查询 + 系统管理
- `engine/renderer/render_system.h/cpp` — ECS 渲染系统
- `runtime/main.cpp` — ECS 驱动渲染 Demo

#### 场景结构
```
Scene
├── Red Cube     (Transform + MeshRenderer)
├── Blue Cube    (Transform + MeshRenderer)
├── Gold Cube    (Transform + MeshRenderer)
└── Ground       (Transform + MeshRenderer)
```

#### Demo 敼果
- 4 个实体通过 ECS 管理
- RenderSystem 自动遍历并渲染
- 实体旋转动画通过修改 TransformComponent
- 材质参数从 MeshRendererComponent 读取

### 设计原理
- ECS 数据驱动：组件存储数据，系统处理逻辑
- 渲染与逻辑解耦：RenderSystem 只负责渲染
- 易于扩展：添加新实体只需创建组件

## [0.17.0] - 2026-06-27

### Phase 1.5: 后处理管线

#### 离屏渲染架构
- **HDR 离屏目标** — R16G16B16A16_SFLOAT 格式，支持 HDR 渲染
- **深度缓冲** — 离屏深度附件
- **三遍渲染管线**：
  1. 阴影渲染通道 → 深度贴图
  2. 主渲染通道 → 离屏 HDR 目标
  3. 后处理通道 → 交换链

#### 后处理效果
- **ACES 色调映射** — 电影级 HDR → LDR 转换
- **Bloom** — 亮度提取 + 高斯模糊 + 合成
- **曝光控制** — 可调曝光度
- **Gamma 校正** — sRGB 输出
- **饱和度/对比度/亮度** — 可调后处理参数

#### PostProcess 类
- 管理离屏渲染资源
- 全屏四边形渲染
- 后处理 UBO（可调参数）
- 窗口大小改变时自动重建

#### 着色器
- `fullscreen_vert.glsl` — 全屏四边形顶点着色器
- `post_process_frag.glsl` — 后处理片段着色器
  - ACES 色调映射
  - 简单 Bloom（高斯模糊近似）
  - 曝光/饱和度/对比度/亮度调整
  - Gamma 校正

#### 新增文件
- `engine/renderer/post_process.h/cpp` — 后处理系统
- `assets/shaders/fullscreen_vert.glsl` — 全屏四边形着色器
- `assets/shaders/post_process_frag.glsl` — 后处理着色器

#### 渲染管线流程
```
第一遍：阴影渲染通道
    ↓ 深度贴图
第二遍：主渲染通道（PBR + 光照 + 阴影）
    ↓ HDR 离屏纹理
第三遍：后处理通道（ACES + Bloom + Gamma）
    ↓ 交换链图像
输出到屏幕
```

#### Demo 效果
- 🎬 ACES 电影级色调映射
- ✨ Bloom 发光效果
- 🔆 可调曝光度
- 🎨 饱和度/对比度调整

### 设计原理
- HDR 格式避免高光裁剪
- ACES 色调映射提供电影级视觉效果
- 三遍渲染分离各阶段，便于扩展

## [0.16.0] - 2026-06-27

### Phase 1.4: 阴影映射系统

#### 阴影渲染管线
- **ShadowMap 类** — 2048x2048 深度贴图，D32_SFLOAT 格式
- **阴影渲染通道** — 仅深度写入，无颜色附件
- **光源空间矩阵** — 正交投影 + lookAt，适合方向光
- **两遍渲染** — 第一遍写阴影贴图，第二一遍采样阴影

#### 着色器更新
- **shadow_vert.glsl** — 阴影顶点着色器（仅 MVP 变换）
- **pbr_frag.glsl** — 添加阴影采样
  - PCF (3x3) 软阴影
  - 自适应偏移（根据法线-光线角度）
  - 阴影强度可配置
  - 边界检测（超出阴影贴图范围）

#### 描述符集架构更新
```
Set 0
├── Binding 0: Scene UBO
├── Binding 1: Light UBO
├── Binding 2: Material UBO
├── Binding 3: Shadow UBO      ← 新增
├── Binding 4: Albedo Texture
└── Binding 5: Shadow Map      ← 新增
```

#### 新增文件
- `assets/shaders/shadow_vert.glsl` — 阴影顶点着色器

#### 修改文件
- `engine/renderer/shadow_map.h/cpp` — 添加光源空间矩阵计算
- `engine/renderer/descriptor_set.h/cpp` — 添加 ShadowUBO 和 ShadowMap 绑定
- `assets/shaders/pbr_frag.glsl` — 阴影采样 + PCF
- `runtime/main.cpp` — 两遍渲染管线

#### Demo 效果
- 方向光产生的阴影
- PCF 软阴影边缘
- 自适应阴影偏移（无 peter-panning）
- 地面和立方体上的阴影

### 设计原理
- 正交投影适合方向光（无透视失真）
- PCF 3x3 软化锯齿状阴影边缘
- 自适应偏移减少 peter-panning 和 shadow acne

## [0.15.0] - 2026-06-27

### Phase 1.3: 多光源 PBR 渲染

#### 光源系统
- **方向光** — 最多 4 个，支持方向/颜色/强度
- **点光源** — 最多 16 个，支持位置/颜色/强度/范围，平方反比衰减
- **聚光灯** — 最多 8 个，支持位置/方向/颜色/强度/内外锥角/范围

#### 着色器更新
- **多光源循环** — 片段着色器遍历所有活跃光源
- **物理衰减** — 平方反比距离衰减 + 平滑截断
- **聚光灯锥角** — 内外锥角平滑过渡
- **环境光** — 可配置的环境光颜色

#### 描述符集架构更新
```
Set 0
├── Binding 0: Scene UBO     (Vertex + Fragment)
├── Binding 1: Light UBO     (Fragment) ← 新增
├── Binding 2: Material UBO  (Fragment)
└── Binding 3: Albedo Texture (Fragment)
```

#### 新增光源数据结构
- `DirectionalLightData` — 方向光数据
- `PointLightData` — 点光源数据
- `SpotLightData` — 聚光灯数据
- `LightUBO` — 光源统一缓冲区

#### Demo 效果
- 🔴 红色立方体（金属度 0.3，粗糙度 0.4）
- 🔵 蓝色立方体（金属度 0.8，粗糙度 0.2）
- 🟡 金色立方体（金属度 1.0，粗糙度 0.1）
- ⬜ 灰色地面（金属度 0.0，粗糙度 0.9）
- 💡 1 个方向光 + 2 个点光源（暖色/冷色）+ 1 个聚光灯

### 设计原理
- 固定大小数组避免动态分配
- 物理正确的光照衰减
- 多材质展示 PBR 参数效果

## [0.14.0] - 2026-06-27

### Phase 1.2: PBR 材质渲染管线

#### PBR 着色器
- **PBR 顶点着色器** — MVP 变换 + 法线矩阵变换
- **PBR 片段着色器** — Cook-Torrance BRDF（GGX 法线分布 + Schlick 几何函数 + Fresnel）
- **多光源支持** — 方向光 + 环境光
- **HDR Tone Mapping** — Reinhard 色调映射
- **Gamma 校正** — sRGB 输出

#### 材质系统
- **SceneUBO** — 视图/投影矩阵、相机位置、光照方向/颜色/强度
- **MaterialUBO** — 反照率、金属度、粗糙度、环境光遮蔽、自发光
- **Push Constants** — 每对象模型矩阵（高效传递）
- **默认纹理** — 1x1 白色纹理作为默认值

#### 描述符集架构
- **Binding 0** — Scene UBO（顶点+片段着色器）
- **Binding 1** — Material UBO（片段着色器）
- **Binding 2** — 反照率纹理采样器（片段着色器）

#### 新增文件
- `assets/shaders/pbr_vert.glsl` — PBR 顶点着色器
- `assets/shaders/pbr_frag.glsl` — PBR 片段着色器

#### 修改文件
- `engine/renderer/descriptor_set.h/cpp` — 重构支持多绑定 + UBO 结构体
- `engine/renderer/pipeline.h/cpp` — 支持 Push Constants + PBR 顶点格式
- `engine/renderer/mesh.h/cpp` — 支持 PBRVertex 构造
- `runtime/main.cpp` — 完整 PBR 渲染管线

#### Demo 效果
- 红色 PBR 材质立方体旋转
- Cook-Torrance 光照模型
- 法线着色正确
- HDR + Gamma 校正输出

### 设计原理
- Push Constants 传递 model 矩阵（128 字节限制内，最高效）
- Scene UBO 在描述符集中（每帧更新一次，所有对象共享）
- Material UBO 支持运行时修改材质属性

## [0.13.0] - 2026-06-27

### Phase 1.1: 纹理数据上传

#### 核心修复
- **一次性命令缓冲** — Device 类添加 `beginSingleTimeCommands`/`endSingleTimeCommands`
- **纹理数据上传** — 通过暂存缓冲 → 图像复制 → 布局转换，实现真正的 GPU 纹理上传
- **图像布局转换** — 实现 `transitionImageLayout`（UNDEFINED → TRANSFER_DST → SHADER_READ_ONLY）
- **缓冲到图像复制** — 实现 `copyBufferToImage`
- **Model 索引缓冲** — glTF 模型加载现在使用索引缓冲绘制

#### 修改文件
- `engine/renderer/vulkan/device.h/cpp` — 添加一次性命令缓冲工具
- `engine/renderer/texture.h/cpp` — 完整实现纹理数据上传管线
- `engine/renderer/model.cpp` — 使用索引缓冲，法线映射为颜色

#### 技术细节
- 纹理使用 STAGING BUFFER + DEVICE_LOCAL 内存（最佳性能）
- 图像布局转换使用正确的管线屏障
- 一次性命令缓冲使用独立命令池

### 设计原理
- 暂存缓冲避免频繁映射 DEVICE_LOCAL 内存
- 管线屏障确保数据传输完成后再使用
- 独立命令池避免与渲染命令池冲突

## [0.12.0] - 2026-06-27

### Demo 级渲染管线

#### 核心渲染修复
- **深度缓冲** — Render Pass 添加 depth attachment，3D 物体遮挡关系正确
- **索引缓冲** — Mesh 类支持 `vkCmdDrawIndexed`，减少顶点重复
- **Uniform Buffer** — 模板类支持双缓冲，每帧更新 MVP 矩阵
- **Descriptor Set** — 封装 layout/pool/descriptor sets，支持 UBO 绑定
- **Pipeline 深度测试** — 启用 depthTestEnable + depthWriteEnable
- **着色器 UBO** — 顶点着色器使用 UBO 进行 MVP 变换

#### 新增文件
- `engine/renderer/uniform_buffer.h/cpp` — Uniform Buffer 封装
- `engine/renderer/descriptor_set.h/cpp` — Descriptor Set 封装

#### 修改文件
- `engine/renderer/renderer.h/cpp` — 添加深度缓冲资源管理
- `engine/renderer/pipeline.h/cpp` — 支持描述符集布局和深度测试
- `engine/renderer/mesh.h/cpp` — 支持索引缓冲
- `assets/shaders/vert.glsl` — 添加 UBO 绑定
- `runtime/main.cpp` — 完整渲染管线连接
- `engine/CMakeLists.txt` — 添加新源文件

#### Demo 效果
- 3D 彩色立方体持续旋转
- 深度测试正确（背面不可见）
- 透视投影 + 相机控制（WASD + 鼠标右键）
- 窗口可调整大小

### 设计原理
- 双缓冲 UBO 避免帧间数据竞争
- Descriptor Set 分离资源绑定与管线
- 深度缓冲使用 DEVICE_LOCAL 内存提升性能

## [0.11.0] - 2026-06-27

### 精美 UI 系统

#### 主题系统
- 精心设计的暗色调色板（参考 Unreal Engine 风格）
- 亮色主题支持
- 统一的样式配置（圆角、间距、边框）
- 主题切换 API

#### 自定义控件库
- Vec3 输入控件（带颜色标签 X/Y/Z）
- 搜索框（带圆角样式）
- 图标按钮和工具栏按钮
- 带图标的树节点和可选列表
- 组件标题栏（带折叠和删除按钮）
- 信息面板和状态指示器
- 统计卡片和进度条
- 空状态提示

#### 新增面板
- **工具栏面板** - 场景控制（播放/暂停/停止）、变换工具切换、视图选项、快捷操作
- **状态栏面板** - FPS 显示、实体数量、内存使用、选中物体信息
- **控制台面板** - 日志输出（Info/Warning/Error）、命令执行、自动滚动、过滤功能
- **设置面板** - 主题/字体/鼠标设置、渲染选项、编辑器偏好、快捷键参考

#### 改进现有面板
- **Inspector** - 使用自定义控件、组件图标、改进折叠头部样式、颜色标签
- **Scene Hierarchy** - 实体图标、搜索过滤、拖拽支持、右键菜单增强
- **Asset Browser** - 网格/列表视图切换、文件类型图标、缩略图预览、面包屑导航
- **Viewport** - Overlay 工具栏、Gizmo 支持、视图选项

#### 编辑器集成
- 默认停靠布局配置
- 所有面板无缝集成
- 回调系统连接（实体选择、场景控制）

### 设计原理
- 参考现代游戏引擎（Unreal/Unity）的视觉风格
- 统一的主题系统确保视觉一致性
- 自定义控件提升用户体验
- 模块化面板架构便于扩展

## [0.10.0] - 2026-06-27

### 生产级功能
- 性能分析器
  - 作用域计时
  - Chrome 追踪格式输出
  - 性能统计 (FPS/Draw Call/三角形)
- 调试控制台
  - 命令注册/执行
  - 日志系统
  - 自动补全
  - 命令历史
- 序列化系统
  - JSON 序列化/反序列化
  - 场景保存/加载
  - 数学类型序列化
- 插件系统
  - 动态库加载
  - 插件生命周期管理
  - 插件注册宏

### 设计原理
- 性能分析器帮助优化瓶颈
- 调试控制台提升开发效率
- 序列化支持场景持久化
- 插件系统支持功能扩展

## [0.9.0] - 2026-06-27

### 功能扩展
- 粒子系统
  - 可配置发射器
  - 生命周期管理
  - 颜色/大小插值
- 地形系统
  - 高度图加载
  - 程序化生成 (Perlin 噪声)
  - 法线计算
- 天空盒系统
  - 立方体贴图支持
  - 渐变色天空
- 骨骼动画系统
  - 关键帧插值
  - 骨骼层级
  - 动画混合

### 性能优化
- 视锥体剔除
  - 平面提取
  - 包围盒/包围球测试
- LOD 系统
  - 多级细节管理
  - 距离-based 切换
- 实例化渲染
  - 批量渲染
  - 实例缓冲区
- 多线程渲染
  - 线程池
  - 命令缓冲区并行录制
- 内存池系统
  - 对象池
  - 内存池
- 资源流式加载
  - 异步加载
  - 优先级队列
  - 多线程加载

### 设计原理
- 粒子系统支持复杂特效
- 地形系统支持大世界
- 动画系统支持角色动画
- 多线程提升渲染性能
- 资源流式加载避免卡顿

## [0.8.0] - 2026-06-27

### 功能
- 阴影映射系统
- 后处理框架
  - Bloom 效果
  - Tone Mapping
  - Gamma 校正
- 着色器编译集成到 CMake

### 设计原理
- 阴影映射提升场景真实感
- 后处理管线便于扩展效果

## [0.7.0] - 2026-06-27

### 功能
- ImGui 集成
- 编辑器面板系统
  - Viewport 面板
  - Inspector 面板
  - Scene Hierarchy 面板
  - Asset Browser 面板
- 编辑器应用框架

### 设计原理
- ImGui 作为编辑器 UI 框架
- 面板化架构便于扩展
- 实体选择和属性编辑

## [0.6.0] - 2026-06-27

### 功能
- Lua 脚本引擎框架
- 脚本组件
- 脚本 API 绑定接口

### 设计原理
- 脚本系统解耦游戏逻辑
- 组件化脚本绑定

## [0.5.0] - 2026-06-27

### 功能
- 音频引擎框架
- 音频源管理
- 3D 音频监听器
- 音频位置/方向设置

### 设计原理
- 单例音频引擎
- 支持 3D 空间音频

## [0.4.0] - 2026-06-27

### 功能
- 物理世界管理
- 刚体系统
- 碰撞检测 (Box/Sphere/Capsule)
- 物理调试绘制

### 设计原理
- 简化物理引擎接口
- 支持多种碰撞体类型
- 调试可视化

## [0.3.0] - 2026-06-27

### 功能
- 相机系统 (FPS 风格)
- 光照系统 (方向光/点光/聚光灯)
- 材质系统 (PBR 基础)
- 纹理加载
- glTF 模型加载
- ECS 组件扩展 (Tag/Camera/Light/MeshRenderer/RigidBody/Collider/Script)
- 系统管理器

### 设计原理
- 组件化架构便于扩展
- PBR 材质基础
- glTF 作为标准模型格式

## [0.2.0] - 2026-06-27

### 功能
- 着色器加载系统 (GLSL → SPIR-V)
- 图形管线创建
- 顶点缓冲区管理
- Mesh 类用于顶点数据
- 彩色三角形渲染
- 着色器编译脚本 (batch/shell)
- CMake 着色器编译集成

### 设计原理
- 着色器使用 SPIR-V 中间格式，支持多语言编译
- 管线封装便于后续扩展
- Mesh 类抽象顶点数据管理

## [0.1.0] - 2026-06-27

### 功能
- 项目初始化和 CMake 构建系统
- 日志系统 (基于 spdlog)
- 事件驱动架构
- GLFW 窗口和输入管理
- Vulkan 实例创建 (含验证层)
- Vulkan 物理/逻辑设备选择
- 交换链管理
- 基础渲染循环
- ECS 框架基础
- 场景管理基础
- 资源管理器骨架

### 设计原理
- 使用现代 C++20 特性
- 模块化架构，便于扩展
- 事件驱动解耦各系统
- FetchContent 自动管理依赖

### 注意事项
- 需要安装 Vulkan SDK 1.3+
- 需要 C++20 兼容编译器
- 首次构建会自动下载第三方库，需要网络连接
