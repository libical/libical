@echo off

rem SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
rem SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

:: Now we declare a scope
Setlocal EnableDelayedExpansion EnableExtensions

if not defined APPVEYOR_BUILD_WORKER_IMAGE set APPVEYOR_BUILD_WORKER_IMAGE="Visual Studio 2019"

if "%USE_MINGW%"=="MinGW" ( goto :mingw )

set arch=x86

if "%platform%" EQU "x64" ( set arch=x86_amd64 )

if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2019" (
        set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
)

if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2015" (
        set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
)

if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2013" (
        set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
)

if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2012" (
        set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"
)

if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2010" (
        set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
)

if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2008" (
        set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
)

:: Visual Studio detected
endlocal & call %SET_VS_ENV% %arch%
goto :eof

:: MinGW detected
:mingw
:: workaround for CMake not wanting sh.exe in PATH for MinGW
endlocal & set PATH=%PATH:C:\Program Files\Git\usr\bin;=% & set PATH=c:\MinGW\bin;%PATH%
