@echo off
echo =====================================
echo   Spark Engine - Build Tools Installer
echo =====================================
echo.
echo This script will install:
echo   - CMake 3.28
echo   - Visual Studio 2022 Build Tools
echo   - Check Vulkan SDK
echo.
echo Press any key to continue...
pause > nul

:: Run PowerShell script as Administrator
powershell -ExecutionPolicy Bypass -Command "Start-Process PowerShell -ArgumentList '-ExecutionPolicy Bypass -File \"%~dp0install_build_tools.ps1\"' -Verb RunAs"

echo.
echo Installation started in a new window.
echo Please wait for it to complete...
pause
