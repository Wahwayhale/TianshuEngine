@echo off
echo =====================================
echo   天枢引擎打包系统
echo =====================================
echo.

cd /d "%~dp0.."

:: 检查参数
if "%1"=="" (
    set VERSION=0.32.0
) else (
    set VERSION=%1
)

if "%2"=="" (
    set PLATFORM=windows
) else (
    set PLATFORM=%2
)

echo 版本: %VERSION%
echo 平台: %PLATFORM%
echo.

:: 执行打包脚本
powershell -ExecutionPolicy Bypass -File "scripts\package_engine.ps1" -Version "%VERSION%" -Platform "%PLATFORM%" -IncludeExamples -IncludeDocs

echo.
pause
