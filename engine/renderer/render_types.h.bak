#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>

namespace spark {

// 光源数量限制
constexpr int MAX_DIRECTIONAL_LIGHTS = 4;
constexpr int MAX_POINT_LIGHTS = 16;
constexpr int MAX_SPOT_LIGHTS = 8;
constexpr int MAX_BONES = 100;

// Push Constants
struct PushConstants {
    Mat4 model;
};

// 顶点结构
struct Vertex {
    float position[3];
    float color[3];
    float texCoord[2];
};

struct PBRVertex {
    float position[3];
    float normal[3];
    float texCoord[2];
};

// 光源结构体
struct DirectionalLightData {
    Vec4 directionAndIntensity;
    Vec4 color;
};

struct PointLightData {
    Vec4 positionAndRange;
    Vec4 colorAndIntensity;
};

struct SpotLightData {
    Vec4 positionAndRange;
    Vec4 directionAndIntensity;
    Vec4 colorAndCutoff;
    Vec4 outerCutoff;
};

// UBO 结构体
struct SceneUBO {
    Mat4 view;
    Mat4 projection;
    Vec3 cameraPos;
    float padding0 = 0.0f;
    Vec3 ambientColor;
    float padding1 = 0.0f;
};

struct LightUBO {
    DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLightData pointLights[MAX_POINT_LIGHTS];
    SpotLightData spotLights[MAX_SPOT_LIGHTS];
    int directionalLightCount = 0;
    int pointLightCount = 0;
    int spotLightCount = 0;
    float padding = 0.0f;
};

struct MaterialUBO {
    Vec4 albedo = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    float emission = 0.0f;
};

struct ShadowUBO {
    Mat4 lightSpaceMatrix;
    float shadowBias = 0.005f;
    float shadowStrength = 0.8f;
    Vec2 padding;
};

} // namespace spark
