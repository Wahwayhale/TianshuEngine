#version 450

// Material UBO
layout(set = 0, binding = 1) uniform MaterialUBO {
    vec4 albedo;
    float metallic;
    float roughness;
    float ao;
    float emission;
} material;

// Albedo texture
layout(set = 0, binding = 2) uniform sampler2D albedoTexture;

// Inputs from vertex shader
layout(location = 0) in vec3 fragWorldPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

// G-Buffer outputs
layout(location = 0) out vec4 outAlbedo;     // RGB = Albedo, A = Metallic
layout(location = 1) out vec4 outNormal;     // RGB = Normal, A = Roughness
layout(location = 2) out vec4 outPosition;   // RGB = World Position, A = AO

void main() {
    // 采样纹理
    vec4 texColor = texture(albedoTexture, fragTexCoord);
    vec3 albedo = material.albedo.rgb * texColor.rgb;

    // 法线归一化
    vec3 normal = normalize(fragNormal);

    // 输出到 G-Buffer
    outAlbedo = vec4(albedo, material.metallic);
    outNormal = vec4(normal * 0.5 + 0.5, material.roughness);  // 编码到 [0,1]
    outPosition = vec4(fragWorldPos, material.ao);
}
