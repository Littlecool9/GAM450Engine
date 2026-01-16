@echo off
setlocal

echo Updating git submodules...

REM Change directory to where this .bat file lives
cd /d "%~dp0"

REM Safety check: are we in a git repo?
if not exist ".git" (
    echo ERROR: This is not a git repository.
    pause
    exit /b 1
)

git submodule update --init --recursive
echo.
echo Submodules updated successfully.


REM =============================
REM Config
REM =============================
set VCPKG_ROOT=%~dp0Mitosis-Engine\third-party\tools\vcpkg
set SCRIPT_DIR=%~dp0
set VCPKG_TRIPLET=x64-windows
set VCPKG_INSTALL_ROOT=%SCRIPT_DIR%Mitosis-Engine\third-party\vcpkg-installed

REM =============================
REM Clone vcpkg if missing
REM =============================
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [vcpkg] Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg "%VCPKG_ROOT%"
)

REM =============================
REM Bootstrap vcpkg
REM =============================
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo [vcpkg] Bootstrapping...
    pushd "%VCPKG_ROOT%"
    call bootstrap-vcpkg.bat
    popd
)

REM =============================
REM Install packages
REM =============================
echo [vcpkg] Installing dependencies...
"%VCPKG_ROOT%\vcpkg.exe" install ^
    --triplet %VCPKG_TRIPLET% ^
    --x-packages-root="%VCPKG_INSTALL_ROOT%"

echo [vcpkg] Done.

pause