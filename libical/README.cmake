libical supports the CMake buildsystem.
You must have CMake version 2.4 or higher.

Please see the comments at the top of CMakeLists.txt for
the available configuration options you can pass to cmake.

The installation directory defaults to /usr/local on UNIX
and c:/Program Files on Windows. You can change this by
passing -DCMAKE_INSTALL_PREFIX=/install/path to cmake.

To build a debug version pass -DCMAKE_BUILD_TYPE=Debug to cmake.

Building on Linux/Unix with a native compiler:
% mkdir build
% cd build
% cmake ..
% make
% make install

Building on Windows with MicroSoft Visual Studio:
% mkdir build
% cd build
% cmake -G "NMake Makefiles" ..
% nmake
% nmake install


Building on Windows with mingw:
% mkdir build
% cd build
% cmake -G "MinGW Makefiles" ..
% mingw32-make
% mingw32-make install


Building on Windows under Cygwin:
% mkdir build
% cd build
% cmake ..
% make
% make install

To run the test suite, run 'make test'.
To run the test suite in verbose mode, run 'make test ARGS="-V"'.
