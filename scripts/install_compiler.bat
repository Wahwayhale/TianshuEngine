@echo off
echo =====================================
echo   安装 C++ 编译器
echo =====================================
echo.

cd /d "%~dp0.."

:: 以管理员权限运行
powershell -ExecutionPolicy Bypass -Command "Start-Process PowerShell -ArgumentList '-ExecutionPolicy Bypass -File \"%~dp0install_compiler.ps1\"' -Verb RunAs"

echo.
echo 安装脚本已在新窗口打开
echo 请按照提示完成安装
pause
