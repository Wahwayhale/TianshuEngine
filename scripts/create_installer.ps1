# 天枢引擎安装程序创建脚本
# 创建 NSIS 安装程序

param(
    [string]$Version = "0.32.0"
)

$ErrorActionPreference = "Stop"

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  创建天枢引擎安装程序" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# 检查 NSIS
$nsisPath = Get-Command makensis -ErrorAction SilentlyContinue
if (-not $nsisPath) {
    Write-Host "[ERROR] NSIS 未安装!" -ForegroundColor Red
    Write-Host "请安装 NSIS: https://nsis.sourceforge.io/Download" -ForegroundColor Yellow
    exit 1
}

# 检查打包目录
$packageDir = "release/TianshuEngine-$Version-windows"
if (!(Test-Path $packageDir)) {
    Write-Host "[ERROR] 打包目录不存在: $packageDir" -ForegroundColor Red
    Write-Host "请先运行 package_engine.bat" -ForegroundColor Yellow
    exit 1
}

# 创建 NSIS 脚本
$nsisScript = @"
!include "MUI2.nsh"

; 安装程序属性
Name "天枢引擎 $Version"
OutFile "release/TianshuEngine-$Version-Setup.exe"
InstallDir "C:\TianshuEngine"
InstallDirRegKey HKLM "Software\TianshuEngine" "InstallDir"
RequestExecutionLevel admin

; 界面
!define MUI_ABORTWARNING
!define MUI_ICON "assets\icon.ico"

; 页面
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; 语言
!insertmacro MUI_LANGUAGE "SimpChinese"

; 安装部分
Section "天枢引擎" SecMain
    SetOutPath `$INSTDIR

    ; 复制文件
    File /r "$packageDir\*.*"

    ; 写入注册表
    WriteRegStr HKLM "Software\TianshuEngine" "InstallDir" "`$INSTDIR"
    WriteRegStr HKLM "Software\TianshuEngine" "Version" "$Version"

    ; 创建卸载程序
    WriteUninstaller "`$INSTDIR\Uninstall.exe"

    ; 创建开始菜单
    CreateDirectory "`$SMPROGRAMS\天枢引擎"
    CreateShortCut "`$SMPROGRAMS\天枢引擎\天枢引擎.lnk" "`$INSTDIR\bin\SparkEditor.exe"
    CreateShortCut "`$SMPROGRAMS\天枢引擎\卸载.lnk" "`$INSTDIR\Uninstall.exe"
    CreateShortCut "`$SMPROGRAMS\天枢引擎\文档.lnk" "`$INSTDIR\QUICKSTART.md"

    ; 设置环境变量
    ; System::Call 'Kernel32::SetEnvironmentVariable(t "TIANSHU_ENGINE", t "$INSTDIR", i 0)'

    MessageBox MB_OK "天枢引擎 $Version 安装完成！"
SectionEnd

; 卸载部分
Section "Uninstall"
    ; 删除文件
    RMDir /r "`$INSTDIR"

    ; 删除注册表
    DeleteRegKey HKLM "Software\TianshuEngine"

    ; 删除开始菜单
    RMDir /r "`$SMPROGRAMS\天枢引擎"
SectionEnd
"@

$nsisFile = "installer.nsi"
Set-Content -Path $nsisFile -Value $nsisScript

Write-Host "NSIS 脚本已创建: $nsisFile" -ForegroundColor Green
Write-Host ""
Write-Host "要创建安装程序，请运行:" -ForegroundColor Yellow
Write-Host "  makensis $nsisFile" -ForegroundColor White
Write-Host ""
