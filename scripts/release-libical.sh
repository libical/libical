#!/bin/bash

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

# Please refer to docs/developer/prelease-source.md for more information.

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
#set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

PROJECT=libical

TOP=$(readlink -nf "$0")
TOP=$(dirname "$TOP")
cd "$TOP/.." || exit 1
TOP=$(pwd)

#function HELP
# print a help message and exit
HELP() {
  echo
  echo "Usage: $(basename "$0") [-f] X.Y.Z"
  echo
  echo "Create the tars/zips for libical release X.Y.Z"
  echo " Options:"
  echo "  -f  Force everything to run, even if the tag for X.Y.Z has already been pushed."
  echo
  exit 0
}

#compare 2 version strings
verlte() {
  printf '%s\n' "$1" "$2" | sort -C -V
}

options=$(getopt -o "hf" --long "help,force" -- "$@")
eval set -- "$options"
force=0
while true; do
  case "$1" in
  -h | --help)
    HELP
    ;;
  -f | --force)
    force=1
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

if (test $# -ne 1); then
  HELP
fi

#compute X(major), Y(minor), Z(patchlevel)
X=$(echo "$1" | cut -d. -f1)
Y=$(echo "$1" | cut -d. -f2)
Z=$(echo "$1" | cut -d. -f3)

#set the branch and tag
branch=$X.$Y
tag=v$branch.$Z
release=$X.$Y.$Z

cd "$TOP" || exit 1
tbranch=$(sed -e 's,.*/,,' "$TOP/.git/HEAD")
if (test "$tbranch" != "$branch"); then
  echo "please git checkout $branch first"
  exit
fi

# Doxygen
if ! command -v doxygen &>/dev/null; then
  echo "doxygen is not installed or not in your PATH. please fix."
  exit 1
fi

#require a minimum doxygen version
minDoxyVersion="1.12.0"
export PATH=/usr/local/opt/doxygen-$minDoxyVersion/bin:$PATH
doxyVersion=$(doxygen -version | awk '{print $1}')
if ! verlte "$minDoxyVersion" "$doxyVersion"; then
  echo "please install doxygen version $minDoxyVersion or higher"
  exit 1
fi

# Update doxyfile
echo -n "Ensuring Doxyfile.cmake is up-to-date: "
doxygen -u docs/Doxyfile.cmake >/dev/null 2>&1
diff docs/Doxyfile.cmake docs/Doxyfile.cmake.bak >/dev/null 2>&1
if (test $? -ne 0); then
  echo "Doxyfile.cmake has been updated by 'doxygen -u'. Please deal with this first"
  exit 1
else
  echo "OK"
  rm -f docs/Doxyfile.cmake.bak
fi

if (test "$(git tag -l | grep -c "$tag$")" -ne 1); then
  echo "please create the git tag $tag first:"
  echo "git tag -m \"$release\" $tag"
  exit
fi

if (test $force -eq 0 -a "$(git ls-remote --tags origin | grep -c "refs/tags/$tag$")" -gt 0); then
  echo "The tag for $tag has already been pushed."
  echo "Change the release number you provided on the command line."
  echo 'Or, if you simply want to re-create the tar and zips use the "-f" option.'
  exit
fi

#Create the API documentation
rm -rf build-docs "$PROJECT-$release-doc.zip"
mkdir build-docs
cd build-docs || exit 1
cmake -G "Ninja" --warn-uninitialized -Werror=dev -DLIBICAL_BUILD_DOCS=True -DLIBICAL_JAVA_BINDINGS=False ..
ninja docs
cp libical.tag "$TOP"
cd apidocs/html || exit 1
7z a "$TOP/$PROJECT-$release-doc.zip" .
cd "$TOP" || exit 1
rm -rf build-docs

git archive --format=tar --prefix="$PROJECT-$release/" "$tag" | gzip >"$PROJECT-$release.tar.gz"
git archive --format=zip --prefix="$PROJECT-$release/" "$tag" >"$PROJECT-$release.zip"

# final cleaning
#anything to clean?

# sanity
files="\
$PROJECT-$release.tar.gz \
$PROJECT-$release.zip \
$PROJECT-$release-doc.zip\
"
for f in $files; do
  ls -l "$f"
done
