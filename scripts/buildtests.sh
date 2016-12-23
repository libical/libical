#!/bin/sh

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
#set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

#ensure parallel builds
export MAKEFLAGS=-j8

WARNINGS() {
  w=`cat $1 | grep warning: | grep -v "failed to load external entity" | grep -v "are missing in source code comment block" | grep -v "g-ir-scanner:" | grep -v "argument unused during compilation:" | wc -l | awk '{print $1}'`
  if ( test $w -gt 0 )
  then
    echo "EXITING. $w warnings encountered"
    echo
    cat $1 | grep warning: | grep -v "failed to load external entity" | grep -v "are missing in source code comment block" | grep -v "g-ir-scanner:" | grep -v "argument unused during compilation:"
    exit 1
  fi
}

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

BDIR() {
  BDIR=$TOP/build-$1
}

BUILD() {
  echo "===== START BUILD: $1 ======"
  cd $TOP
  BDIR $1
  mkdir -p $BDIR
  cd $BDIR
  rm -rf *
  cmake .. $2 || exit 1
  make |& tee make.out || exit 1
  WARNINGS make.out
  make test |& tee make-test.out || exit 1
  cd ..
  rm -rf $BDIR
  echo "===== END BUILD: $1 ======"
}

SET_GCC() {
  export CC=gcc; export CXX=g++
}

SET_CLANG() {
  export CC=clang; export CXX=clang++
}

CPPCHECK() {
  echo "===== START SETUP FOR CPPCHECK: $1 ======"

  #first build it
  cd $TOP
  BDIR "$1-cppcheck"
  mkdir -p $BDIR
  cd $BDIR
  rm -rf *
  cmake .. $2
  make |& tee make.out

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

SPLINT() {
  echo "===== START SPLINT: $1 ======"

  #first build it
  cd $TOP
  BDIR "$1-splint"
  mkdir -p $BDIR
  cd $BDIR
  rm -rf *
  cmake .. $2
  make |& tee make.out

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

BUILD_TIDY() {
  echo "===== START CLANG-TIDY: $1 ====="
  tidyopts="*,-modernize*,-google-build-using-namespace,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-type-const-cast,-cppcoreguidelines-pro-type-reinterpret-cast,-llvm-include-order,-cppcoreguidelines-pro-type-vararg"
  cd $TOP
  BDIR "$1-tidy"
  mkdir -p $BDIR
  cd $BDIR
  rm -rf *
  cmake .. $2 -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=$tidyopts"
  cmake --build . |& tee make-tidy.out
  WARNINGS make-tidy.out
  cd ..
  rm -rf $BDIR
  echo "===== END CLANG-TIDY: $1 ====="
}

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

#MAIN
TOP=`dirname $0`
cd $TOP
cd ..
TOP=`pwd`
BDIR=""

CMAKEOPTS="-DCMAKE_BUILD_TYPE=Debug -DUSE_INTEROPERABLE_VTIMEZONES=True -DWITH_BDB=True -DGOBJECT_INTROSPECTION=True -DICAL_GLIB=True"

KRAZY

#GCC based tests
SET_GCC
BUILD test1-gcc ""
BUILD test2-gcc "$CMAKEOPTS"
BUILD test1cross-gcc "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake"
BUILD test2cross-gcc "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake $CMAKEOPTS"

SPLINT test2-gcc "$CMAKEOPTS"
CPPCHECK test2-gcc "$CMAKEOPTS"

#Clang based tests
SET_CLANG
BUILD test1-clang ""
BUILD test2-clang "$CMAKEOPTS"
BUILD test1cross-clang "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake"
BUILD test2cross-clang "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake $CMAKEOPTS"

BUILD_TIDY test2 "$CMAKEOPTS"

BUILD asan1 "-DADDRESS_SANITIZER=True -DUSE_INTEROPERABLE_VTIMEZONES=True -DWITH_BDB=True"

echo "ALL TESTS COMPLETED SUCCESSFULLY"
