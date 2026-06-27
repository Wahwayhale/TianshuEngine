@echo off
echo =====================================
echo   Spark Engine - Build Script
echo =====================================
echo.

cd /d "%~dp0.."

:: Check if CMake is installed
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found! Please run install_build_tools.bat first.
    pause
    exit /b 1
)

:: Create build directory
if not exist build mkdir build

echo [1/3] Configuring project...
cmake --preset windows-release
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [2/3] Building project...
cmake --build --preset windows-release
if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Build complete!
echo.
echo Output files:
echo   - build\windows-release\editor\Release\SparkEditor.exe
echo   - build\windows-release\runtime\Release\SparkRuntime.exe
echo.
pause
