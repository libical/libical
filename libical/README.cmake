libical supports the CMake buildsystem.
You must have CMake version 2.4 or higher.

Please see the comments at the top of CMakeLists.txt for
the available configuration options you can pass to cmake.

The installation directory defaults to /usr/local on UNIX
and c:/Program Files on Windows. You can change this by
passing -DCMAKE_INSTALL_PREFIX=/path/to/libical to cmake.

To build

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
% cmake -G "NMake Makefiles" ..
% mingw32-make
% mingw32-make install


Building on Windows under Cygwin:
% mkdir build
% cd build
% cmake ..
% make
% make install
