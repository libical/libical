# Libical v4.0.2

## Overview

This is a patch release and is fully source and binary compatible with version 4.0.0.

## ReleaseNotes

- Fix a heap-buffer-overflow in `icalcomponent_merge_component()`.
- Remove "-Wl,-z,nodlopen" from gcc/clang compile options.
- icaltime: Prefer passed-in time's timezone in `icaltime_compare_date_only()`.
- icaltime: Check for month out-of-range in `icaltime_day_of_year()`.

Please see the [CHANGELOG](https://github.com/libical/libical/blob/4.0/CHANGELOG.md) for more.
