# HOWTO: Setup for development on Windows

Notes on how to setup a developer environment on Windows for libical.
Work-in-progress

## Preliminary

Use winget to install the universal package updater (aka winget-ui)

```shell
     winget install --exact --id MartiCliment.UniGetUI --source winget
```

then pin unigetui to the taskbar and/or menu.

## Toolchain

Use winget to install toolchain:

- git
- cmake
- ninja
- MSVC community compiler:
  - winget install Microsoft.VisualStudio.BuildTools --source winget
  - winget install Microsoft.VisualStudio.Community --source winget
- Strawberry perl

   __WARNING__ Strawberry Perl adds c:\Strawberry\perl\c\bin in %PATH% which causes CMake
   to use the mingw found in there.  Best bet is to remove c:\Strawberry\perl\c\bin from %PATH%.

## Vcpkg

```shell
     mkdir c:\users\winterz\projects
     cd c:\users\winterz\projects
     git clone https://github.com/microsoft/vcpkg.git
```

## Building libical

```shell
     cd c:\users\winterz\projects
     git clone https://github.com/libical/libical.git
     cd libical
     mkdir build
     cd build
     set VCPKG_ROOT=C:\Users\winterz\projects\vcpkg
     cmake -G Ninja "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ..
     ninja
     ninja test
```
