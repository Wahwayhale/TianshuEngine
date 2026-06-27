# Spark Engine Scripts

## Installation

### Quick Install (Recommended)

1. Double-click `install_build_tools.bat`
2. Wait for installation to complete (10-30 minutes)
3. Restart your terminal
4. Run `build.bat` to build the engine

### Manual Install

If the automatic installer doesn't work, install manually:

#### 1. CMake
- Download: https://cmake.org/download/
- Install with "Add CMake to the system PATH" checked

#### 2. Visual Studio 2022
- Download: https://visualstudio.microsoft.com/downloads/
- Install "Desktop development with C++" workload

#### 3. Vulkan SDK
- Download: https://vulkan.lunarg.com/sdk/home
- Install with default settings

## Building

### Using Build Script
```batch
scripts\build.bat
```

### Manual Build
```batch
cd E:\项目\系统
cmake --preset windows-release
cmake --build --preset windows-release
```

### Build Presets
- `windows-debug` - Debug build with validation
- `windows-release` - Release build
- `windows-relwithdebinfo` - Release with debug symbols

## Output

After building, you'll find:
- `build/windows-release/editor/Release/SparkEditor.exe` - Editor
- `build/windows-release/runtime/Release/SparkRuntime.exe` - Runtime

## Troubleshooting

### CMake not found
- Restart your terminal after installing CMake
- Check if CMake is in PATH: `cmake --version`

### Visual Studio not found
- Make sure "Desktop development with C++" workload is installed
- Try running from "Developer Command Prompt for VS 2022"

### Vulkan SDK not found
- Install Vulkan SDK from https://vulkan.lunarg.com/sdk/home
- Set VULKAN_SDK environment variable

### Build errors
- Make sure all dependencies are installed
- Check the error messages for specific issues
- Try cleaning the build directory: `rd /s /q build`
