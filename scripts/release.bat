@echo off
echo =====================================
echo   Spark Engine - Release Script
echo =====================================
echo.

cd /d "%~dp0.."

:: Check if git is available
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Git not found! Please install Git first.
    pause
    exit /b 1
)

:: Get current version from CHANGELOG
for /f "tokens=*" %%a in ('findstr /r "^\#\# \[0\." CHANGELOG.md') do (
    set VERSION_LINE=%%a
    goto :found_version
)
:found_version
echo Current version: %VERSION_LINE%
echo.

:: Ask for new version
set /p NEW_VERSION=Enter new version (e.g., 0.29.0):

if "%NEW_VERSION%"=="" (
    echo [ERROR] Version cannot be empty!
    pause
    exit /b 1
)

echo.
echo Creating release for version %NEW_VERSION%...
echo.

:: Create git tag
echo [1/3] Creating git tag...
git tag -a v%NEW_VERSION% -m "Release v%NEW_VERSION%"
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create tag!
    pause
    exit /b 1
)

:: Push tag
echo [2/3] Pushing tag to remote...
git push origin v%NEW_VERSION%
if %errorlevel% neq 0 (
    echo [ERROR] Failed to push tag!
    pause
    exit /b 1
)

:: Trigger GitHub Actions
echo [3/3] Tag pushed! GitHub Actions will now build and release.
echo.
echo =====================================
echo   Release Process Started!
echo =====================================
echo.
echo The following will happen automatically:
echo   1. GitHub Actions will build for Windows, Linux, macOS
echo   2. Release packages will be created
echo   3. GitHub Release will be published
echo.
echo Check progress at:
echo   https://github.com/%GITHUB_REPOSITORY%/actions
echo.
echo Download links will be available at:
echo   https://github.com/%GITHUB_REPOSITORY%/releases/tag/v%NEW_VERSION%
echo.
pause
