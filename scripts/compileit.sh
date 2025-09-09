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

CMAKE_VERSION4_OPTIONS=""
if (test "$BRANCH" != "3.0"); then
  CMAKE_VERSION4_OPTIONS="\
    -DLIBICAL_DEVMODE=ON \
    -DLIBICAL_DEVMODE_MEMORY_CONSISTENCY=ON \
    -DLIBICAL_SYNCMODE_THREADLOCAL=ON \
  "
fi

if (test "$(uname -s)" = "Darwin"); then
  export XML_CATALOG_FILES=/usr/local/etc/xml/catalog
fi

rm -rf "$BDIR"
mkdir -p "$BDIR" &&
  cd "$BDIR"
# shellcheck disable=SC2086
cmake -S .. \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DLIBICAL_BUILD_TESTING=ON \
  -DLIBICAL_BUILD_DOCS=ON \
  -DLIBICAL_BUILD_EXAMPLES=ON \
  -DLIBICAL_CXX_BINDINGS=ON \
  -DGOBJECT_INTROSPECTION=ON \
  -DLIBICAL_GLIB_VAPI=ON \
  -DLIBICAL_GLIB_BUILD_DOCS=ON \
  $CMAKE_VERSION4_OPTIONS \
  -DLIBICAL_BUILD_TESTING_BIGFUZZ=ON \
  -DCMAKE_INSTALL_PREFIX="$HOME/tmp/libical-$BRANCH" &&
  ninja && ninja test && ninja install && ninja docs && ninja build-book && ninja uninstall

rm -rf "$HOME/tmp/libical-$BRANCH"
