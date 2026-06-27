# WebGL 构建脚本
param(
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  天枢引擎 - WebGL 构建" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# 检查 Emscripten
$emccPath = Get-Command emcc -ErrorAction SilentlyContinue
if (-not $emccPath) {
    # 尝试常见路径
    $possiblePaths = @(
        "$env:EMSDK",
        "$env:HOME/emsdk",
        "C:\emsdk"
    )

    foreach ($p in $possiblePaths) {
        if (Test-Path "$p/upstream/emscripten/emcc") {
            $env:PATH = "$p/upstream/emscripten;$env:PATH"
            $emccPath = Get-Command emcc -ErrorAction SilentlyContinue
            break
        }
    }
}

if (-not $emccPath) {
    Write-Host "[ERROR] Emscripten 未找到!" -ForegroundColor Red
    Write-Host "请安装 Emscripten:" -ForegroundColor Yellow
    Write-Host "  git clone https://github.com/emscripten-core/emsdk.git" -ForegroundColor White
    Write-Host "  cd emsdk" -ForegroundColor White
    Write-Host "  ./emsdk install latest" -ForegroundColor White
    Write-Host "  ./emsdk activate latest" -ForegroundColor White
    Write-Host "  source ./emsdk_env.sh" -ForegroundColor White
    exit 1
}

Write-Host "Emscripten 已找到" -ForegroundColor Green

# 设置变量
$buildDir = "build/web"

# 清理构建目录
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null

# 配置
Write-Host "`n[1/3] 配置 CMake..." -ForegroundColor Yellow
emcmake cmake -S . -B $buildDir `
    -DCMAKE_BUILD_TYPE=$BuildType `
    -DBUILD_EDITOR=OFF `
    -DPLATFORM_WEB=ON

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
$outputDir = "release/web"
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null

# 复制生成的文件
$files = @(
    "$buildDir/TianshuEngine.js",
    "$buildDir/TianshuEngine.wasm",
    "$buildDir/TianshuEngine.data"
)

foreach ($file in $files) {
    if (Test-Path $file) {
        Copy-Item -Path $file -Destination $outputDir/
        Write-Host "已复制: $(Split-Path $file -Leaf)" -ForegroundColor Green
    }
}

# 创建 HTML 文件
$htmlContent = @"
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>天枢引擎</title>
    <style>
        body {
            margin: 0;
            padding: 0;
            background: #1a1a1e;
            overflow: hidden;
        }
        canvas {
            display: block;
            width: 100vw;
            height: 100vh;
        }
        #loading {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            color: #fff;
            font-family: sans-serif;
        }
    </style>
</head>
<body>
    <div id="loading">Loading...</div>
    <canvas id="canvas" oncontextmenu="event.preventDefault()" tabindex=-1></canvas>
    <script>
        var Module = {
            onRuntimeInitialized: function() {
                document.getElementById('loading').style.display = 'none';
            },
            canvas: (function() {
                var canvas = document.getElementById('canvas');
                canvas.addEventListener("webglcontextlost", function(e) {
                    alert('WebGL context lost. You will need to reload the page.');
                    e.preventDefault();
                }, false);
                return canvas;
            })()
        };
    </script>
    <script src="TianshuEngine.js"></script>
</body>
</html>
"@

Set-Content -Path "$outputDir/index.html" -Value $htmlContent

# 计算大小
$totalSize = 0
Get-ChildItem -Path $outputDir | ForEach-Object {
    $totalSize += $_.Length
}
$totalSizeMB = $totalSize / 1MB

Write-Host ""
Write-Host "=====================================" -ForegroundColor Green
Write-Host "  WebGL 构建完成!" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host ""
Write-Host "输出目录: $outputDir" -ForegroundColor White
Write-Host "总大小: $([math]::Round($totalSizeMB, 2)) MB" -ForegroundColor White
Write-Host ""
Write-Host "测试方法:" -ForegroundColor Yellow
Write-Host "  cd $outputDir" -ForegroundColor White
Write-Host "  python -m http.server 8080" -ForegroundColor White
Write-Host "  打开浏览器访问 http://localhost:8080" -ForegroundColor White
Write-Host ""
