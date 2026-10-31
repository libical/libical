#!/bin/bash

# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

# Please refer to docs/developer/HOWTO-update-zoneinfo.md for more information
# Visit <http://www.iana.org/time-zones> to find the Time Zone Database version you want (eg. 2025c)

#Exit if any undefined variable is used.
set -u
#Exit this script if it any subprocess exits non-zero.
set -e
#If any process in a pipeline fails, the return value is a failure.
set -o pipefail

prog=$(basename "$0")
if (test $# -ne 1); then
  echo "Usage: $prog <tzdatabase-version>"
  echo "eg: $prog 2025c"
  exit 1
fi
# name of tzdata is passed on command line
tzdataVersion="$1"

TOP=$(readlink -nf "$0")
TOP=$(dirname "$TOP")
cd "$TOP/.."
TOP=$(pwd)

cd "$TOP/vzic"

# Get the tzdata file
tzdataDir="tzdata$tzdataVersion"
tzdataTarball="$tzdataDir.tar.gz"
rm -f "$tzdataTarball"*
wget "http://data.iana.org/time-zones/releases/$tzdataTarball"

# Uncompress and untar the "tarball" tzdata file
mkdir -p "$tzdataDir"
cd "$tzdataDir"
tar xfz ../"$tzdataTarball"
cd ..
rm -f "$tzdataTarball"

# Build vzic
buildDir=build-update-zoneinfo
mkdir -p $buildDir
cd $buildDir
cmake -DSTANDALONE=True ..
cmake --build .

# Run vzic and generate the new zoneinfo
./vzic --pure --olson-dir ../"$tzdataDir" --output-dir ./zoneinfo

# Run vzic-merge.pl to merge the new zoneinfo into the existing zoneinfo
cd ..
perl vzic-merge.pl --master-zoneinfo-dir="$TOP/zoneinfo" --new-zoneinfo-dir="$buildDir/zoneinfo"

# Testing (currently failing)
#cd $buildDir
#perl ../vzic-dump.pl ../"$tzdataDir"
#./vzic --dump --pure --olson-dir ../"$tzdataDir"
#diff -ruw zoneinfo/ZonesPerl zoneinfo/ZonesVzic
#diff -ruw zoneinfo/RulesPerl zoneinfo/RulesVzic

# Add the new timezones in the zones.tab file by hand
echo
echo "DONE!"
echo "The libical/zoneinfo has been updated for $tzdataDir"
echo "git add any new zoneinfo files and then git commit zoneinfo"

cd "$TOP/vzic"
rm -f "$tzdataTarball"*
rm -rf "$tzdataDir"

set +e
diff -q "$TOP/zoneinfo/zones.tab" "$TOP/vzic/$buildDir/zoneinfo/zones.tab" >/dev/null
if (test $? -eq 1); then
  echo
  echo "WARNING: The new zones.tab differs from the current zoneinfo/zones.tab."
  echo "You need to merge the new zone.tab into the current one by-hand."
  echo
  echo "Run:"
  echo " diff $TOP/zoneinfo/zones.tab" "$TOP/vzic/$buildDir/zoneinfo/zones.tab"
  echo
  echo "Copy lines from the new zones.tab that are new (or differ) into the official zoneinfo/zones.tab"
  echo "... and of course then commit the updated zoneinfo/zones.tab"
else
  rm -rf $buildDir
fi
set -e
