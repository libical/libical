@echo off

REM SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
REM SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

set BDIR=build-windows-msvc

REM Assumes running in an MSVC Developer Command Prompt
REM To use MinGW uncomment the following lines:
REM set "PATH=%PATH%\c:\mingw\mingw64\bin"
REM set BDIR=build-windows-mingw

REM Modify VCPKG_ROOT to your vpckg clone (if you have one; else MSVS version is used)
set VCPKG_ROOT=c:\users\winterz\projects\vcpkg
set CMAKE_VERSION4_OPTIONS="-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DLIBICAL_DEVMODE=ON -DLIBICAL_DEVMODE_MEMORY_CONSISTENCY=ON -DLIBICAL_DEVMODE_SYNCMODE_THREADLOCAL=ON -DLIBICAL_BUILD_VZIC=OFF
echo %CMAKE_VERSION4_OPTIONS%

rmdir %BDIR% /s /q
mkdir %BDIR%
cd %BDIR%

cmake -S  .. -G Ninja -DCMAKE_BUILD_TYPE=Debug -DLIBICAL_STATIC=OFF -DLIBICAL_ENABLE_BUILTIN_TZDATA=ON -DLIBICAL_BUILD_TESTING=ON -DLIBICAL_BUILD_DOCS=ON -DLIBICAL_BUILD_EXAMPLES=ON -DLIBICAL_CXX_BINDINGS=ON -DLIBICAL_JAVA_BINDINGS=OFF -DLIBICAL_GLIB=OFF -DLIBICAL_GOBJECT_INTROSPECTION=OFF -DLIBICAL_GLIB_VAPI=OFF -DLIBICAL_GLIB_BUILD_DOCS=OFF %CMAKE_VERSION4_OPTIONS% -DLIBICAL_BUILD_TESTING_BIGFUZZ=ON
set VCPKG_ROOT=
ninja --verbose
ninja test

cd ..
rmdir %BDIR% /s /q
