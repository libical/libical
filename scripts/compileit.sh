#!/bin/bash

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

TOP=$(readlink -nf "$0")
TOP=$(dirname "$TOP")
cd "$TOP/.."
TOP=$(pwd)
BRANCH=$(git branch --show-current | awk -F/ '{print $NF}')
BDIR="$TOP/build-$BRANCH-gcc"
INSTALLDIR="$HOME/tmp/libical-$BRANCH"

CMAKE_VERSION4_OPTIONS=""
if (test "$BRANCH" != "3.0"); then
  CMAKE_VERSION4_OPTIONS="\
    -DLIBICAL_DEVMODE=ON \
    -DLIBICAL_DEVMODE_MEMORY_CONSISTENCY=ON \
    -DLIBICAL_DEVMODE_SYNCMODE_THREADLOCAL=ON \
  "
fi

if (test "$(uname -s)" = "Darwin"); then
  export XML_CATALOG_FILES=/usr/local/etc/xml/catalog
fi

rm -rf "$BDIR"
mkdir -p "$BDIR" &&
  cd "$BDIR"
# shellcheck disable=SC2086,SC2226
cmake -S .. \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DLIBICAL_STATIC=OFF \
  -DLIBICAL_ENABLE_BUILTIN_TZDATA=OFF \
  -DLIBICAL_BUILD_TESTING=ON \
  -DLIBICAL_BUILD_DOCS=ON \
  -DLIBICAL_BUILD_EXAMPLES=ON \
  -DLIBICAL_CXX_BINDINGS=ON \
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

#static build testing
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
