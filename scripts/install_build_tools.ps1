# Spark Engine - Build Tools Installation Script
# Run as Administrator: Right-click -> Run with PowerShell (Admin)

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  Spark Engine - Build Tools Installer" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as Administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "[ERROR] Please run this script as Administrator!" -ForegroundColor Red
    Write-Host "Right-click the script -> Run with PowerShell (Admin)" -ForegroundColor Yellow
    pause
    exit 1
}

# Create temp directory
$tempDir = "$env:TEMP\spark_engine_install"
if (!(Test-Path $tempDir)) {
    New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
}

# =============================================
# Function: Download File
# =============================================
function Download-File {
    param(
        [string]$Url,
        [string]$Output
    )

    Write-Host "Downloading: $Url" -ForegroundColor Gray
    try {
        $webClient = New-Object System.Net.WebClient
        $webClient.DownloadFile($Url, $Output)
        Write-Host "Downloaded: $Output" -ForegroundColor Green
        return $true
    } catch {
        Write-Host "[ERROR] Failed to download: $_" -ForegroundColor Red
        return $false
    }
}

# =============================================
# Step 1: Install CMake
# =============================================
Write-Host ""
Write-Host "[1/3] Installing CMake..." -ForegroundColor Yellow

# Check if CMake is already installed
$cmakePath = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakePath) {
    Write-Host "CMake is already installed: $($cmakePath.Source)" -ForegroundColor Green
} else {
    $cmakeUrl = "https://github.com/Kitware/CMake/releases/download/v3.28.1/cmake-3.28.1-windows-x86_64.msi"
    $cmakeMsi = "$tempDir\cmake-3.28.1-windows-x86_64.msi"

    if (Download-File -Url $cmakeUrl -Output $cmakeMsi) {
        Write-Host "Installing CMake (this may take a few minutes)..." -ForegroundColor Yellow
        Start-Process msiexec.exe -ArgumentList "/i `"$cmakeMsi`" ADD_CMAKE_TO_PATH=System /quiet" -Wait -NoNewWindow
        Write-Host "CMake installed successfully!" -ForegroundColor Green

        # Refresh PATH
        $env:PATH = [System.Environment]::GetEnvironmentVariable("PATH", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("PATH", "User")
    } else {
        Write-Host "[ERROR] Failed to download CMake" -ForegroundColor Red
    }
}

# =============================================
# Step 2: Install Visual Studio 2022 Build Tools
# =============================================
Write-Host ""
Write-Host "[2/3] Installing Visual Studio 2022 Build Tools..." -ForegroundColor Yellow

# Check if VS 2022 is already installed
$vsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -property installationPath
    if ($vsPath) {
        Write-Host "Visual Studio is already installed: $vsPath" -ForegroundColor Green
    }
}

# Download VS Build Tools installer
$vsUrl = "https://aka.ms/vs/17/release/vs_BuildTools.exe"
$vsInstaller = "$tempDir\vs_BuildTools.exe"

if (!(Test-Path "C:\Program Files\Microsoft Visual Studio\2022")) {
    if (Download-File -Url $vsUrl -Output $vsInstaller) {
        Write-Host "Installing VS 2022 Build Tools (this will take 10-30 minutes)..." -ForegroundColor Yellow
        Write-Host "The installer will run in the background..." -ForegroundColor Gray

        # Install with C++ workload
        $arguments = "--quiet --wait --norestart --nocache --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
        Start-Process -FilePath $vsInstaller -ArgumentList $arguments -Wait -NoNewWindow

        Write-Host "Visual Studio 2022 Build Tools installed!" -ForegroundColor Green
    } else {
        Write-Host "[ERROR] Failed to download VS Build Tools" -ForegroundColor Red
    }
} else {
    Write-Host "Visual Studio 2022 directory found" -ForegroundColor Green
}

# =============================================
# Step 3: Install Vulkan SDK
# =============================================
Write-Host ""
Write-Host "[3/3] Checking Vulkan SDK..." -ForegroundColor Yellow

$vulkanSdk = $env:VULKAN_SDK
if ($vulkanSdk -and (Test-Path $vulkanSdk)) {
    Write-Host "Vulkan SDK is installed: $vulkanSdk" -ForegroundColor Green
} else {
    Write-Host "Vulkan SDK not found!" -ForegroundColor Yellow
    Write-Host "Please download and install Vulkan SDK from:" -ForegroundColor Cyan
    Write-Host "https://vulkan.lunarg.com/sdk/home" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "After installation, restart this script." -ForegroundColor Yellow
}

# =============================================
# Summary
# =============================================
Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  Installation Summary" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# Check CMake
$cmakeCheck = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakeCheck) {
    Write-Host "[OK] CMake: $(cmake --version 2>&1 | Select-Object -First 1)" -ForegroundColor Green
} else {
    Write-Host "[MISSING] CMake - Please restart your terminal" -ForegroundColor Yellow
}

# Check Visual Studio
if (Test-Path "C:\Program Files\Microsoft Visual Studio\2022") {
    Write-Host "[OK] Visual Studio 2022" -ForegroundColor Green
} else {
    Write-Host "[MISSING] Visual Studio 2022" -ForegroundColor Red
}

# Check Vulkan SDK
if ($env:VULKAN_SDK) {
    Write-Host "[OK] Vulkan SDK: $env:VULKAN_SDK" -ForegroundColor Green
} else {
    Write-Host "[MISSING] Vulkan SDK" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  Next Steps" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "1. Close and reopen your terminal" -ForegroundColor White
Write-Host "2. Run: cd E:\项目\系统" -ForegroundColor White
Write-Host "3. Run: cmake --preset windows-release" -ForegroundColor White
Write-Host "4. Run: cmake --build --preset windows-release" -ForegroundColor White
Write-Host ""

# Cleanup
Write-Host "Cleaning up temp files..." -ForegroundColor Gray
Remove-Item -Path $tempDir -Recurse -Force -ErrorAction SilentlyContinue

Write-Host "Installation complete!" -ForegroundColor Green
pause
