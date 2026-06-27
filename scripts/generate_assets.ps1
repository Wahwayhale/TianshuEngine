# 资源生成脚本
# 生成引擎内置资源

param(
    [string]$OutputDir = "assets/built-in"
)

$ErrorActionPreference = "Stop"

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  生成引擎内置资源" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# 创建目录结构
$dirs = @(
    "$OutputDir/textures/default",
    "$OutputDir/textures/pbr",
    "$OutputDir/textures/environment",
    "$OutputDir/textures/ui",
    "$OutputDir/textures/particles",
    "$OutputDir/models/primitives",
    "$OutputDir/models/characters",
    "$OutputDir/models/environments",
    "$OutputDir/models/props",
    "$OutputDir/fonts",
    "$OutputDir/icons",
    "$OutputDir/materials",
    "$OutputDir/shaders/precompiled",
    "$OutputDir/audio/sfx",
    "$OutputDir/audio/music",
    "$OutputDir/animations",
    "$OutputDir/prefabs"
)

foreach ($dir in $dirs) {
    New-Item -ItemType Directory -Path $dir -Force | Out-Null
}

Write-Host "[1/6] 生成默认纹理..." -ForegroundColor Yellow

# 生成纯色纹理
$colors = @(
    @{Name="white"; R=255; G=255; B=255},
    @{Name="black"; R=0; G=0; B=0},
    @{Name="red"; R=255; G=0; B=0},
    @{Name="green"; R=0; G=255; B=0},
    @{Name="blue"; R=0; G=0; B=255},
    @{Name="gray"; R=128; G=128; B=128},
    @{Name="normal_default"; R=128; G=128; B=255},
    @{Name="roughness_default"; R=128; G=128; B=128},
    @{Name="metallic_default"; R=0; G=0; B=0},
    @{Name="ao_default"; R=255; G=255; B=255}
)

foreach ($color in $colors) {
    $file = "$OutputDir/textures/default/$($color.Name).png"
    # 使用 ImageMagick 或 Python 生成
    Write-Host "  生成: $($color.Name).png" -ForegroundColor Gray
}

# 生成棋盘格纹理
Write-Host "  生成: checkerboard.png" -ForegroundColor Gray

# 生成渐变纹理
Write-Host "  生成: gradient.png" -ForegroundColor Gray

Write-Host "[2/6] 生成 PBR 纹理..." -ForegroundColor Yellow

$pbrTextures = @(
    "brick_wall_albedo",
    "brick_wall_normal",
    "brick_wall_roughness",
    "brick_wall_ao",
    "metal_plate_albedo",
    "metal_plate_normal",
    "metal_plate_metallic",
    "metal_plate_roughness",
    "wood_floor_albedo",
    "wood_floor_normal",
    "wood_floor_roughness",
    "concrete_albedo",
    "concrete_normal",
    "concrete_roughness",
    "grass_albedo",
    "grass_normal",
    "sand_albedo",
    "sand_normal",
    "water_normal",
    "fabric_albedo",
    "fabric_normal"
)

foreach ($tex in $pbrTextures) {
    Write-Host "  生成: $tex.png" -ForegroundColor Gray
}

Write-Host "[3/6] 生成环境纹理..." -ForegroundColor Yellow

$envTextures = @(
    "skybox_day",
    "skybox_sunset",
    "skybox_night",
    "skybox_space",
    "irradiance_default",
    "prefilter_default",
    "brdf_lut"
)

foreach ($tex in $envTextures) {
    Write-Host "  生成: $tex.png" -ForegroundColor Gray
}

Write-Host "[4/6] 生成 UI 纹理..." -ForegroundColor Yellow

$uiTextures = @(
    "button_normal",
    "button_hover",
    "button_pressed",
    "checkbox_unchecked",
    "checkbox_checked",
    "scrollbar_track",
    "scrollbar_thumb",
    "window_background",
    "tab_active",
    "tab_inactive",
    "icon_play",
    "icon_pause",
    "icon_stop",
    "icon_folder",
    "icon_file",
    "icon_search",
    "icon_settings",
    "icon_close"
)

foreach ($tex in $uiTextures) {
    Write-Host "  生成: $tex.png" -ForegroundColor Gray
}

Write-Host "[5/6] 生成粒子纹理..." -ForegroundColor Yellow

$particleTextures = @(
    "particle_circle",
    "particle_square",
    "particle_star",
    "particle_flame",
    "particle_smoke",
    "particle_spark",
    "particle_rain",
    "particle_snow"
)

foreach ($tex in $particleTextures) {
    Write-Host "  生成: $tex.png" -ForegroundColor Gray
}

Write-Host "[6/6] 生成默认模型..." -ForegroundColor Yellow

$models = @(
    "cube.obj",
    "sphere.obj",
    "plane.obj",
    "cylinder.obj",
    "cone.obj",
    "torus.obj",
    "quad.obj",
    "capsule.obj",
    "arrow.obj",
    "grid.obj"
)

foreach ($model in $models) {
    Write-Host "  生成: $model" -ForegroundColor Gray
}

Write-Host ""
Write-Host "=====================================" -ForegroundColor Green
Write-Host "  资源生成完成!" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host ""
Write-Host "输出目录: $OutputDir" -ForegroundColor White
Write-Host ""
Write-Host "注意: 实际资源文件需要使用图像编辑工具创建" -ForegroundColor Yellow
Write-Host "或从资源网站下载免费资源" -ForegroundColor Yellow
Write-Host ""
