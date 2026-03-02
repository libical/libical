# Building Libical

Libical uses the [CMake buildsystem](https://cmake.org)

Please see the comments at the top of CMakeLists.txt for
the available configuration options you can pass to cmake.

The installation directory defaults to /usr/local on UNIX
and c:/Program Files on Windows.  You can change this by
passing -DCMAKE_INSTALL_PREFIX=/install/path to cmake.

To build a debug version pass -DCMAKE_BUILD_TYPE=Debug to cmake.

To build libical you will need:

- a C99-compliant C compiler (let us know if the build fails with your C compiler)
- a C11-compliant C compiler for libical-glib
- a C++11 compliant compiler for C++ bindings
- CMake version 3.20.0 or higher
- Perl
- libicu (not required but strongly recommended)

Building on Unix with gcc or clang:

```shell
% mkdir build
% cd build
% cmake ..
% make
% make install
```

Building on Windows with MicroSoft Visual Studio:
From a command prompt for the version of MSVC you want to use

```shell
% mkdir build
% cd build
% cmake -G "NMake Makefiles" ..
% nmake
% nmake install
```

NOTE: Some MSVC 32bit compilers (like MSVC2005) use a 64bit version of time_t.
In these cases you must pass -DLIBICAL_ENABLE_MSVC_32BIT_TIME_T=true to cmake to make sure
the 32bit version of time_t is used instead.

Building on Windows with mingw:
Make sure you have the path to the MinGW programs in %PATH% first, for example:

```shell
% set "PATH=c:\MinGW\mingw64\bin;%PATH%"
% mkdir build
% cd build
% cmake -G "MinGW Makefiles" ..
% mingw32-make
% mingw32-make install
```

Building on Windows under Cygwin:

```shell
% mkdir build
% cd build
% cmake ..
% make
% make install
```

Building on MSYS with mingw:

```shell
% mkdir build
% cd build
% cmake -G "MSYS Makefiles" ..
% nmake
% nmake install
```

To run the test suite, from inside the build directory
run `make test` (or `nmake test` or `mingw32-make test`)

To run the test suite in verbose mode, pass ARGS="-V" to the make command
For example: `nmake test ARGS="-V"`

By default, the buildsystem creates shared(dynamic) and static versions
of the libraries, but that behavior can be modified at CMake time:

- To build the static libraries only, pass -DSTATIC_ONLY=True to cmake.
- To build the shared libraries only, pass -DSHARED_ONLY=True to cmake.

## Building with Different Compilers

For example, say you want to use Clang to build on Linux.
Then you can set the C and C++ compilers at CMake time, like so:

```shell
% CC=clang CXX=clang++ cmake ..
```

## Building for OSX (Mac)

### Homebrew

A couple of necessary dependencies do not have their headers
and development libraries linked into /usr/local/{include,lib} due
to possible contamination with the operating system versions of the
same name.  This is known to be the case with libffi and libxml2

Fix is to export PKG_CONFIG_PATH so CMake can find them, as in:

```shell
export PKG_CONFIG_PATH=/usr/local/opt/libffi/lib/pkgconfig:/usr/local/opt/libxml2/lib/pkgconfig
```

## Building for Android

```shell
% mkdir build
% cd build
% export ANDROID_NDK=/path/to/android-ndk
% cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
        -DCMAKE_INSTALL_PREFIX=/install/path .. \
        -DANDROID_ABI=[x86,x86_64,armeabi-v7a,arm64-v8a,...]
% make
% make install
```

For more information about building CMake projects on Android see
<https://developer.android.com/ndk/guides/cmake>

## Optional Dependencies

- libicu "International Components for Unicode" development libraries.
  Highly recommended for RSCALE support.
  libicu can be found at <http://www.icu-project.org>

  If CMake doesn't locate your libicu installation try setting the
  ICU_ROOT or ICU_BASE environment variables to the top folder where
  libicu is installed and run cmake again.

- Berkeley DB storage.
  Optional for the Berkeley Database storage support.
  The development toolkit can be found at <https://www.oracle.com/database/berkeley-db>

  **This code path has not been fully tested.**
  **Don't be surprised if there are bugs.**
  **Do NOT use for production code.**

- GObject Introspection.  To generate GObject introspection "typelib"
  files pass the -DLIBICAL_GOBJECT_INTROSPECTION=True option to cmake.  You'll
  need to have the GObject Introspection development package v0.6.7 or higher
  installed beforehand.

  Requires the GObject Introspection development package:

  - Ubuntu: apt-get install gobject-introspection
  - Fedora: dnf install gobject-introspection
  - Mac: brew install gobject-introspection pygobject3
  - Windows: vcpkg install gobject-introspection (see [using vcpkg](#Using vcpkg to install dependencies).

You can also force CMake to ignore any of the optional dependencies
by passing the option -DCMAKE_DISABLE_FIND_PACKAGE_<PACKAGE>=True.
For instance:

```shell
  % cmake -DCMAKE_DISABLE_FIND_PACKAGE_ICU=True # tell cmake to ignore ICU
```

## Language Bindings

This C library can be built with bindings for these other languages:

- C++. If a C++ compiler is found the buildsystem will create and install the C++ bindings API.
  Turn off the C++ bindings by passing -DLIBICAL_CXX_BINDINGS=False to cmake.
  Don't mix ABI from C and C++ compilers.

- Python bindings are built using gobject-introspection, which is enabled
  by passing -DLIBICAL_GOBJECT_INTROSPECTION=True to cmake.

  Requires the GObject Introspection development package (see above).

  Don't forget to set (or append to, as needed) the GI_TYPELIB_PATH environment
  variable to $PREFIX/lib/girepository-1.0 (or $PREFIX/lib64/girepository-1.0).

- Java. If Java (and associated JNI) along with a C++ compiler is found the
  buildsystem will create and install the Java bindings.

  Depending on your system, you may need to set the JAVA_HOME environment variable as well.

  Disable the Java bindings by passing -DLIBICAL_JAVA_BINDINGS=False to cmake.

## Tweaking the Library Behavior

Use these CMake options to adjust the library behavior as follows:

- LIBICAL_ENABLE_ERRORS_ARE_FATAL=[true|false]
  Set to make icalerror_* calls abort instead of internally signaling an error.
  Default=false

- LIBICAL_ENABLE_BUILTIN_TZDATA=[true|false]
  Set to build using our own (instead of the system's) timezone data.
  Default=false (use the system timezone data on non-Windows systems)
  ALWAYS true on Windows systems

## Tweaking the Installation Directories

By default, the installation layout is according to the
[GNU standard installation directories](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html).

You can override various installation folders by passing the following variables to cmake:

- CMAKE_INSTALL_LIBDIR = The fullpath where you want the put the installed libraries
- CMAKE_INSTALL_INCLUDEDIR = The fullpath where you want to put the installed include files
- CMAKE_INSTALL_DATAROOTDIR = The fullpath where to want to put the shared files

for example: `cmake -DCMAKE_INSTALL_PREFIX=/opt -DCMAKE_INSTALL_LIBDIR=/opt/lib32 ..`

## Build the documentation

Run `make docs` to build the API documentation and reference.

Disable the docs make target by running cmake with -DLIBICAL_BUILD_DOCS=False option.
Disable the libical-glib developer documentation by running cmake with -DLIBICAL_GLIB_BUILD_DOCS=False.

## Using vcpkg to install dependencies

If you want to use vcpkg (%VCPKG_ROOT% on Windows or $VCPKG_ROOT on non-Windows) to build
the dependencies as part of the project you can also tell CMake the vcpkg toolchain file
with the CMake "CMAKE_TOOLCHAIN_FILE" setting.

For example: if you have vcpkg installed in your project folder:

```shell
     export VCPKG_ROOT=$HOME/projects/vcpkg
     cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake <OPTIONS> ..
```

or on Windows:

```shell
     set VCPKG_ROOT=%HOMEPATH%\projects\vcpkg
     cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake <OPTIONS> ..
```

### Windows Notes

MSVC comes with vcpkg pre-installed and sets `%VCPKG_ROOT%` for you.
However, the only way to update that vcpkg version is by upgrading
MSVC itself; which may not be what you want and may be too old.
Consider installing your own vcpkg instead of relying on the MSVC vcpkg.

Either way (using Microsoft-provided or your own vcpkg):

```shell
     cmake "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake <OPTIONS> ..
```

### Non-Windows Notes

You'll need to pre-install autoconf, automake and autoconf-archive for icu.
See `%VCPKG_ROOT%/ports/icu/portfile.cmake` for more info.
