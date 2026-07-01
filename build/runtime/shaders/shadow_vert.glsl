#version 450

// Push constants
layout(push_constant) uniform PushConstants {
    mat4 model;
} push;

// Light space matrix
layout(set = 0, binding = 0) uniform ShadowUBO {
    mat4 lightSpaceMatrix;
} shadow;

// Vertex attributes
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

void main() {
    gl_Position = shadow.lightSpaceMatrix * push.model * vec4(inPosition, 1.0);
}
