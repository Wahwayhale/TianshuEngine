# 天枢引擎 - 渲染系统

## 概述

天枢引擎使用 Vulkan 1.3 作为渲染后端，支持 PBR 渲染、阴影映射、后处理等功能。

## 渲染管线

### 前向渲染

```
场景数据 → 顶点着色器 → 光栅化 → 片段着色器 → 输出
```

### 延迟渲染

```
场景数据 → G-Buffer → 光照计算 → 后处理 → 输出
```

## PBR 材质

### 材质属性

```cpp
struct MaterialProperties {
    Vec4 albedo;        // 基础颜色
    float metallic;     // 金属度 (0-1)
    float roughness;    // 粗糙度 (0-1)
    float ao;           // 环境光遮蔽
    Vec3 emission;      // 自发光
    float emissionIntensity;
};
```

### 创建材质

```cpp
auto material = std::make_shared<Material>("MyMaterial");
material->getProperties().albedo = Vec4(0.8f, 0.2f, 0.2f, 1.0f);
material->getProperties().metallic = 0.0f;
material->getProperties().roughness = 0.5f;
```

### 预设材质

```cpp
auto metal = Material::createMetal(Vec3(0.8f), 0.2f);
auto plastic = Material::createPlastic(Vec3(0.8f, 0.2f, 0.2f));
auto glass = Material::createGlass(0.3f);
auto emissive = Material::createEmissive(Vec3(1.0f, 0.9f, 0.7f), 2.0f);
```

## 光源

### 方向光

```cpp
auto dirLight = LightManager::get().createDirectionalLight("Sun");
dirLight->setDirection(Vec3(-0.5f, -1.0f, -0.3f));
dirLight->setColor(Vec3(1.0f, 0.95f, 0.9f));
dirLight->setIntensity(2.0f);
```

### 点光源

```cpp
auto pointLight = LightManager::get().createPointLight("Lamp");
pointLight->setPosition(Vec3(3.0f, 2.0f, 3.0f));
pointLight->setColor(Vec3(1.0f, 0.8f, 0.6f));
pointLight->setRange(10.0f);
```

### 聚光灯

```cpp
auto spotLight = LightManager::get().createSpotLight("Flashlight");
spotLight->setPosition(Vec3(0.0f, 5.0f, 0.0f));
spotLight->setDirection(Vec3(0.0f, -1.0f, 0.0f));
spotLight->setInnerCutoff(12.5f);
spotLight->setOuterCutoff(17.5f);
```

## 阴影

### 阴影映射

```cpp
auto shadowMap = std::make_unique<ShadowMap>(device, 2048, 2048);
Mat4 lightSpaceMatrix = shadowMap->getLightSpaceMatrix(lightDir);
```

### 级联阴影

```cpp
dirLight->setCascadeCount(4);
dirLight->setShadowDistance(100.0f);
```

## 后处理

### 设置

```cpp
PostProcessSettings settings;
settings.bloom.enabled = true;
settings.bloom.threshold = 1.0f;
settings.bloom.intensity = 0.04f;
settings.colorGrading.exposure = 1.5f;
settings.colorGrading.toneMappingMode = ToneMappingMode::ACES;
```

### 效果

- **Bloom** — 辉光效果
- **SSAO** — 屏幕空间环境光遮蔽
- **SSR** — 屏幕空间反射
- **TAA** — 时间抗锯齿
- **色调映射** — ACES/Reinhard/AgX
- **色彩校正** — 曝光/对比度/饱和度

## 着色器

### 顶点着色器

```glsl
#version 450

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 projection;
    vec3 cameraPos;
} scene;

layout(push_constant) uniform PushConstants {
    mat4 model;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

void main() {
    gl_Position = scene.projection * scene.view * push.model * vec4(inPosition, 1.0);
}
```

### 片段着色器

```glsl
#version 450

layout(set = 0, binding = 2) uniform MaterialUBO {
    vec4 albedo;
    float metallic;
    float roughness;
} material;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = material.albedo;
}
```
