@echo off
setlocal
call "%~dp0build.cmd"
if errorlevel 1 (
    echo Build failed. See the error above.
    pause
    exit /b 1
)
pushd "%~dp0build\bin\Release"
if errorlevel 1 exit /b 1
SoccerAnimation.exe
set "RESULT=%ERRORLEVEL%"
popd
exit /b %RESULT%
