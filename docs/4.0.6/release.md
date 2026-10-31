# Libical v4.0.6

## Overview

This is a patch release and is fully source and binary compatible with version 4.0.0.

[Note: the libicalvcard is a released as a *Technical Preview*. As such,
its API is not finalized and no source or binary compatibility is guaranteed
with that library in the 4.0.x series.]

## ReleaseNotes

- CVE: Fix UBSAN issue "bsearch comparator through an incompatible function pointer"
- RFC5545 section 3.3.5 Date-Time fixes:
  - Resolve local times in a fall-back overlap to the first occurrence
  - Fix explicit local time in a DST gap resolves using the post-transition offset
- RFC5545 section 3.3.10 Recurrence Rule fixes:
  - FREQ=WEEKLY with BYMONTH and BYSETPOS loses occurrences in the week straddling month boundary
- libicalvcard: the typed vcardvalue getters now check that the value type of the property
  uses the same union member as the expected value type of the getter. If the type mismatches,
  then they return the zero value for the expected value type. The typed setters error with
  ICAL_BADARG_ERROR instead of overwriting a value held in another union member.
- libicalvcal: Fixed a memory leak in vobject.c
- Buildsystem changes:
  - Fix build paths in installed cmake files (esp. for cross-compiling)
  - Install the libicalversion.h header
  - Fix building libical as a submodule in a larger CMake project
  - Fix "now" linker support check in the hardening compiler options
  - Fix the DESTINATION path for libical.jar
  - Use private linkage for 3rdparty libraries (icu, glib, ..)
- Built-in timezones updated to tzdata2026e.
- Remove ancient hack (since 2012) to support malformed TZID parameters
- Build and test validated with clang 23.1.1
- Build and test validated with gcc (GCC) 17.0.0 20260920 (experimental)

Please see the [CHANGELOG](https://github.com/libical/libical/blob/4.0/CHANGELOG.md) for more.
