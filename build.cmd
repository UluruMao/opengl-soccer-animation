@echo off
setlocal
for /f "tokens=2 delims=:" %%C in ('chcp') do set "BUILD_CODEPAGE=%%C"
call :build
set "BUILD_RESULT=%ERRORLEVEL%"
if defined BUILD_CODEPAGE chcp %BUILD_CODEPAGE% >nul
exit /b %BUILD_RESULT%

:build
call "%~dp0tools\msvc-env.cmd"
if errorlevel 1 exit /b 1
pushd "%~dp0"
if errorlevel 1 exit /b 1
"%CMAKE%" -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto failed
"%CMAKE%" --build build --target SoccerAnimation
if errorlevel 1 goto failed
echo Built: %CD%\build\bin\Release\SoccerAnimation.exe
popd
exit /b 0

:failed
popd
exit /b 1
