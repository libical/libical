#!/bin/bash

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

#update coverity scan at https://scan.coverity.com/builds?project=libical%2Flibical
#download/untar the latest cov-analysis-linux64-<version> from https://scan.coverity.com/download?tab=cxx
#you'll need to update PATH according for the cov-analysis-linux64-<version> bin dir

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

COVVERS=2024.12.1
ME="winter@kde.org"

if (test ! -x "$HOME/projects/cov-analysis-linux64-$COVVERS/bin"); then
  echo "Sorry, cannot locate your coverity analysis tools"
  echo "Looking for $HOME/projects/cov-analysis-linux64-$COVVERS"
  exit 1
fi

PROG=$(readlink -nf "$0")
DIR=$(dirname "$PROG")
TOP=$(dirname "$DIR")
cd "$TOP" || exit 1

builddir=build-coverity_scan-gcc
rm -rf $builddir
mkdir $builddir
cd "$builddir" || exit 1
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DLIBICAL_GOBJECT_INTROSPECTION=False -DLIBICAL_GLIB=False -DLIBICAL_GLIB_BUILD_DOCS=False -DLIBICAL_ENABLE_BUILTIN_TZDATA=true -DLIBICAL_JAVA_BINDINGS=False ..
PATH=$HOME/projects/cov-analysis-linux64-$COVVERS/bin:$PATH cov-build --dir cov-int ninja || exit 1

set +e
declare -i errors
errors=$(grep -c '\[ERROR\]' "$TOP/$builddir/cov-int/build-log.txt")
if (test $errors -gt 0); then
  echo "errors detected. Exiting.."
  echo "A few errors are:"
  grep '\[ERROR\]' "$TOP/$builddir/cov-int/build-log.txt" | head -5
  exit 1
fi
set -e

tar czvf libical.tgz cov-int || exit 1

packageVersion=$(grep 'set[[:space:]]*([[:space:]]*PACKAGE_VERSION.*"[[:digit:]]' LibIcalConfigVersion.cmake | cut -d\" -f2)
curl --form token=j3ZIRWijLkYWhRzEuJEVgg \
  --form email=$ME \
  --form file=@libical.tgz \
  --form version="$packageVersion" \
  --form description="$packageVersion release" \
  --progress-bar \
  https://scan.coverity.com/builds?project=libical%2Flibical
