# Libical v4.0.1

## Overview

This is a patch release and is fully source and binary compatible with version 4.0.0.

## ReleaseNotes

* Increase the ICAL_LIMIT_RRULE_SEARCH limit from 100 to 500. ICAL_LIMIT_RRULE_SEARCH is the
  maximum times to search vtimezone rrules for an occurrence before the specified end year.
* Fix an unsafe exec() in the `vzic` tool when compiled with the CREATE_SYMLINK option.

Please see the [CHANGELOG](https://github.com/libical/libical/blob/4.0/CHANGELOG.md) for more.
