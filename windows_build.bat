@echo off
setlocal

echo ========================================
echo RDXGraphics Windows Build
echo ========================================

REM ------------------------------------------------------------
REM 1. Find / initialize Visual Studio 2022
REM ------------------------------------------------------------

if defined VSCMD_VER (
    echo [OK] Visual Studio Developer environment already active.
    goto :vs_ready
)

echo [INFO] Visual Studio Developer environment not detected.
echo [INFO] Searching for Visual Studio 2022...

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found.
    echo.
    echo Please install Visual Studio 2022 with:
    echo   Desktop development with C++
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "VSINSTALL=%%i"
)

if not defined VSINSTALL (
    echo [ERROR] Could not find a Visual Studio installation with C++ tools.
    exit /b 1
)

echo [OK] Found Visual Studio:
echo      %VSINSTALL%

call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=x64

if errorlevel 1 (
    echo [ERROR] Failed to initialize Visual Studio environment.
    exit /b 1
)

:vs_ready

REM ------------------------------------------------------------
REM 2. Force our own vcpkg
REM ------------------------------------------------------------

set "VCPKG_ROOT=D:\Repos\vcpkg"

echo [INFO] VCPKG_ROOT=%VCPKG_ROOT%

if not exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
    echo [ERROR] vcpkg toolchain not found:
    echo         %VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
    exit /b 1
)

echo [OK] vcpkg toolchain found.


REM ------------------------------------------------------------
REM 3. Force MSVC instead of Clang
REM ------------------------------------------------------------

where cl.exe >nul 2>&1

if errorlevel 1 (
    echo [ERROR] cl.exe was not found.
    echo [ERROR] Visual Studio C++ environment was not initialized correctly.
    exit /b 1
)

echo [OK] MSVC compiler:
where cl.exe

set "CC=cl"
set "CXX=cl"

echo [INFO] CC=%CC%
echo [INFO] CXX=%CXX%


REM ------------------------------------------------------------
REM 4. Check Ninja
REM ------------------------------------------------------------

where ninja.exe >nul 2>&1

if errorlevel 1 (
    echo [ERROR] Ninja was not found on PATH.
    echo.
    echo Install Ninja, for example:
    echo   winget install Ninja-build.Ninja
    exit /b 1
)

echo [OK] Ninja:
ninja --version


REM ------------------------------------------------------------
REM 5. Configure
REM ------------------------------------------------------------

echo.
echo ========================================
echo Configuring
echo ========================================

cmake --preset=default -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed.
    exit /b 1
)


REM ------------------------------------------------------------
REM 6. Build
REM ------------------------------------------------------------

echo.
echo ========================================
echo Building
echo ========================================

cmake --build build --config Debug -j

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed.
    exit /b 1
)

echo.
echo ========================================
echo Build successful!
echo ========================================

endlocal