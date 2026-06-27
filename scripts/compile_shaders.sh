#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SHADER_DIR="$SCRIPT_DIR/../assets/shaders"
OUTPUT_DIR="$SCRIPT_DIR/../assets/shaders"

echo "Compiling shaders..."

# Check if glslc is available
if ! command -v glslc &> /dev/null; then
    echo "Error: glslc not found. Please install Vulkan SDK and add it to PATH."
    echo "Download from: https://vulkan.lunarg.com/sdk/home"
    exit 1
fi

# Compile vertex shader
echo "Compiling vert.glsl..."
glslc "$SHADER_DIR/vert.glsl" -o "$OUTPUT_DIR/vert.spv"
if [ $? -ne 0 ]; then
    echo "Failed to compile vertex shader!"
    exit 1
fi

# Compile fragment shader
echo "Compiling frag.glsl..."
glslc "$SHADER_DIR/frag.glsl" -o "$OUTPUT_DIR/frag.spv"
if [ $? -ne 0 ]; then
    echo "Failed to compile fragment shader!"
    exit 1
fi

echo "Shaders compiled successfully!"
echo "Output: $OUTPUT_DIR/vert.spv, $OUTPUT_DIR/frag.spv"
