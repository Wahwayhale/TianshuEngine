#pragma once

#include "math/math_types.h"
#include <memory>
#include <string>

namespace spark {

class Texture;

class Material {
public:
    Material();
    ~Material();

    // PBR properties
    Vec3 albedo = Vec3(1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;

    // Emission
    Vec3 emission = Vec3(0.0f);
    float emissionIntensity = 0.0f;

    // Textures
    std::shared_ptr<Texture> albedoTexture;
    std::shared_ptr<Texture> normalTexture;
    std::shared_ptr<Texture> metallicRoughnessTexture;
    std::shared_ptr<Texture> aoTexture;
    std::shared_ptr<Texture> emissionTexture;

    // Alpha
    float alpha = 1.0f;
    bool doubleSided = false;

    // Getters for shader
    Vec3 getAlbedo() const { return albedo; }
    float getMetallic() const { return metallic; }
    float getRoughness() const { return roughness; }
};

} // namespace spark
