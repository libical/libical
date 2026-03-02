#!/bin/bash

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

USAGE() {
  echo "Usage: $(basename "$0") [-s]"
  echo "where, the -s means to build statically"
  exit 1
}

staticBuild=0
if (test $# -gt 1); then
  USAGE
elif (test $# -eq 1); then
  if (test "$1" == "-s"); then
    staticBuild=1
  else
    USAGE
  fi
fi

TOP=$(readlink -nf "$0")
TOP=$(dirname "$TOP")
cd "$TOP/.."
TOP=$(pwd)
BRANCH=$(git branch --show-current | awk -F/ '{print $NF}')
BDIR="$TOP/build-$BRANCH-gcc"
INSTALLDIR="$HOME/tmp/libical-$BRANCH"

export ASAN_OPTIONS="detect_leaks=0:verify_asan_link_order=0" #link_order is needed with different ld on Fedora (like gold)

CMAKE_STRICT=""
CMAKE_VERSION4_OPTIONS=""
if (test "$BRANCH" != "3.0"); then
  CMAKE_STRICT="--warn-uninitialized -Werror=dev"
  CMAKE_VERSION4_OPTIONS="\
    -DLIBICAL_DEVMODE=ON \
    -DLIBICAL_DEVMODE_MEMORY_CONSISTENCY=ON \
    -DLIBICAL_DEVMODE_SYNCMODE_THREADLOCAL=ON \
    -DLIBICAL_DEVMODE_ADDRESS_SANITIZER=ON \
    -DLIBICAL_DEVMODE_LEAK_SANITIZER=ON \
    -DLIBICAL_DEVMODE_UNDEFINED_SANITIZER=ON \
    -DLIBICAL_BUILD_VZIC=ON \
  "
fi

if (test "$(uname -s)" = "Darwin"); then
  export XML_CATALOG_FILES=/usr/local/etc/xml/catalog
fi

if (test $staticBuild -eq 0); then
  rm -rf "$BDIR"
  mkdir -p "$BDIR" &&
    cd "$BDIR"
  # shellcheck disable=SC2086,SC2226
  cmake -S .. \
    $CMAKE_STRICT \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DLIBICAL_STATIC=OFF \
    -DLIBICAL_ENABLE_BUILTIN_TZDATA=OFF \
    -DLIBICAL_BUILD_TESTING=ON \
    -DLIBICAL_BUILD_DOCS=ON \
    -DLIBICAL_BUILD_EXAMPLES=ON \
    -DLIBICAL_CXX_BINDINGS=ON \
    -DLIBICAL_JAVA_BINDINGS=OFF \
    -DLIBICAL_GOBJECT_INTROSPECTION=ON \
    -DLIBICAL_GLIB_VAPI=ON \
    -DLIBICAL_GLIB_BUILD_DOCS=ON \
    $CMAKE_VERSION4_OPTIONS \
    -DLIBICAL_BUILD_TESTING_BIGFUZZ=ON \
    -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" &&
    (cd "$TOP" && ln -sf "$BDIR/compile_commands.json") &&
    ninja &&
    ninja test &&
    ninja install &&
    ninja docs &&
    ninja build-book
  if (test $? -ne 0); then exit; fi
  ninja uninstall && rm -rf "$INSTALLDIR"

else #static build
  BDIR="$BDIR-static"
  INSTALLDIR="$INSTALLDIR-static"

  #rm -rf "$BDIR"
  mkdir -p "$BDIR" &&
    cd "$BDIR"
  # shellcheck disable=SC2086,SC2226
  cmake -S .. \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DLIBICAL_STATIC=ON \
    -DLIBICAL_BUILD_TESTING=ON \
    -DLIBICAL_BUILD_DOCS=ON \
    -DLIBICAL_BUILD_EXAMPLES=ON \
    -DLIBICAL_CXX_BINDINGS=ON \
    -DLIBICAL_JAVA_BINDINGS=OFF \
    -DLIBICAL_GOBJECT_INTROSPECTION=OFF \
    -DLIBICAL_GLIB_VAPI=OFF \
    -DLIBICAL_GLIB_BUILD_DOCS=OFF \
    $CMAKE_VERSION4_OPTIONS \
    -DLIBICAL_BUILD_TESTING_BIGFUZZ=ON \
    -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" &&
    (cd "$TOP" && ln -sf "$BDIR/compile_commands.json") &&
    ninja &&
    ninja test &&
    ninja install &&
    ninja docs &&
    ninja build-book
  if (test $? -ne 0); then exit; fi
  ninja uninstall && rm -rf "$INSTALLDIR"
fi
