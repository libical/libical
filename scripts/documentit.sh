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
  echo "Usage: $(basename "$0")"
  exit 1
}

TOP=$(readlink -nf "$0")
TOP=$(dirname "$TOP")
cd "$TOP/.."
TOP=$(pwd)
BRANCH=$(git branch --show-current | awk -F/ '{print $NF}')
BDIR="$TOP/build-apidox"
INSTALLDIR="$HOME/tmp/libical-$BRANCH"

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
  -DLIBICAL_BUILD_TESTING=OFF \
  -DLIBICAL_BUILD_DOCS=ON \
  -DLIBICAL_BUILD_EXAMPLES=ON \
  -DLIBICAL_CXX_BINDINGS=OFF \
  -DLIBICAL_JAVA_BINDINGS=OFF \
  -DLIBICAL_GOBJECT_INTROSPECTION=OFF \
  -DLIBICAL_GLIB_VAPI=OFF \
  -DLIBICAL_GLIB_BUILD_DOCS=OFF \
  -DLIBICAL_BUILD_TESTING_BIGFUZZ=OFF \
  -DCMAKE_INSTALL_PREFIX="$INSTALLDIR" &&
  ninja --verbose &&
  ninja docs &&
  ninja build-book
