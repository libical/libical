@echo off

:: Now we declare a scope
Setlocal EnableDelayedExpansion EnableExtensions

if not defined COMPILER set COMPILER=msvc2015

if "%COMPILER%"=="MinGW" ( goto :mingw )

set VCARCH=x86

if "%PLATFORM%"=="X64" ( set VCARCH=x86_amd64 )

if "%COMPILER%"=="msvc2015" (
    set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
)

if "%COMPILER%"=="msvc2013" (
    set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
)

if "%COMPILER%"=="msvc2012" (
    set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"
)

if "%COMPILER%"=="msvc2010" (
    set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
)

if "%COMPILER%"=="msvc2008" (
    set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"
)

:: Visual Studio detected
endlocal & call %SET_VS_ENV% %VCARCH%
goto :eof

:: MinGW detected
:mingw
:: workaround for CMake not wanting sh.exe in PATH for MinGW
endlocal & set PATH=%PATH:C:\Program Files\Git\usr\bin;=% & set PATH=c:\MinGW\bin;%PATH%

:eof
if "%CMAKE_GENERATOR%"=="Ninja" (
    cinst -y ninja
    set PATH=C:\tools\ninja;%PATH%
)
if "%CMAKE_GENERATOR%"=="NMake Makefiles JOM" (
    cinst -y jom
)
