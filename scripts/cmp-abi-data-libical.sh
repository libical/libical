#!/bin/bash

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

# requires abi-compliance-checker from http://lvc.github.io/abi-compliance-checker
# compare abi-dumper data from 2 releases

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

# generate the dumps for the X.0.0 tag
# generate the dumps for the current branch
# compare them

TOP=$(readlink -nf "$0")
TOP=$(dirname "$TOP")
cd "$TOP/.."
TOP=$(pwd)

#reference version
STABLE_VERS_MAJ=4
STABLE_VERS=$STABLE_VERS_MAJ.0.0
STABLE_TAG="v$STABLE_VERS"
#ensure there is a tag for the reference version
cd "$TOP"
if (test -z "$(git tag -l | grep "$STABLE_TAG")"); then
  echo "Sorry, there is no tag $STABLE_TAG yet for the reference version"
  exit 1
fi

#top-level for the dumps
STORE_PATH="$TOP/abi-dumps"

#list of libraries to compare (review whenever a new major release happens)
libs=" \
libical \
libical_cxx \
libicalss \
libicalss_cxx \
libicalvcal \
libicalvcard \
libical-glib \
"

#cmake options should be identical for both builds
CMAKE_OPTIONS="-G Ninja -DLIBICAL_DEVMODE_ABI_DUMPER=true -DCMAKE_BUILD_TYPE=Debug -DLIBICAL_CXX_BINDINGS=true -DLIBICAL_GOBJECT_INTROSPECTION=true -DLIBICAL_GLIB=true -DLIBICAL_JAVA_BINDINGS=false"

# First configure the build for the current branch
builddir=build-abi-gcc
cd "$TOP"
rm -rf $builddir
mkdir $builddir
cd $builddir
# shellcheck disable=SC2086
cmake .. $CMAKE_OPTIONS

# Compute the library version for the current branch
VERS=$(grep CMAKE_PROJECT_VERSION: CMakeCache.txt | cut -d= -f2)
if (test -z "$VERS"); then
  echo "Unable to compute the project version by looking in CMakeCache.txt. Something went wrong."
  rm -rf $builddir
  exit 1
fi
VERS_MAJ="$(echo "$VERS" | cut -d. -f1)"
if (test "$VERS_MAJ" != "$STABLE_VERS_MAJ"); then
  echo "Sorry, this script currently supports version $STABLE_VERS_MAJ only. The computed current version is $VERS."
  rm -rf $builddir
  exit 1
fi

# Compile the current branch
ninja

# ABI dump for each library of the current branch
DUMP_PATH="$STORE_PATH/$VERS"
mkdir -p "$DUMP_PATH"
for l in $libs; do
  abi-dumper "lib/$l.so.$VERS" -o "$DUMP_PATH/$l-$VERS.dump" -lver "$VERS"
done

# Checkout the reference branch
git checkout $STABLE_TAG

# Now configure and build the reference branch
cd "$TOP"
rm -rf $builddir
mkdir $builddir
cd $builddir
# shellcheck disable=SC2086
cmake .. $CMAKE_OPTIONS
ninja

# ABI dump for each library of the reference
DUMP_PATH="$STORE_PATH/$STABLE_VERS"
mkdir -p "$DUMP_PATH"
for l in $libs; do
  abi-dumper "lib/$l.so.$STABLE_VERS" -o "$DUMP_PATH/$l-$STABLE_VERS.dump" -lver "$STABLE_VERS"
done
rm -rf $builddir

cd "$STORE_PATH"
for l in $libs; do
  abi-compliance-checker -l "$l" \
    -old "$STABLE_VERS/$l-$STABLE_VERS.dump" \
    -new "$VERS/$l-$VERS.dump"
done
