@echo off
echo =====================================
echo   天枢引擎 - 一键构建
echo =====================================
echo.

cd /d "%~dp0.."

:: 检查 CMake
set CMAKE="E:\CMAKE\bin\cmake.exe"
if not exist %CMAKE% (
    echo [错误] CMake 未找到
    echo 请先安装 CMake: https://cmake.org/download/
    pause
    exit /b 1
)

:: 检查编译器
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    where cl >nul 2>&1
    if %errorlevel% neq 0 (
        echo [错误] 未找到 C++ 编译器
        echo.
        echo 请运行: scripts\install_compiler.bat
        echo.
        pause
        exit /b 1
    )
)

echo [1/4] 配置 CMake...
if not exist build mkdir build

:: 尝试不同的生成器
%CMAKE% -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release 2>nul
if %errorlevel% neq 0 (
    %CMAKE% -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release 2>nul
    if %errorlevel% neq 0 (
        %CMAKE% -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release 2>nul
        if %errorlevel% neq 0 (
            echo [错误] CMake 配置失败
            echo 请检查编译器是否正确安装
            pause
            exit /b 1
        )
    )
)

echo [2/4] 编译引擎...
%CMAKE% --build build --config Release

if %errorlevel% neq 0 (
    echo [错误] 编译失败
    pause
    exit /b 1
)

echo [3/4] 复制输出...
if not exist release mkdir release

:: 复制可执行文件
copy build\runtime\SparkRuntime.exe release\ 2>nul
copy build\editor\SparkEditor.exe release\ 2>nul

echo [4/4] 构建完成!
echo.
echo 输出文件:
echo   - release\SparkRuntime.exe
echo   - release\SparkEditor.exe
echo.
pause
