@echo off
title Select a build target
pushd %~dp0\..\

:select
echo Select a target:
echo   1. vs2019
echo   2. vs2022
echo   3. vscode
set /p target=
IF /i "%target%"=="1" goto vs2019
IF /i "%target%"=="2" goto vs2022
IF /i "%target%"=="3" goto vscode
echo %target% is not valid
goto select

:vs2019
call premake\premake5.exe vs2019
goto commonExit

:vs2022
call premake\premake5.exe vs2022
goto commonExit

:vscode
call premake\premake5.exe vscode
goto commonExit

:commonExit
popd
PAUSE