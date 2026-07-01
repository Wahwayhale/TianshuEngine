# 编译器安装脚本
# 自动安装 MinGW-w64 (轻量级 C++ 编译器)

$ErrorActionPreference = "Stop"

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  安装 C++ 编译器" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# 检查是否已安装
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if ($gcc) {
    Write-Host "GCC 已安装: $($gcc.Source)" -ForegroundColor Green
    gcc --version | Select-Object -First 1
    exit 0
}

Write-Host "选择安装方式:" -ForegroundColor Yellow
Write-Host ""
Write-Host "1. MinGW-w64 (推荐，轻量级)" -ForegroundColor White
Write-Host "2. Visual Studio 2022 (完整 IDE)" -ForegroundColor White
Write-Host ""

$choice = Read-Host "请选择 (1 或 2)"

switch ($choice) {
    "1" {
        Write-Host "`n安装 MinGW-w64..." -ForegroundColor Yellow

        # 下载 MinGW-w64
        $mingwUrl = "https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-re1/x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-re1.7z"
        $mingwArchive = "$env:TEMP\mingw64.7z"
        $mingwDir = "E:\MinGW"

        Write-Host "下载 MinGW-w64..." -ForegroundColor Gray
        Invoke-WebRequest -Uri $mingwUrl -OutFile $mingwArchive

        # 解压
        Write-Host "解压到 $mingwDir..." -ForegroundColor Gray
        if (!(Test-Path $mingwDir)) {
            New-Item -ItemType Directory -Path $mingwDir -Force | Out-Null
        }

        # 使用 7z 或 tar 解压
        $7zPath = Get-Command 7z -ErrorAction SilentlyContinue
        if ($7zPath) {
            & 7z x $mingwArchive -o"$mingwDir" -y
        } else {
            Write-Host "需要 7-Zip 来解压。请手动解压到 $mingwDir" -ForegroundColor Yellow
            Write-Host "下载 7-Zip: https://7-zip.org/" -ForegroundColor Cyan
            exit 1
        }

        # 添加到 PATH
        $mingwBin = "$mingwDir\mingw64\bin"
        $env:PATH = "$mingwBin;$env:PATH"

        # 永久添加到 PATH
        $currentPath = [System.Environment]::GetEnvironmentVariable("PATH", "User")
        if ($currentPath -notlike "*$mingwBin*") {
            [System.Environment]::SetEnvironmentVariable("PATH", "$mingwBin;$currentPath", "User")
            Write-Host "已添加到 PATH" -ForegroundColor Green
        }

        # 验证
        Write-Host "`n验证安装..." -ForegroundColor Yellow
        & "$mingwBin\gcc.exe" --version | Select-Object -First 1
        & "$mingwBin\g++.exe" --version | Select-Object -First 1

        Write-Host "`nMinGW-w64 安装完成!" -ForegroundColor Green
    }

    "2" {
        Write-Host "`n请手动安装 Visual Studio 2022:" -ForegroundColor Yellow
        Write-Host "1. 下载: https://visualstudio.microsoft.com/downloads/" -ForegroundColor Cyan
        Write-Host "2. 选择 '使用 C++ 的桌面开发'" -ForegroundColor Cyan
        Write-Host "3. 安装完成后重新运行此脚本" -ForegroundColor Cyan

        # 打开下载页面
        Start-Process "https://visualstudio.microsoft.com/downloads/"
    }

    default {
        Write-Host "无效选择" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "安装完成后，请重新运行构建脚本" -ForegroundColor Yellow
