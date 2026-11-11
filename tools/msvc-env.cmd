@echo off
rem Intentionally no SETLOCAL: editors use this script to import the environment.
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo ERROR: Visual Studio Installer was not found.
    echo Install Visual Studio with the Desktop development with C++ workload.
    exit /b 1
)

set "VSINSTALL="
for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
if not defined VSINSTALL (
    echo ERROR: The Visual Studio C++ tools were not found.
    echo Add the Desktop development with C++ workload in Visual Studio Installer.
    exit /b 1
)

call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1

set "VSCMAKE=%VSINSTALL%\Common7\IDE\CommonExtensions\Microsoft\CMake"
set "CMAKE=%VSCMAKE%\CMake\bin\cmake.exe"
if not exist "%CMAKE%" (
    echo ERROR: Visual Studio's CMake tools were not found.
    echo Add C++ CMake tools for Windows in Visual Studio Installer.
    exit /b 1
)
set "PATH=%VSCMAKE%\CMake\bin;%VSCMAKE%\Ninja;%PATH%"
set "VSLANG=1033"
rem Keep localized /showIncludes messages consistent with CMake's UTF-8 files.
chcp 65001 >nul
exit /b 0
