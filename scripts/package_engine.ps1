# 天枢引擎打包脚本
# 用法: .\scripts\package_engine.ps1 [-Version "0.32.0"] [-Platform "windows"]

param(
    [string]$Version = "0.32.0",
    [string]$Platform = "windows",
    [switch]$IncludeExamples,
    [switch]$IncludeDocs
)

$ErrorActionPreference = "Stop"

# 配置
$EngineName = "TianshuEngine"
$OutputDir = "release"
$PackageDir = "$OutputDir/$EngineName-$Version-$Platform"

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  天枢引擎打包系统 v$Version" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# 清理旧的打包目录
if (Test-Path $PackageDir) {
    Remove-Item -Recurse -Force $PackageDir
}
New-Item -ItemType Directory -Path $PackageDir -Force | Out-Null

# 创建目录结构
Write-Host "[1/8] 创建目录结构..." -ForegroundColor Yellow
$dirs = @(
    "$PackageDir/bin",
    "$PackageDir/lib",
    "$PackageDir/include",
    "$PackageDir/assets",
    "$PackageDir/assets/shaders",
    "$PackageDir/assets/textures",
    "$PackageDir/assets/models",
    "$PackageDir/assets/fonts",
    "$PackageDir/examples",
    "$PackageDir/docs",
    "$PackageDir/templates"
)

foreach ($dir in $dirs) {
    New-Item -ItemType Directory -Path $dir -Force | Out-Null
}

# 复制引擎头文件
Write-Host "[2/8] 复制引擎头文件..." -ForegroundColor Yellow
$includeDirs = @(
    "engine/core",
    "engine/renderer",
    "engine/ecs",
    "engine/scene",
    "engine/math",
    "engine/physics",
    "engine/audio",
    "engine/script",
    "engine/resource",
    "engine/ui",
    "engine/ai",
    "engine/platform"
)

foreach ($dir in $includeDirs) {
    if (Test-Path $dir) {
        $destDir = "$PackageDir/include/" + ($dir -replace "engine/", "")
        Copy-Item -Path $dir -Destination $destDir -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# 复制着色器
Write-Host "[3/8] 复制着色器..." -ForegroundColor Yellow
if (Test-Path "assets/shaders") {
    Copy-Item -Path "assets/shaders/*" -Destination "$PackageDir/assets/shaders/" -Recurse -Force
}

# 复制编辑器 UI 资源
Write-Host "[4/8] 复制编辑器资源..." -ForegroundColor Yellow
if (Test-Path "editor/panels") {
    Copy-Item -Path "editor/panels/*.h" -Destination "$PackageDir/include/panels/" -Force -ErrorAction SilentlyContinue
}

# 复制示例
if ($IncludeExamples) {
    Write-Host "[5/8] 复制示例项目..." -ForegroundColor Yellow
    if (Test-Path "examples") {
        Copy-Item -Path "examples/*" -Destination "$PackageDir/examples/" -Recurse -Force
    }
}

# 复制文档
if ($IncludeDocs) {
    Write-Host "[6/8] 复制文档..." -ForegroundColor Yellow
    if (Test-Path "docs") {
        Copy-Item -Path "docs/*" -Destination "$PackageDir/docs/" -Recurse -Force
    }
    if (Test-Path "README.md") {
        Copy-Item -Path "README.md" -Destination "$PackageDir/"
    }
    if (Test-Path "CHANGELOG.md") {
        Copy-Item -Path "CHANGELOG.md" -Destination "$PackageDir/"
    }
    if (Test-Path "LICENSE") {
        Copy-Item -Path "LICENSE" -Destination "$PackageDir/"
    }
}

# 创建项目模板
Write-Host "[7/8] 创建项目模板..." -ForegroundColor Yellow
$templateCmake = @"
cmake_minimum_required(VERSION 3.20)
project(MyGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置引擎路径
set(TIANSHU_ENGINE_DIR "`${CMAKE_CURRENT_SOURCE_DIR}/engine")

# 包含引擎
include_directories(`${TIANSHU_ENGINE_DIR}/include)
link_directories(`${TIANSHU_ENGINE_DIR}/lib)

# 查找依赖
find_package(Vulkan REQUIRED)

# 游戏源文件
set(GAME_SOURCES
    src/main.cpp
)

# 创建可执行文件
add_executable(MyGame `${GAME_SOURCES})

# 链接引擎库
target_link_libraries(MyGame PRIVATE
    TianshuEngine
    Vulkan::Vulkan
    glfw
    glm::glm
    spdlog::spdlog
)
"@

$templateMain = @"
#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"

using namespace spark;

class MyGame : public Application {
public:
    MyGame() : Application("My Game", 1280, 720) {
        // 创建场景
        m_scene = std::make_unique<Scene>();

        // 创建实体
        auto& player = m_scene->createEntity("Player");
        player.addComponent<TransformComponent>(Vec3(0.0f, 0.0f, 0.0f));

        SPARK_INFO("Game started!");
    }

protected:
    void onUpdate(float deltaTime) override {
        m_scene->update(deltaTime);
    }

    void onRender() override {
        // 渲染逻辑
    }

private:
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        MyGame game;
        game.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return 1;
    }
    return 0;
}
"@

Set-Content -Path "$PackageDir/templates/CMakeLists.txt" -Value $templateCmake
New-Item -ItemType Directory -Path "$PackageDir/templates/src" -Force | Out-Null
Set-Content -Path "$PackageDir/templates/src/main.cpp" -Value $templateMain

# 创建快速开始指南
$quickStart = @"
# 天枢引擎 - 快速开始

## 安装

1. 解压此包到任意目录
2. 确保已安装:
   - Vulkan SDK 1.3+
   - CMake 3.20+
   - Visual Studio 2022 (Windows) 或 GCC 12+ (Linux)

## 创建新项目

1. 复制 `templates/` 目录到你的项目位置
2. 修改 `CMakeLists.txt` 中的 `TIANSHU_ENGINE_DIR` 指向引擎目录
3. 编辑 `src/main.cpp` 开始开发

## 构建项目

``````bash
mkdir build
cd build
cmake ..
cmake --build .
``````

## 引擎结构

``````
include/          - 引擎头文件
lib/              - 引擎库文件
assets/           - 引擎资源
  shaders/        - 着色器
  textures/       - 纹理
  models/         - 模型
  fonts/          - 字体
examples/         - 示例项目
docs/             - 文档
templates/        - 项目模板
``````

## API 概览

### 核心
- `Application` - 应用基类
- `Scene` - 场景管理
- `Entity` - 实体

### 渲染
- `Renderer` - 渲染器
- `Camera` - 相机
- `Material` - 材质
- `Mesh` - 网格

### 物理
- `PhysicsWorld` - 物理世界
- `RigidBody` - 刚体
- `Collider` - 碰撞体

### 脚本
- `ScriptEngine` - 脚本引擎
- `LuaScriptComponent` - Lua 脚本组件

## 示例

``````cpp
#include "core/application.h"
#include "scene/scene.h"
#include "ecs/components.h"

using namespace spark;

class MyApp : public Application {
public:
    MyApp() : Application("My Game", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        auto& entity = m_scene->createEntity("Cube");
        entity.addComponent<TransformComponent>();
        entity.addComponent<MeshRendererComponent>();
    }

    void onUpdate(float dt) override {
        m_scene->update(dt);
    }

private:
    std::unique_ptr<Scene> m_scene;
};
``````

## 更多信息

- 文档: docs/
- 示例: examples/
- GitHub: https://github.com/Wahwayhale/TianshuEngine
"@

Set-Content -Path "$PackageDir/QUICKSTART.md" -Value $quickStart

# 创建版本信息
$versionInfo = @"
{
    "name": "TianshuEngine",
    "version": "$Version",
    "platform": "$Platform",
    "build_date": "$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')",
    "vulkan_version": "1.3",
    "cpp_standard": "C++20"
}
"@

Set-Content -Path "$PackageDir/version.json" -Value $versionInfo

# 压缩打包
Write-Host "[8/8] 压缩打包..." -ForegroundColor Yellow
$zipFile = "$OutputDir/$EngineName-$Version-$Platform.zip"
if (Test-Path $zipFile) {
    Remove-Item -Force $zipFile
}

Compress-Archive -Path $PackageDir -DestinationPath $zipFile

# 计算大小
$zipSize = (Get-Item $zipFile).Length / 1MB
$dirSize = (Get-ChildItem -Path $PackageDir -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB

Write-Host ""
Write-Host "=====================================" -ForegroundColor Green
Write-Host "  打包完成!" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host ""
Write-Host "输出文件:" -ForegroundColor White
Write-Host "  目录: $PackageDir" -ForegroundColor Gray
Write-Host "  压缩包: $zipFile" -ForegroundColor Gray
Write-Host ""
Write-Host "大小:" -ForegroundColor White
Write-Host "  目录: $([math]::Round($dirSize, 2)) MB" -ForegroundColor Gray
Write-Host "  压缩包: $([math]::Round($zipSize, 2)) MB" -ForegroundColor Gray
Write-Host ""
Write-Host "内容:" -ForegroundColor White
Write-Host "  - 引擎库和头文件" -ForegroundColor Gray
Write-Host "  - 着色器" -ForegroundColor Gray
Write-Host "  - 项目模板" -ForegroundColor Gray
if ($IncludeExamples) {
    Write-Host "  - 示例项目" -ForegroundColor Gray
}
if ($IncludeDocs) {
    Write-Host "  - 文档" -ForegroundColor Gray
}
Write-Host ""
