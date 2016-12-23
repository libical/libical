#!/bin/sh

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
#set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

#ensure parallel builds
export MAKEFLAGS=-j8

##### START FUNCTIONS #####

#function SET_GCC
# setup compiling with gcc
SET_GCC() {
  export CC=gcc; export CXX=g++
}

#function SET_CLANG
# setup compiling with clang
SET_CLANG() {
  export CC=clang; export CXX=clang++
}

#function COMPILE_WARNINGS:
# print warnings found in the compile-stage output
# $1 = file with the compile-stage output
COMPILE_WARNINGS() {
  w=`cat $1 | grep warning: | grep -v "failed to load external entity" | grep -v "are missing in source code comment block" | grep -v "g-ir-scanner:" | grep -v "argument unused during compilation:" | wc -l | awk '{print $1}'`
  if ( test $w -gt 0 )
  then
    echo "EXITING. $w warnings encountered"
    echo
    cat $1 | grep warning: | grep -v "failed to load external entity" | grep -v "are missing in source code comment block" | grep -v "g-ir-scanner:" | grep -v "argument unused during compilation:"
    exit 1
  fi
}

#function CPPCHECK_WARNINGS:
# print warnings found in the cppcheck output
# $1 = file with the cppcheck output
CPPCHECK_WARNINGS() {
  w=`cat $1 | grep '\(warning\)' | wc -l | awk '{print $1}'`
  if ( test $w -gt 0 )
  then
    echo "EXITING. $w warnings encountered"
    echo
    cat $1 | grep '\(warning\)'
    exit 1
  fi
}

#function BDIR:
# set the name of the build directory for the current test
# $1 = the name of the current test
BDIR() {
  BDIR=$TOP/build-$1
}

#function CONFIGURE:
# creates the builddir and runs CMake with the specified options
# $1 = the name of the test
# $2 = CMake options
CONFIGURE() {
  BDIR $1
  mkdir -p $BDIR
  cd $BDIR
  rm -rf *
  cmake .. $2 || exit 1
}

#function BUILD:
# runs a build test, where build means: configure, compile, link and run the unit tests
# $1 = the name of the test
# $2 = CMake options
BUILD() {
  cd $TOP
  CONFIGURE "$1" "$2"
  make |& tee make.out || exit 1
  COMPILE_WARNINGS make.out
  make test |& tee make-test.out || exit 1
  cd ..
  rm -rf $BDIR
}

#function GCC_BUILD:
# runs a build test using gcc
# $1 = the name of the test (which will have "-gcc" appended to it)
# $2 = CMake options
GCC_BUILD() {
  name="$1-gcc"
  echo "===== START GCC BUILD: $1 ======"
  SET_GCC
  BUILD "$name" "$2"
  echo "===== END GCC BUILD: $1 ======"
}

#function CLANG_BUILD:
# runs a build test using clang
# $1 = the name of the test (which will have "-clang" appended to it)
# $2 = CMake options
CLANG_BUILD() {
  name="$1-clang"
  echo "===== START CLANG BUILD: $1 ======"
  SET_CLANG
  BUILD "$name" "$2"
  echo "===== END CLANG BUILD: $1 ======"
}

#function CPPCHECK
# runs a cppcheck test, which means: configure, compile, link and run cppcheck
# $1 = the name of the test (which will have "-cppcheck" appended to it)
# $2 = CMake options
CPPCHECK() {
  name="$1-cppcheck"
  echo "===== START SETUP FOR CPPCHECK: $1 ======"

  #first build it
  cd $TOP
  SET_GCC
  CONFIGURE "$name" "$2"
  make |& tee make.out || exit 1

  echo "===== START CPPCHECK: $1 ======"
  cd $TOP
  cppcheck --quiet --std=posix --language=c \
           --force --error-exitcode=1 --inline-suppr \
           --enable=warning,performance,portability,information \
           -D sleep="" \
           -D localtime_r="" \
           -D gmtime_r="" \
           -D size_t="unsigned long" \
           -D bswap32="" \
           -D PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP="" \
           -I $BDIR \
           -I $BDIR/src/libical \
           -I $BDIR/src/libicalss \
           -I $TOP/src/libical \
           -I $TOP/src/libicalss \
           -I $TOP/src/libicalvcal \
           $TOP/src $BDIR/src/libical/icalderived* 2>&1 | \
      grep -v 'Found a statement that begins with numeric constant' | \
      grep -v 'cannot find all the include files' | \
      grep -v Net-ICal | \
      grep -v icalssyacc\.c  | \
      grep -v icalsslexer\.c | tee cppcheck.out
  CPPCHECK_WARNINGS cppcheck.out
  rm -f cppcheck.out
  rm -rf $BDIR
  echo "===== END CPPCHECK: $1 ======"
}

#function SPLINT
# runs a splint test, which means: configure, compile, link and run splint
# $1 = the name of the test (which will have "-splint" appended to it
# $2 = CMake options
SPLINT() {
  name="$1-splint"
  echo "===== START SETUP FOR SPLINT: $1 ======"

  #first build it
  cd $TOP
  SET_GCC
  CONFIGURE "$name" "$2"
  make |& tee make.out || exit 1

  echo "===== START SPLINT: $1 ======"
  cd $TOP
  files=`find src -name "*.c" -o -name "*.h" | \
  # skip C++
  grep -v _cxx | grep -v /Net-ICal-Libical |
  # skip lex/yacc
  grep -v /icalssyacc | grep -v /icalsslexer | \
  # skip test programs
  grep -v /test/ | grep -v /vcaltest\.c | grep -v /vctest\.c | \
  # skip builddirs
  grep -v build-`
  files="$files $BDIR/src/libical/*.c $BDIR/src/libical/*.h"

  splint $files \
       -weak -warnposix \
       -modobserver -initallelements -redef \
       -linelen 1000 \
       -DWITH_BDB=1 \
       -DHAVE_CONFIG_H=1 \
       -DPACKAGE_DATA_DIR="\"foo\"" \
       -DTEST_DATADIR="\"bar\"" \
       -D"gmtime_r"="" \
       -D"localtime_r"="" \
       -D"nanosleep"="" \
       -D"popen"="fopen" \
       -D"pclose"="" \
       -D"setenv"="" \
       -D"strdup"="" \
       -D"strcasecmp"="strcmp" \
       -D"strncasecmp"="strncmp" \
       -D"putenv"="" \
       -D"unsetenv"="" \
       -D"tzset()"=";" \
       -DLIBICAL_ICAL_EXPORT=extern \
       -DLIBICAL_ICALSS_EXPORT=extern \
       -DLIBICAL_VCAL_EXPORT=extern \
       -DENOENT=1 -DENOMEM=1 -DEINVAL=1 -DSIGALRM=1 \
       `pkg-config glib-2.0 --cflags` \
       `pkg-config libxml-2.0 --cflags` \
       -I $BDIR \
       -I $BDIR/src \
       -I $BDIR/src/libical \
       -I $BDIR/src/libicalss \
       -I $TOP \
       -I $TOP/src \
       -I $TOP/src/libical \
       -I $TOP/src/libicalss \
       -I $TOP/src/libicalvcal \
       -I $TOP/src/libical-glib | \
  cat -
  status=${PIPESTATUS[0]}
  rm -rf $BDIR
  if ( test $status -gt 0 )
  then
    echo "Splint warnings encountered.  Exiting..."
    exit 1
  fi
  echo "===== END SPLINT: $1 ======"
}

#function CLANGTIDY
# runs a clang-tidy test, which means: configure, compile, link and run clang-tidy
# $1 = the name of the test (which will have "-tidy" appended)
# $2 = CMake options
CLANGTIDY() {
  echo "===== START CLANG-TIDY: $1 ====="
  tidyopts="*,-modernize*,-google-build-using-namespace,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-type-const-cast,-cppcoreguidelines-pro-type-reinterpret-cast,-llvm-include-order,-cppcoreguidelines-pro-type-vararg"
  cd $TOP
  CONFIGURE "$1-tidy" "$2 -DCMAKE_CXX_CLANG_TIDY=\"clang-tidy;-checks=$tidyopts\""
  cmake --build . |& tee make-tidy.out
  COMPILE_WARNINGS make-tidy.out
  cd ..
  rm -rf $BDIR
  echo "===== END CLANG-TIDY: $1 ====="
}

#function KRAZY
# runs a krazy2 test
KRAZY() {
  echo "===== START KRAZY ====="
  cd $TOP
  krazy2all |& tee krazy.out
  status=$?
  if ( test $status -gt 0 )
  then
    echo "Krazy warnings encountered.  Exiting..."
    exit 1
  fi
  rm -f krazy.out
  echo "===== END KRAZY ======"
}

##### END FUNCTIONS #####

#MAIN
TOP=`dirname $0`
cd $TOP
cd ..
TOP=`pwd`
BDIR=""

CMAKEOPTS="-DCMAKE_BUILD_TYPE=Debug -DUSE_INTEROPERABLE_VTIMEZONES=True -DWITH_BDB=True -DGOBJECT_INTROSPECTION=True -DICAL_GLIB=True"

KRAZY

#GCC based tests
GCC_BUILD test1 ""
GCC_BUILD test2 "$CMAKEOPTS"
GCC_BUILD test1cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake"
GCC_BUILD test2cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake $CMAKEOPTS"

SPLINT test2 "$CMAKEOPTS"
CPPCHECK test2 "$CMAKEOPTS"

#Clang based tests
CLANG_BUILD test1 ""
CLANG_BUILD test2 "$CMAKEOPTS"
CLANG_BUILD test1cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake"
CLANG_BUILD test2cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake $CMAKEOPTS"

CLANGTIDY test2 "$CMAKEOPTS"

CLANG_BUILD asan1 "-DADDRESS_SANITIZER=True -DUSE_INTEROPERABLE_VTIMEZONES=True -DWITH_BDB=True"

echo "ALL TESTS COMPLETED SUCCESSFULLY"
