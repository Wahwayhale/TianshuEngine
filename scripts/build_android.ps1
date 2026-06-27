# Android 构建脚本
param(
    [string]$Arch = "arm64-v8a",
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  天枢引擎 - Android 构建" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# 检查 Android NDK
$ndkPath = $env:ANDROID_NDK_HOME
if (-not $ndkPath) {
    $ndkPath = $env:NDK_ROOT
}

if (-not $ndkPath) {
    # 尝试常见路径
    $possiblePaths = @(
        "$env:LOCALAPPDATA\Android\Sdk\ndk\25.1.8937393",
        "$env:ANDROID_HOME\ndk\25.1.8937393",
        "C:\Android\ndk\25.1.8937393"
    )

    foreach ($p in $possiblePaths) {
        if (Test-Path $p) {
            $ndkPath = $p
            break
        }
    }
}

if (-not $ndkPath -or !(Test-Path $ndkPath)) {
    Write-Host "[ERROR] Android NDK 未找到!" -ForegroundColor Red
    Write-Host "请安装 Android NDK 并设置 ANDROID_NDK_HOME 环境变量" -ForegroundColor Yellow
    Write-Host "下载地址: https://developer.android.com/ndk/downloads" -ForegroundColor Yellow
    exit 1
}

Write-Host "NDK 路径: $ndkPath" -ForegroundColor Green

# 设置变量
$buildDir = "build/android-$Arch"
$toolchainFile = "$ndkPath/build/cmake/android.toolchain.cmake"

# 清理构建目录
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

# 配置
Write-Host "`n[1/3] 配置 CMake..." -ForegroundColor Yellow
cmake -S . -B $buildDir `
    -DCMAKE_TOOLCHAIN_FILE=$toolchainFile `
    -DANDROID_ABI=$Arch `
    -DANDROID_PLATFORM=android-24 `
    -DANDROID_STL=c++_shared `
    -DCMAKE_BUILD_TYPE=$BuildType `
    -DBUILD_EDITOR=OFF `
    -DPLATFORM_ANDROID=ON

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] CMake 配置失败!" -ForegroundColor Red
    exit 1
}

# 构建
Write-Host "`n[2/3] 构建引擎..." -ForegroundColor Yellow
cmake --build $buildDir --config $BuildType

if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] 构建失败!" -ForegroundColor Red
    exit 1
}

# 复制输出
Write-Host "`n[3/3] 复制输出..." -ForegroundColor Yellow
$outputDir = "release/android/$Arch"
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null

# 复制 SO 文件
$soFile = "$buildDir/libTianshuEngine.so"
if (Test-Path $soFile) {
    Copy-Item -Path $soFile -Destination $outputDir/
    Write-Host "已复制: libTianshuEngine.so" -ForegroundColor Green
}

Write-Host ""
Write-Host "=====================================" -ForegroundColor Green
Write-Host "  Android 构建完成!" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host ""
Write-Host "架构: $Arch" -ForegroundColor White
Write-Host "输出: $outputDir" -ForegroundColor White
Write-Host ""
