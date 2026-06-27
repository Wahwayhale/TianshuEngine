@echo off
setlocal

set SHADER_DIR=%~dp0..\assets\shaders
set OUTPUT_DIR=%~dp0..\assets\shaders

echo Compiling shaders...

:: Check if glslc is available
where glslc >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: glslc not found. Please install Vulkan SDK and add it to PATH.
    echo Download from: https://vulkan.lunarg.com/sdk/home
    exit /b 1
)

:: Compile vertex shader
echo Compiling vert.glsl...
glslc "%SHADER_DIR%\vert.glsl" -o "%OUTPUT_DIR%\vert.spv"
if %ERRORLEVEL% neq 0 (
    echo Failed to compile vertex shader!
    exit /b 1
)

:: Compile fragment shader
echo Compiling frag.glsl...
glslc "%SHADER_DIR%\frag.glsl" -o "%OUTPUT_DIR%\frag.spv"
if %ERRORLEVEL% neq 0 (
    echo Failed to compile fragment shader!
    exit /b 1
)

echo Shaders compiled successfully!
echo Output: %OUTPUT_DIR%\vert.spv, %OUTPUT_DIR%\frag.spv

endlocal
