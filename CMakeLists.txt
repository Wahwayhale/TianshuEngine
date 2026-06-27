cmake_minimum_required(VERSION 3.20)
project(TianshuEngine VERSION 0.32.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_EDITOR "Build the editor" ON)
option(PLATFORM_ANDROID "Build for Android" OFF)
option(PLATFORM_WEB "Build for Web/Emscripten" OFF)

# 平台检测
if(PLATFORM_ANDROID)
    add_definitions(-DPLATFORM_ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    message(STATUS "Building for Android")
elseif(PLATFORM_WEB)
    add_definitions(-DPLATFORM_WEB)
    message(STATUS "Building for Web/Emscripten")
elseif(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
    message(STATUS "Building for Windows")
elseif(UNIX AND NOT APPLE)
    add_definitions(-DPLATFORM_LINUX)
    add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
    message(STATUS "Building for Linux")
elseif(APPLE)
    add_definitions(-DPLATFORM_MACOS)
    add_definitions(-DVK_USE_PLATFORM_METAL_EXT)
    message(STATUS "Building for macOS")
endif()

# Find Vulkan (非 Web 平台)
if(NOT PLATFORM_WEB)
    find_package(Vulkan REQUIRED)
endif()

# Third party libraries
add_subdirectory(third_party)

# Engine library
add_subdirectory(engine)

# 平台特定构建
if(PLATFORM_ANDROID)
    # Android 使用单独的 CMakeLists
    message(STATUS "Android build: use platforms/android/CMakeLists.txt")
elseif(PLATFORM_WEB)
    # Web 使用单独的 CMakeLists
    message(STATUS "Web build: use platforms/web/CMakeLists.txt")
else()
    # 桌面平台
    add_subdirectory(runtime)

    if(BUILD_EDITOR)
        add_subdirectory(editor)
    endif()
endif()
