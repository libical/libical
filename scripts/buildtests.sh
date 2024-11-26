#!/bin/bash

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

#ensure parallel builds
export MAKEFLAGS=-j4
export CMAKE_BUILD_PARALLEL_LEVEL=4

if (test "$(uname -s)" = "Darwin"); then
  #needed to find homebrew's libxml2 and libffi on osx
  export PKG_CONFIG_PATH=/usr/local/opt/libffi/lib/pkgconfig:/usr/local/opt/libxml2/lib/pkgconfig
  #needed to find the homebrew installed xml2 catalog
  export XML_CATALOG_FILES=/usr/local/etc/xml/catalog
fi

##### START FUNCTIONS #####

#function HELP
# print a help message and exit
HELP() {
  echo
  echo "Usage: $(basename "$0") [OPTIONS]"
  echo
  echo "Run build tests"
  echo "Options:"
  echo " -m, --no-cmake-compat      Don't require CMake version compatibility"
  echo " -p, --no-precommit         Don't run pre-commit test"
  echo " -k, --no-krazy             Don't run any Krazy tests"
  echo " -s, --no-splint            Don't run any splint tests"
  echo " -b, --no-scan              Don't run any scan-build tests"
  echo " -t, --no-tidy              Don't run any clang-tidy tests"
  echo " -c, --no-cppcheck          Don't run any cppcheck tests"
  echo " -g, --no-gcc-build         Don't run any gcc-build tests with Unix Makefiles"
  echo " -n, --no-ninja-gcc-build   Don't run any gcc build tests with ninja"
  echo " -l, --no-clang-build       Don't run any clang-build tests"
  echo " -f, --no-fortify-build     Don't run the FORTIFY-build tests (gcc12)"
  echo " -x, --no-memc-build        Don't run any MEMCONSIST-build (memory consistency) tests"
  echo " -a, --no-asan-build        Don't run any ASAN-build (sanitize-address) tests"
  echo " -d, --no-tsan-build        Don't run any TSAN-build (sanitize-threads) tests"
  echo " -u, --no-ubsan-build       Don't run any UBSAN-build (sanitize-undefined) tests"
  echo " -r, --no-threadlocal-build Don't run the THREADLOCAL-build tests"
  echo " -R, --reverse              Reverse polarity on the options"
  echo
}

REVERSE() {
  if [[ $1 -eq 1 && $reverse -eq 0 ]] || [[ $reverse -eq 1 && $1 -eq 0 ]]; then
    echo "go"
  else
    echo
  fi
}

COMMAND_EXISTS() {
  command -v "$1" >/dev/null 2>&1
  if (test $? != 0); then
    echo "$1 is not in your PATH. Either install this program or skip the associated test"
    if (test $# -gt 1); then
      echo "or disable this check by passing the $2 command-line option"
    fi
    exit 1
  fi
}

#function SET_NINJA
# set cmake generator to ninja
SET_NINJA() {
  export CMAKE_GENERATOR=Ninja
}
UNSET_NINJA() {
  unset CMAKE_GENERATOR
}

#function SET_GCC
# setup compiling with gcc
SET_GCC() {
  export CC=gcc
  export CXX=g++
}

#function SET_CLANG
# setup compiling with clang
SET_CLANG() {
  export CC=clang
  export CXX=clang++
}

#function SET_BDIR:
# set the name of the build directory for the current test
# $1 = the name of the current test
SET_BDIR() {
  BDIR=$TOP/build-$1
}

#function CHECK_WARNINGS:
# print non-whitelisted warnings found in the file and exit if there are any
# $1 = file to check
# $2 = warning keyword
# $3 = whitelist regex
CHECK_WARNINGS() {
  set +e
  declare -i numIssues
  if (test -z "$3"); then
    numIssues=$(grep "$2" <"$1" | sort | uniq | wc -l | awk '{print $1}')
  else
    numIssues=$(grep "$2" <"$1" | grep -v "$3" | sort | uniq | wc -l | awk '{print $1}')
  fi
  if (test $numIssues -gt 0); then
    echo "EXITING. $numIssues warnings encountered"
    echo
    if (test -n "$3"); then
      grep "$2" <"$1" | grep -v "$3" | sort | uniq
    else
      grep "$2" <"$1" | sort | uniq
    fi
    exit 1
  fi
  set -e
}

#function COMPILE_WARNINGS:
# print warnings found in the compile-stage output
# $1 = file with the compile-stage output
COMPILE_WARNINGS() {
  whitelist='\(i-cal-object\.c\|libical-glib-scan\.c\|no[[:space:]]link[[:space:]]for:\|Value[[:space:]]descriptions\|unused[[:space:]]declarations\|G_ADD_PRIVATE\|g_type_class_add_private.*is[[:space:]]deprecated\|g-ir-scanner:\|/gobject/gtype\.h\|clang.*argument[[:space:]]unused[[:space:]]during[[:space:]]compilation\|U_PLATFORM_HAS_WINUWP_API\|const[[:space:]]DBT\)'
  CHECK_WARNINGS "$1" "warning:" "$whitelist"
}

#function CPPCHECK_WARNINGS:
# print warnings found in the cppcheck output
# $1 = file with the cppcheck output
CPPCHECK_WARNINGS() {
  CHECK_WARNINGS "$1" "\(warning\|error\|portability\)" ""
}

#function TIDY_WARNINGS:
# print warnings find in the clang-tidy output
# $1 = file with the clang-tidy output
TIDY_WARNINGS() {
  #whitelist='\(Value[[:space:]]descriptions\|unused[[:space:]]declarations\|g-ir-scanner:\|clang.*argument[[:space:]]unused[[:space:]]during[[:space:]]compilation\|modernize-\|cppcoreguidelines-pro-type-const-cast\|cppcoreguidelines-pro-type-vararg\|cppcoreguidelines-pro-type-reinterpret-cast\|cppcoreguidelines-owning-memory\|fuchsia.*\|hicpp-use-auto\|hicpp-no-malloc\|hicpp-use-nullptr\|hicpp-exception-baseclass\|hicpp-vararg\|cppcoreguidelines-pro-type-vararg\|cppcoreguidelines-pro-bounds-pointer-arithmetic\|google-build-using-namespace\|llvm-include-order\|hicpp-use-equals-default\|cppcoreguidelines-no-malloc\|g_type_class_add_private.*is[[:space:]]deprecated\)'
  whitelist='\(no[[:space:]]link[[:space:]]for:\|Value[[:space:]]descriptions\|unused[[:space:]]declarations\|G_ADD_PRIVATE\|g_type_class_add_private.*is[[:space:]]deprecated\|g-ir-scanner:\|clang.*argument[[:space:]]unused[[:space:]]during[[:space:]]compilation\|libical-glib-scan\.c\)'
  CHECK_WARNINGS "$1" "warning:" "$whitelist"
}

#function SCAN_WARNINGS:
# print warnings found in the scan-build output
# $1 = file with the scan-build output
SCAN_WARNINGS() {
  whitelist='\(no[[:space:]]link[[:space:]]for:\|g_type_class_add_private.*is[[:space:]]deprecated\|libical-glib-scan\.c\|/i-cal-object\.c\|/vcc\.c\|/vobject\.c\|/icalsslexer\.c\|Value[[:space:]]descriptions\|unused[[:space:]]declarations\|icalerror.*Dereference[[:space:]]of[[:space:]]null[[:space:]]pointer\|G_ADD_PRIVATE\|g-ir-scanner:\)'
  CHECK_WARNINGS "$1" "warning:" "$whitelist"
}

#function CONFIGURE:
# creates the builddir and runs CMake with the specified options
# $1 = the name of the test
# $2 = CMake options
CONFIGURE() {
  SET_BDIR "$1"
  rm -rf "$BDIR"
  mkdir "$BDIR"
  cd "$BDIR" || exit 1
  # shellcheck disable=SC2086
  cmake --warn-uninitialized -Werror=dev .. $2 2>&1 | tee cmake.out || exit 1
  declare -i numWarnings
  declare -i numDeprecates
  set +e
  numWarnings=$(grep -ic "cmake warning" cmake.out)
  numDeprecates=$(grep -ic "cmake deprecat" cmake.out)
  set -e
  if (test $numWarnings -gt 0 -o $numDeprecates -gt 0); then
    echo "cmake warnings encountered"
    exit 1
  fi
}

#function CLEAN:
# remove the builddir
CLEAN() {
  cd "$TOP" || exit 1
  rm -rf "$BDIR"
}

#function BUILD:
# runs a build test, where build means: configure, compile, link and run the unit tests
# $1 = the name of the test
# $2 = CMake options
BUILD() {
  cd "$TOP" || exit 1
  CONFIGURE "$1" "$2"
  cmake --build . 2>&1 | tee make.out || exit 1
  COMPILE_WARNINGS make.out

  if (test "$(uname -s)" = "Darwin"); then
    export DYLD_LIBRARY_PATH=$BDIR/lib
  else
    export LD_LIBRARY_PATH=$BDIR/lib
  fi
  ctest . 2>&1 | tee make-test.out || exit 1
  CLEAN
}

#function GCC_BUILD:
# runs a build test using gcc
# $1 = the name of the test (which will have "-gcc" appended to it)
# $2 = CMake options
GCC_BUILD() {
  name="$1-gcc"
  if (test -z "$(REVERSE $rungccbuild)"); then
    echo "===== GCC BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "gcc"
  echo "===== START GCC BUILD: $1 ======"
  SET_GCC
  BUILD "$name" "$2"
  echo "===== END GCC BUILD: $1 ======"
}

#function FORTIFY_BUILD:
# runs a build test using gcc (v12 or higher) with fortify CFLAGS
# $1 = the name of the test (which will have "-fortify" appended to it)
# $2 = CMake options
FORTIFY_BUILD() {
  name="$1-fortify"
  if (test -z "$(REVERSE $runfortifybuild)"); then
    echo "===== FORTIFY BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "gcc"
  declare -i gccVersion
  gccVersion=$(gcc -dumpversion)
  if (test $gccVersion -lt 12); then
    echo "Sorry, gcc must be version 12 or higher to support fortify. Exiting..."
    exit 1
  fi
  echo "===== START FORTIFY BUILD: $1 ======"
  SET_GCC
  export CFLAGS="-Og -gdwarf-5 -fno-optimize-sibling-calls -Wall -W -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -D_FORTIFY_SOURCE=3 -fPIC -grecord-gcc-switches -fno-allow-store-data-races -fstack-protector-strong -fstack-clash-protection -fcf-protection=full --param=ssp-buffer-size=1"
  BUILD "$name" "$2"
  unset CFLAGS
  echo "===== END FORTIFY BUILD: $1 ======"
}

#function NINJA_GCC_BUILD:
# runs a build test using gcc using the Ninja cmake generator
# $1 = the name of the test (which will have "-ninjagcc" appended to it)
# $2 = CMake options
NINJA_GCC_BUILD() {
  name="$1-ninjagcc"
  if (test -z "$(REVERSE $runninja)"); then
    echo "===== NINJA_GCC BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "gcc"
  echo "===== START NINJA_GCC BUILD: $1 ======"
  SET_GCC
  SET_NINJA
  BUILD "$name" "$2"
  UNSET_NINJA
  echo "===== END NINJA_GCC BUILD: $1 ======"
}

#function CLANG_BUILD:
# runs a build test using clang
# $1 = the name of the test (which will have "-clang" appended to it)
# $2 = CMake options
CLANG_BUILD() {
  name="$1-clang"
  if (test -z "$(REVERSE $runclangbuild)"); then
    echo "===== CLANG BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "clang"
  echo "===== START CLANG BUILD: $1 ======"
  SET_CLANG
  BUILD "$name" "$2"
  echo "===== END CLANG BUILD: $1 ======"
}

#function MEMCONSIST_BUILD:
# runs a gcc memory consistency build test
# $1 = the name of the test (which will have "-mem" appended to it)
# $2 = CMake options
MEMCONSIST_BUILD() {
  name="$1-mem"
  if (test -z "$(REVERSE $runmemcbuild)"); then
    echo "===== MEMCONSIST BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  echo "===== START MEMCONSIST BUILD: $1 ======"
  BUILD "$name" "-DLIBICAL_DEVMODE_MEMORY_CONSISTENCY=True $2"
  echo "===== END MEMCONSIST BUILD: $1 ======"
}

#function ASAN_BUILD:
# runs a clang ASAN build test
# $1 = the name of the test (which will have "-asan" appended to it)
# $2 = CMake options
ASAN_BUILD() {
  name="$1-asan"
  if (test -z "$(REVERSE $runasanbuild)"); then
    echo "===== ASAN BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  echo "===== START ASAN BUILD: $1 ======"
  SET_GCC
  export ASAN_OPTIONS=verify_asan_link_order=0 #seems to be needed with different ld on Fedora (like gold)
  BUILD "$name" "-DLIBICAL_DEVMODE_ADDRESS_SANITIZER=True $2"
  echo "===== END ASAN BUILD: $1 ======"
}

#function TSAN_BUILD:
# runs a clang TSAN build test
# $1 = the name of the test (which will have "-tsan" appended to it)
# $2 = CMake options
TSAN_BUILD() {
  name="$1-tsan"
  if (test -z "$(REVERSE $runtsanbuild)"); then
    echo "===== TSAN BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  echo "===== START TSAN BUILD: $1 ======"
  SET_GCC
  BUILD "$name" "-DLIBICAL_DEVMODE_THREAD_SANITIZER=True $2"
  echo "===== END TSAN BUILD: $1 ======"
}

#function UBSAN_BUILD:
# runs a clang UBSAN build test
# $1 = the name of the test (which will have "-ubsan" appended to it)
# $2 = CMake options
UBSAN_BUILD() {
  name="$1-ubsan"
  if (test -z "$(REVERSE $runubsanbuild)"); then
    echo "===== UBSAN BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  echo "===== START UBSAN BUILD: $1 ======"
  SET_GCC
  ulimit -S -t 180     # oss-fuzz uses 60 seconds which is too low for us
  ulimit -S -m 2621440 # oss-fuzz uses 2560Mb
  #export UBSAN_OPTIONS=allocator_release_to_os_interval_ms=500:halt_on_error=1:handle_abort=2:handle_segv=2:handle_sigbus=2:handle_sigfpe=2:handle_sigill=2:print_stacktrace=1:print_summary=1:print_suppressions=0:silence_unsigned_overflow=1:symbolize=1:use_sigaltstack=1
  # halt_on_error=1 finds all possible ubs issues
  export UBSAN_OPTIONS=allocator_release_to_os_interval_ms=500:handle_abort=2:handle_segv=2:handle_sigbus=2:handle_sigfpe=2:handle_sigill=2:print_stacktrace=1:print_summary=1:print_suppressions=0:silence_unsigned_overflow=1:symbolize=1:use_sigaltstack=1
  export CFLAGS="-g -fno-omit-frame-pointer"
  BUILD "$name" "-DLIBICAL_DEVMODE_UNDEFINED_SANITIZER=True $2"
  unset CFLAGS
  ulimit -S -t unlimited
  ulimit -S -m unlimited
  echo "===== END UBSAN BUILD: $1 ======"
}

#function THREADLOCAL_BUILD() {
# runs a THREADLOCAL test
# $1 = the name of the test (which will have "-threadlocal" appended to it)
# $2 = CMake options
THREADLOCAL_BUILD() {
  name="$1-threadlocal"
  if (test -z "$(REVERSE $runthreadlocalbuild)"); then
    echo "===== THREADLOCAL BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  echo "===== START THREADLOCAL BUILD: $1 ======"
  SET_GCC
  BUILD "$name" "-DLIBICAL_SYNCMODE_THREADLOCAL=True $2"
  echo "===== END THREADLOCAL BUILD: $1 ======"
}

#function CPPCHECK
# runs a cppcheck test, which means: configure, compile, link and run cppcheck
# $1 = the name of the test (which will have "-cppcheck" appended to it)
# $2 = CMake options
CPPCHECK() {
  name="$1-cppcheck"
  if (test -z "$(REVERSE $runcppcheck)"); then
    echo "===== CPPCHECK TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "cppcheck" "-c"
  echo "===== START SETUP FOR CPPCHECK: $1 ======"

  #first build it
  cd "$TOP" || exit 1
  SET_GCC
  CONFIGURE "$name" "$2"
  make 2>&1 | tee make.out || exit 1

  echo "===== START CPPCHECK: $1 ======"
  cd "$TOP" || exit 1
  cppcheck --quiet \
    --language=c++ \
    --std=c++11 \
    --force --error-exitcode=1 --inline-suppr \
    --enable=warning,performance,portability \
    --check-level=exhaustive \
    --disable=missingInclude \
    --suppress=passedByValue --suppress=ctuOneDefinitionRuleViolation \
    --template='{file}:{line},{severity},{id},{message}' \
    --checkers-report=cppcheck-report.txt \
    -U PVL_USE_MACROS \
    -I "$BDIR" \
    -I "$BDIR/src/libical" \
    -I "$BDIR/src/libicalss" \
    -I "$BDIR/src/libicalvcal" \
    -I "$BDIR/src/libicalvcard" \
    -I "$BDIR/src/libical-glib" \
    -I "$TOP/src/libical" \
    -I "$TOP/src/libicalss" \
    -I "$TOP/src/libicalvcal" \
    -I "$TOP/src/libicalvcard" \
    -I "$TOP/src/libical-glib" \
    -i "$TOP/src/Net-ICal-Libical" \
    -i "$TOP/src/libicalss/icalssyacc.c" \
    -i "$TOP/src/libicalss/icalsslexer.c" \
    -i "$TOP/src/libicalvcal/vcc.c" \
    "$TOP/src" "$BDIR"/src/libical/icalderived* 2>&1 |
    tee cppcheck.out
  CPPCHECK_WARNINGS cppcheck.out
  rm -f cppcheck.out cppcheck-report.txt
  CLEAN
  echo "===== END CPPCHECK: $1 ======"
}

#function SPLINT
# runs a splint test, which means: configure, compile, link and run splint
# $1 = the name of the test (which will have "-splint" appended to it
# $2 = CMake options
SPLINT() {
  name="$1-splint"
  if (test -z "$(REVERSE $runsplint)"); then
    echo "===== SPLINT TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "splint" "-s"
  echo "===== START SETUP FOR SPLINT: $1 ======"

  #first build it
  cd "$TOP" || exit 1
  SET_GCC
  CONFIGURE "$name" "$2"
  make 2>&1 | tee make.out || exit 1

  echo "===== START SPLINT: $1 ======"
  cd "$TOP" || exit 1
  set +e
  files=$(find src -name "*.c$" -o -name "*.h$" |
    # skip C++
    grep -v _cxx | grep -v /Net-ICal-Libical |
    # skip lex/yacc
    grep -v /icalssyacc | grep -v /icalsslexer |
    # skip test programs
    grep -v /test/ | grep -v /vcaltest\.c | grep -v /vctest\.c |
    # skip builddirs
    grep -v build-)
  files="$files $BDIR/src/libical/*.c $BDIR/src/libical/*.h"

  # shellcheck disable=SC2086
  splint $files \
    -badflag \
    -preproc \
    -weak -warnposix \
    -modobserver -initallelements -redef \
    -linelen 1000 \
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
    -DLIBICAL_VCARD_EXPORT=extern \
    -DLIBICAL_ICAL_NO_EXPORT="" \
    -DLIBICAL_ICALSS_NO_EXPORT="" \
    -DLIBICAL_VCAL_NO_EXPORT="" \
    -DLIBICAL_VCARD_NO_EXPORT="" \
    -DENOENT=1 -DENOMEM=1 -DEINVAL=1 -DSIGALRM=1 \
    "$(pkg-config glib-2.0 --cflags)" \
    "$(pkg-config libxml-2.0 --cflags)" \
    -I "$BDIR" \
    -I "$BDIR/src" \
    -I "$BDIR/src/libical" \
    -I "$BDIR/src/libicalss" \
    -I "$BDIR/src/libicalvcard" \
    -I "$BDIR/src/libical-glib" \
    -I "$TOP" \
    -I "$TOP/src" \
    -I "$TOP/src/libical" \
    -I "$TOP/src/libicalss" \
    -I "$TOP/src/libicalvcal" \
    -I "$TOP/src/libicalvcard" \
    -I "$TOP/src/libical-glib" |
    grep -v '[[:space:]]Location[[:space:]]unknown[[:space:]]' |
    grep -v '[[:space:]]Code[[:space:]]cannot[[:space:]]be[[:space:]]parsed.' |
    cat - 2>&1 | tee "splint-$name.out"
  set -e
  declare -i status
  status=${PIPESTATUS[0]}
  if (test $status -gt 0); then
    echo "Splint warnings encountered.  Exiting..."
    exit 1
  fi
  CLEAN
  rm "splint-$name.out"
  echo "===== END SPLINT: $1 ======"
}

#function CLANGTIDY
# runs a clang-tidy test, which means: configure, compile, link and run clang-tidy
# $1 = the name of the test (which will have "-tidy" appended)
# $2 = CMake options
CLANGTIDY() {
  if (test -z "$(REVERSE $runtidy)"); then
    echo "===== CLANG-TIDY TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "clang-tidy" "-t"
  echo "===== START CLANG-TIDY: $1 ====="
  cd "$TOP" || exit 1
  SET_CLANG
  CONFIGURE "$1-tidy" "$2 -DCMAKE_CXX_CLANG_TIDY=clang-tidy"
  cmake --build . 2>&1 | tee make-tidy.out || exit 1
  TIDY_WARNINGS make-tidy.out
  CLEAN
  echo "===== END CLANG-TIDY: $1 ====="
}

#function CLANGSCAN
# runs a scan-build, which means: configure, compile and link using scan-build
# $1 = the name of the test (which will have "-scan" appended)
# $2 = CMake options
CLANGSCAN() {
  if (test -z "$(REVERSE $runscan)"); then
    echo "===== SCAN-BUILD TEST $1 DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "scan-build" "-b"
  echo "===== START SCAN-BUILD: $1 ====="
  cd "$TOP" || exit 1

  #configure specially with scan-build
  SET_BDIR "$1-scan"
  rm -rf "$BDIR"
  mkdir "$BDIR"
  cd "$BDIR" || exit 1
  # shellcheck disable=SC2086
  scan-build cmake .. $2 || exit 1

  scan-build make 2>&1 | tee make-scan.out || exit 1
  SCAN_WARNINGS make-scan.out
  CLEAN
  echo "===== END CLANG-SCAN: $1 ====="
}

#function KRAZY
# runs a krazy2 test
KRAZY() {
  if (test -z "$(REVERSE $runkrazy)"); then
    echo "===== KRAZY TEST DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "krazy2all" "-k"
  echo "===== START KRAZY ====="
  cd "$TOP" || exit 1
  krazy2all 2>&1 | tee krazy.out
  status=$?
  if (test $status -gt 0); then
    echo "Krazy warnings encountered.  Exiting..."
    exit 1
  fi
  rm -f krazy.out
  echo "===== END KRAZY ======"
}

#function PRECOMMIT
# run pre-commit
PRECOMMIT() {
  if (test -z "$(REVERSE $runprecommit)"); then
    echo "===== PRECOMMIT DISABLED DUE TO COMMAND LINE OPTION ====="
    return
  fi
  COMMAND_EXISTS "pre-commit" "-p"
  echo "===== START PRECOMMIT ====="
  cd "$TOP" || exit 1
  pre-commit run --all-files 2>&1 | tee precommit.out
  status=$?
  if (test $status -gt 0); then
    echo "pre-commit issues encountered.  Exiting..."
    exit 1
  fi
  rm -f precommit.out
  echo "===== END PRECOMMIT ======"
}

##### END FUNCTIONS #####

options=$(getopt -o "hmpksbtcgnlfxadurR" --long "help,no-cmake-compat,no-precommit,no-krazy,no-splint,no-scan,no-tidy,no-cppcheck,no-gcc-build,no-ninja-gcc-build,no-clang-build,no-fortify-build,no-memc-build,no-asan-build,no-tsan-build,no-ubsan-build,no-threadlocal-build,reverse" -- "$@")
eval set -- "$options"

reverse=0
cmakecompat=1
runkrazy=1
runprecommit=1
runcppcheck=1
runtidy=1
runsplint=1
runscan=1
runninja=1
runclangbuild=1
rungccbuild=1
runasanbuild=1
runtsanbuild=1
runubsanbuild=1
runmemcbuild=1
runfortifybuild=1
runthreadlocalbuild=1
while true; do
  case "$1" in
  -h | --help)
    HELP
    exit 1
    ;;
  -m | --no-cmake-compat)
    cmakecompat=0
    shift
    ;;
  -k | --no-krazy)
    runkrazy=0
    shift
    ;;
  -p | --no-precommit)
    runprecommit=0
    shift
    ;;
  -c | --no-cppcheck)
    runcppcheck=0
    shift
    ;;
  -t | --no-tidy)
    runtidy=0
    shift
    ;;
  -b | --no-scan)
    runscan=0
    shift
    ;;
  -s | --no-splint)
    runsplint=0
    shift
    ;;
  -n | --no-ninja-gcc-build)
    runninja=0
    shift
    ;;
  -l | --no-clang-build)
    runclangbuild=0
    shift
    ;;
  -g | --no-gcc-build)
    rungccbuild=0
    shift
    ;;
  -a | --no-asan-build)
    runasanbuild=0
    shift
    ;;
  -d | --no-tsan-build)
    runtsanbuild=0
    shift
    ;;
  -u | --no-ubsan-build)
    runubsanbuild=0
    shift
    ;;
  -x | --no-memc-build)
    runmemcbuild=0
    shift
    ;;
  -f | --no-fortify-build)
    runfortifybuild=0
    shift
    ;;
  -r | --no-threadlocal-build)
    runthreadlocalbuild=0
    shift
    ;;
  -R | --reverse)
    reverse=1
    shift
    ;;
  --)
    shift
    break
    ;;
  *)
    echo "Internal error!"
    exit 1
    ;;
  esac
done

#MAIN
TOP=$(dirname "$0")
cd "$TOP" || exit 1
cd ..
TOP=$(pwd)
BDIR=""

#use minimum cmake version unless the --no-cmake-compat option is specified
if (test ! -z "$(REVERSE $cmakecompat)"); then
  if (test ! -e "$TOP/CMakeLists.txt"); then
    echo "Unable to locate the project top-level CMakeLists.txt.  Fix me"
    exit 1
  fi
  # read the min required CMake version from the top-level CMake file
  # shellcheck disable=SC2086
  minCMakeVers=$(grep -i cmake_minimum_required $TOP/CMakeLists.txt | grep VERSION | sed 's/^.*VERSION\s*//' | cut -d. -f1-2 | sed 's/\s*).*$//' | awk '{print $NF}')
  # adjust PATH
  X=$(echo "$minCMakeVers" | cut -d. -f1)
  Y=$(echo "$minCMakeVers" | cut -d. -f2)
  if (test -z "$X" -o -z "$Y"); then
    echo "Bad CMake version encountered in the $TOP/CMakeLists.txt"
    exit 1
  fi
  Z=$(echo "$minCMakeVers" | cut -d. -f3)
  if (test -z "$Z"); then
    minCMakeVers="$minCMakeVers.0"
  fi
  export PATH=/usr/local/opt/cmake-$minCMakeVers/bin:$PATH
  # check the version
  declare -i isCMakeVersion
  isCMakeVersion=$(cmake --version | head -1 | grep -c "$minCMakeVers")
  if (test $isCMakeVersion -ne 1); then
    echo "Not using cmake version $minCMakeVers"
    echo "Maybe you need to install it into /usr/local/opt/cmake-$minCMakeVers (or use the -m option)"
    exit 1
  fi
fi

#use non-Ninja cmake generator by-default
UNSET_NINJA

DEFCMAKEOPTS="-DCMAKE_BUILD_TYPE=Release -DNDEBUG=1"
CMAKEOPTS="-DCMAKE_BUILD_TYPE=Debug -DGOBJECT_INTROSPECTION=False -DICAL_GLIB=False -DICAL_BUILD_DOCS=False"
UUCCMAKEOPTS="$CMAKEOPTS -DCMAKE_DISABLE_FIND_PACKAGE_ICU=True"
TZCMAKEOPTS="$CMAKEOPTS -DUSE_BUILTIN_TZDATA=True"
LTOCMAKEOPTS="$CMAKEOPTS -DENABLE_LTO_BUILD=True"
GLIBOPTS="-DCMAKE_BUILD_TYPE=Debug -DICAL_GLIB=True -DGOBJECT_INTROSPECTION=True -DUSE_BUILTIN_TZDATA=OFF -DICAL_GLIB_VAPI=ON"
FUZZOPTS="-DLIBICAL_BUILD_TESTING_BIGFUZZ=True"

#Static code checkers
STATICCCHECKOPTS="\
-DCMAKE_BUILD_TYPE=Debug \
-DSHARED_ONLY=True \
-DWITH_CXX_BINDINGS=True \
-DUSE_BUILTIN_TZDATA=True \
-DICAL_GLIB=True \
-DGOBJECT_INTROSPECTION=True \
-DICAL_GLIB_VAPI=True \
-DICAL_GLIB_BUILD_DOCS=True \
-DLIBICAL_BUILD_TESTING=True \
-DLIBICAL_BUILD_EXAMPLES=True \
"
PRECOMMIT
KRAZY
SPLINT test "$STATICCCHECKOPTS"
CLANGSCAN test "$STATICCCHECKOPTS"
CLANGTIDY test "$STATICCCHECKOPTS"
CPPCHECK test "$STATICCCHECKOPTS"

#GCC based build tests
GCC_BUILD testgcc1 "$DEFCMAKEOPTS"
GCC_BUILD testgcc2 "$CMAKEOPTS"
GCC_BUILD testgcc3 "$UUCCMAKEOPTS"
if (test "$(uname -s)" = "Linux"); then
  GCC_BUILD testgcc4lto "$LTOCMAKEOPTS"
fi
GCC_BUILD testgcc4glib "$GLIBOPTS"
GCC_BUILD testgccnocxx "$CMAKEOPTS -DWITH_CXX_BINDINGS=off"
if (test "$(uname -s)" = "Linux"); then
  echo "Temporarily disable cross-compile tests"
#  GCC_BUILD testgcc1cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake"
#  GCC_BUILD testgcc2cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake $CMAKEOPTS"
fi
GCC_BUILD testgcc1builtin "-DUSE_BUILTIN_TZDATA=True"
GCC_BUILD testgcc2builtin "$TZCMAKEOPTS"

#Ninja build tests
NINJA_GCC_BUILD testninjagcc1 "$DEFCMAKEOPTS"
NINJA_GCC_BUILD testninjagcc2 "-DICAL_GLIB=True"
NINJA_GCC_BUILD testninjagcc3 "-DICAL_GLIB_VAPI=ON"
NINJA_GCC_BUILD testninjagcc4 "-DSHARED_ONLY=True -DICAL_GLIB_BUILD_DOCS=False"
NINJA_GCC_BUILD testninjagcc5 "-DSHARED_ONLY=True"
NINJA_GCC_BUILD testninjagcc6 "-DSTATIC_ONLY=True -DGOBJECT_INTROSPECTION=False -DICAL_GLIB_BUILD_DOCS=False"

# Clang based build tests
CLANG_BUILD testclang1 "$DEFCMAKEOPTS"
CLANG_BUILD testclang2 "$CMAKEOPTS"
CLANG_BUILD testclang3 "$UUCCMAKEOPTS"
#not supported with clang yet CLANG_BUILD testclang4lto "$LTOCMAKEOPTS"
CLANG_BUILD testclang4glib "$GLIBOPTS"
if (test "$(uname -s)" = "Linux"); then
  echo "Temporarily disable cross-compile tests"
#  CLANG_BUILD testclang1cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake"
#  CLANG_BUILD testclang2cross "-DCMAKE_TOOLCHAIN_FILE=$TOP/cmake/Toolchain-Linux-GCC-i686.cmake $CMAKEOPTS"
fi

#Fortify build
FORTIFY_BUILD test1fortify "$DEFCMAKEOPTS"
FORTIFY_BUILD test2fortify "$CMAKEOPTS"
FORTIFY_BUILD test3fortify "$TZCMAKEOPTS"
FORTIFY_BUILD test4fortify "$UUCCMAKEOPTS"
FORTIFY_BUILD test5fortify "$GLIBOPTS"
FORTIFY_BUILD test6fortify "$FUZZOPTS"

#Memory consistency check
MEMCONSIST_BUILD test1memc ""
MEMCONSIST_BUILD test2memc "$CMAKEOPTS"
MEMCONSIST_BUILD test3memc "$TZCMAKEOPTS"
MEMCONSIST_BUILD test4memc "$UUCCMAKEOPTS"
MEMCONSIST_BUILD test5memc "$GLIBOPTS"
MEMCONSIST_BUILD test6memc "$FUZZOPTS"

#Address sanitizer
ASAN_BUILD test1asan "$DEFCMAKEOPTS"
ASAN_BUILD test2asan "$CMAKEOPTS"
ASAN_BUILD test3asan "$TZCMAKEOPTS"
ASAN_BUILD test4asan "$UUCCMAKEOPTS"
ASAN_BUILD test5asan "$GLIBOPTS"
ASAN_BUILD test6asan "$FUZZOPTS"

#Thread sanitizer
#libical-glib tests fail tsan with /lib64/libtsan.so.2: cannot allocate memory in static TLS block
#TSAN_BUILD test1tsan "$DEFCMAKEOPTS"
TSAN_BUILD test2tsan "$CMAKEOPTS"
TSAN_BUILD test3tsan "$TZCMAKEOPTS"
TSAN_BUILD test4tsan "$UUCCMAKEOPTS"
#TSAN_BUILD test5tsan "$GLIBOPTS"
#TSAN_BUILD test6tsan "$FUZZOPTS"

#Undefined sanitizer
UBSAN_BUILD test1ubsan "$DEFCMAKEOPTS"
UBSAN_BUILD test2ubsan "$CMAKEOPTS"
UBSAN_BUILD test3ubsan "$TZCMAKEOPTS"
UBSAN_BUILD test4ubsan "$UUCCMAKEOPTS"
UBSAN_BUILD test5ubsan "$GLIBOPTS"
UBSAN_BUILD test6ubsan "$FUZZOPTS"

#Threadlocal
THREADLOCAL_BUILD test1threadlocal "$DEFCMAKEOPTS"
THREADLOCAL_BUILD test2threadlocal "$CMAKEOPTS"
THREADLOCAL_BUILD test3threadlocal "$TZCMAKEOPTS"
THREADLOCAL_BUILD test4threadlocal "$UUCCMAKEOPTS"
THREADLOCAL_BUILD test5threadlocal "$GLIBOPTS"
THREADLOCAL_BUILD test6threadlocal "$FUZZOPTS"

echo "ALL TESTS COMPLETED SUCCESSFULLY"
