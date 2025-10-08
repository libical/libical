# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [4.0.0] - Unreleased

This is a major release and is **not** source or binary compatible with version 3.x.

Please see [Version 4 Migration Guide](docs/MigrationGuide_to_4.md)
for details about API changes since libical 3.x.

### Added

- REUSE compliant licensing
- Supports multi-valued parameters (DELEGATED-FROM, DELEGATED-TO, MEMBER, DISPLAY, FEATURE, etc)
- Adds java bindings
- Brand new icalvcard library that follows the libical API.
- draft-ietf-calext-eventpub-extensions-19 (RFC 9073) support added
- draft-ietf-calext-valarm-extensions-07 (RFC 9074) support added
- Added support for Event Publishing (RFC 9073) and VALARM (RFC 9074) Extensions
- New CMake option "LIBICAL_ENABLE_64BIT_ICALTIME_T" to use 64-bit time_t implementations
   on 32-bit systems (where available and supported.  Windows-only so far)
- GEO property has arbitrary precision (values are internally stored as strings, not doubles)
- Allow previous recurrence iteration
- icalparser_ctrl setting defines how to handle invalid CONTROL characters during parsing
- Full support for `BYSETPOS`, i.e. remove the limitation to MONTHLY and YEARLY frequencies

### Changed

- Requires MSVC 2013 or higher (when building on Windows with MSVC)
- Requires CMake v3.20.0 or higher
- For the C++ bindings, requires a C++11 compliant C++ compiler
- libical-glib requires glib 2.38 or higher
- libical-glib requires a C11 compliant C compiler
- Replace gtk-doc with gi-docgen to produce libical-glib developer documentation
- libical-glib API is considered stable; no longer need to define LIBICAL_GLIB_UNSTABLE_API=1
   before including <libical-glib/libical-glib.h>
- By default, only shared libraries are built; to get static libraries one must invoke
  separate build using the CMake `-DLIBICAL_STATIC=True` option.
- CMake option -DGOBJECT_INTROSPECTION=True by default.
- `icaltzutil_get_zone_directory()` can use the TZDIR environment to find system zoneinfo
- `icaltimezone_set_tzid_prefix()` now allows setting an empty tzid prefix.
- `icaldurationtype_from_int` and `icaldurationtype_as_int` have been renamed to
    `icaldurationtype_from_seconds` and `icaldurationtype_as_seconds`, respectively.

### Deprecated

- The icalvcal library is deprecated and will be removed sometime in the 4.x series.
   Please port your icalvcal code to use icalvcard instead.

- Several CMake options are renamed in favor of a "namespaced" equivalent.
  Please port your buildscripts to the new CMake option names.
  A complete list is provided in this "CMake options" section of the
  [Version 4 Migration Guide](docs/MigrationGuide_to_4.md)

### Removed

- The old src/python code is removed in favor of the glib-introspection generated
   Python bindings. Requires building with -DGOBJECT_INTROSPECTION=True

### Fixed

- Fixed `icalcomponent_get_duration()` to account for DTEND/DUE TZID
- Improved performance of recurrence iterators
- Improved handling of BYWEEKNO when BYDAY is not present
- Fixed `icalcomponent_get_dtend()` and `icalcomponent_foreach_recurrence()`
    to correctly handle nominal durations
- Fixed `icalcomponent_foreach_recurrence` to filter out duplicate
    instances

## [3.0.21] - Unreleased

- Fix some scenarios with RRULEs and EXRULE (#754)

## [3.0.20] - 2025-05-10

- Clean up the byte-swapping macros
- Fix a testcase in libical-glib when using 64-bit on a 32-bit system
- Fix regression introduced in v3.0.19 ignoring non-empty REQUEST-STATUS extdata
- Fix regression introduced in v3.0.19 error reporting for END before BEGIN
- Fix a CMake problem in libical-glib

## [3.0.19] - 2024-12-23

- Fix for changes to the libicu 75 API
- Add vcpkg manifest-mode support
- Improved berkeley-db discovery on Mac with homebrew
- Improved libicu discrovery on Mac with homebrew
- Properly set DYLD_LIBRARY_PATH on Mac for libical-ical tests
- Resolved known limitation: Negative values are now also supported for `BYMONTHDAY` and `BYYEARDAY`.
- Add support for RDATE;VALUE=PERIOD
- Fix time conversion to time_t for times before epoch
- Allow `icalcomponent_foreach_recurrence` to receive `DATE`-only `start` and `end` params.
- Fix the calculation of an event's duration if `DTSTART` is a `DATE`-only value.
- Fix `icaltime_span_new()` - ignore the case where DTEND is unset and require it to be
  set by the caller instead.
- Various fixes for fuzzer issues

## [3.0.18] - 2024-03-24

- Requires CMake v3.5.0 or higher
- Escape ^ (U+005E) character in parameter values according to RFC 6868
- New CMake option LIBICAL_BUILD_EXAMPLES to disable building the examples
- Should be able to use the project as a submodule
- Built-in timezones updated to tzdata2024a

## [3.0.17] - 2023-10-14

- Improved Android support
- Escape commas in x-property TEXT values
- Built-in timezones updated to tzdata2023c
- icalparser_ctrl setting defines how to handle invalid CONTROL characters during parsing
- New publicly available functions:
  - get_zone_directory
  - icalparser_get_ctrl
  - icalparser_set_ctrl

## [3.0.16] - 2022-10-17

- Fix regressions in 3.0.15 due to improperly tested fuzz fixes
- Fix argument guards in icaltime_as_timet to match documentation and tests

## [3.0.15] - 2022-10-06

- Add missing property parameters into libical-glib
- Fix CMake option USE_32BIT_TIME_T actually uses a 32-bit time_t value
- Fix icaltime_as_timet, which returned incorrect results for years >= 2100,
   to work properly between years 1902 and 10k.
- Fix x-property comma handling and escaping
- Built-in timezones updated to tzdata2022d (now with a VTIMEZONE for each time zone alias)
- Fix a fuzzer issue
- Handle unreachable-code compile warnings with clang
- Ensure all vanew_foo() calls finish with (void*)0 (not 0)

## [3.0.14] - 2022-02-15

- icalvalue: Reset non-UTC icaltimetype::zone on set
- Fix icalcomponent_set_due not removing TZID when necessary

## [3.0.13] - 2022-01-17

- `icalcomponent_get_dtend()` returns `icaltime_null_time()` unless called on VEVENT, VAVAILABILITY
  or VFREEBUSY
- `icalcomponent_get_duration()` for VTODO calculate with DUE instead of DTEND
- Replace CMake FindBDB with FindBerleyDB (<https://github.com/sum01/FindBerkeleyDB>)
- Fix finding ICU and BerkeleyDB on Mac (look for homebrew installs first)

## [3.0.12] - 2021-12-08

- Fix a libicalval crash in cleanVObject
- METHOD:DECLINECOUNTER must have DTEND or DURATION
- Handle if DTEND and DURATION are both missing
- Improved FindICU (copied from official CMake.  see files in cmake/Kitware)
- Buildsystem fixes (especially for the Ninja generator)
- Built-in timezones updated to tzdata2021e

## [3.0.11] - 2021-10-09

- Fix `icalrecur_iterator_set_start()` for hourly, minutely, and secondly recurrences
- Fix build for Berkeley DB version greater than 5
- Fix vcal for some architectures (like aarch64, ppc64le and s390x)
- Fix memory leaks in vcal
- Prevent crash when looking for tzid in initialize_rscale
- Adjust libdir and includedir in generated pkgconfig files
- Built-in timezones updated to tzdata2021c

## [3.0.10] - 2021-04-17

- Fix building -DSTATIC_ONLY=True with Ninja
- Fix generating wrong recurrence rules (#478)
- Fix a bug computing transitions in tzfiles
- Fix reading TZif files to use TZ string in the footer as the last (non-terminating) transitions
- Fix reading TZif files to use more RRULEs and/or RDATEs whevever possible
- Built-in timezones updated to tzdata2021a

## [3.0.9] - 2021-01-16

- Add support for empty parameters, e.g. CN=""
- Accept VTIMEZONE with more than one X- property
- Several fixes for recurrences containing BYWEEKNO
- `icalrecurrencetype_from_string()` will reject any RRULE that contains a rule-part that
  occurs more than once
- Improve thread safety
- Fix compiled-in path for the built-in timezone data
- Fix reading TZif files with empty v1 data (use v2+ whenever possible)
- Add backwards compatibility for previous TZIDs
- Built-in timezones updated to tzdata2020d
- `icalrecurrencetype_from_string()` will reject any RRULE that contains a rule-part that
  occurs more than once
- Fix build with newer libicu
- Fix cross-compile support in libical-glib

## [3.0.8] - 2020-03-07

- Fix for `icalattach_new_from_data()` and the 'free_fn' argument
- Fix if recurrencetype contains both COUNT and UNTIL (only output UNTIL in the RRULE)

## [3.0.7] - 2019-12-15

- libical-glib: Fix ICalAttach handling of the icalattach native structure
- Let `icalattach_new_from_data()` use the 'free_fn' argument again
- Fix memory leaks in attachment handling and elsewhere
- Fix a multithreading race condition
- More fuzzification fixes

## [3.0.6] - 2019-09-14

- Handle both COUNT and UNTIL in RRULEs
- Fix RRULE BYDAY with INTERVAL=2 conflict
- Various fuzzification fixes
- New publicly available function:
  - icaltimezone_truncate_vtimezone
- Add option to disable building the test suite
- Built-in timezones updated to tzdata2019c

## [3.0.5] - 2019-05-14

- New publicly available function:
  - icalproperty_get_datetime_with_component
- Allow reset DATE/DATE-TIME VALUE parameter for all-day events
- `icalproperty_get_datetime_with_component()` will use location as TZID fallback.
- New CMake option ENABLE_GTK_DOC for disabling the libical-glib developer documentation
- GObject Introspection - use $MAJOR-0 versioning
- libical-glib API is considered unstable, define LIBICAL_GLIB_UNSTABLE_API=1 before
   including <libical-glib/libical-glib.h> to be able to use it.
- Built-in timezones updated to tzdata2019a
- De-fuzzifications and Coverity fixes

## [3.0.4] - 2018-08-14

- Silently fail RSCALE recurrence clauses when RSCALE is disabled
- Fixed `icalcomponent_set_comment()` and `icalcomponent_set_uid()`
- fix FREQ=MONTHLY;BYMONTH
- Skip UTF-8 marker when parsing
- Fix parsing "<CR>?<LF>" in VCF files produced by Outlook
- Fix TZID on DATE-TIME value can override time specified in UTC
- CMake discovery module for ICU uses pkg-config now
- New publicly available function:
  - icalparameter_kind_is_valid
- Built-in timezones updated to tzdata2018e

## [3.0.3] - 2018-02-27

- VTODO COMPLETED property can be a DATE-TIME or DATE (for backward compatibility)
- Improved recurrence iteration

## [3.0.2] - 2018-02-17

- No longer attempt to detect the need for -DUSE_32BIT_TIME_T with MSVC
- New CMake option ICAL_BUILD_DOCS which can be used to disable the docs target
- Fix threading hang in BSD type systems (OpenBSD, MacOS,...)
- Build with Ninja improvements

## [3.0.1] - 2017-11-18

- Built-in timezones updated to tzdata2017c
- Fix a multi-threaded deadlock in `icaltimezone_load_builtin_timezone()`
- Fix a CMake problem with parallel builds

## [3.0.0] - 2017-10-28

- Relicense from MPL 1.0 to MPL 2.0 (keep dual license for LGPL v2.1)
- Requires CMake v3.1.0 or higher along with various CMake and buildsystem fixes
- Added a 'make uninstall'
- Fixed use-after-free issues and some memory leaks
- Built-in timezones updated to tzdata2017b
- More accurate VTIMEZONE generation when using the system time zone data (when USE_BUILTIN_TZDATA=False)
- `icalvalue_as_ical_string()` returns "TRUE" (non-zero) or "FALSE" (zero) values only.
- New icalvalue.h convenience macros: ICAL_BOOLEAN_TRUE and ICAL_BOOLEAN_FALSE
- Better value type checking of property values when parsing
- icalvalue_new/set_date and icalvalue_new/set_datetime now enforce DATE and DATE-TIME values respectively
- draft-ietf-calext-extensions (RFC 7986) support added
- Parameter values are now en/decoded per RFC 6868
- Removed is_utc from icaltimetype struct
  - Set icaltimetype.zone to `icaltimezone_get_utc_timezone()` to change a time to UTC
  - Use `icaltime_is_utc()` to check if a time is in UTC
- Added support for VPATCH component
- New publicly available functions:
  - icalproperty_set_parent (icalproperty_get_parent was already public)
  - icalvalue_get_parent (icalvalue_set_parent was already public)
  - icalparameter_set_parent
  - icalparameter_get_parent
  - icalvalue_new_datetimedate (DATE or DATE-TIME)
  - icalvalue_set_datetimedate
  - icalvalue_get_datetimedate
  - icalrecur_iterator_set_start
  - icalcomponent_normalize
  - icalproperty_normalize
- Removed deprecated functions:
  - icaltime_from_timet (use icaltime_from_timet_with_zone)
  - icaltime_start_day_of_week (use icaltime_start_day_week)
  - icalproperty_remove_parameter (use icalproperty_remove_parameter_by_kind)
  - icalproperty_string_to_enum (use icalproperty_kind_and_string_to_enum)
- Signature changed for functions:
  - Parse_MIME_FromFileName
  - icalgauge *icalgauge_new_from_sql
  - const char *icallangbind_property_eval_string
  - const char *icallangbind_property_eval_string_r
  - void set_zone_directory
  - icalcalendar *icalcalendar_new
  - int icalrecur_expand_recurrence

## [2.0.0] - 2015-12-28

- WARNING: Version 2 IS NOT Binary Compatible with Older Versions
- Version 2 is Source Compatible with Older Versions
- Lots of source code scrubbing
- [New] RSCALE support (requires libicu from <http://www.icu-project.org>)
- [New] CalDAV attachment support (draft-ietf-calext-caldav-attachments)
- [New] Resurrect the Berkeley DB storage support
- [Bug] issue83: Incorrect recurrence generation for weekly pattern
- Handle RRULEs better
- Handle threading better

## [1.0.1] - 2014-10-11

- [Bug] issue74: Do not escape double quote character
- [Bug] issue80,issue92: fix crashes using `icaltimezone_load_builtin_timezone()` recursively
- [Bug] Fix `icalcomponent_foreach_recurrence()` and large durations between recurrences (e.g. FREQ=YEARLY)
- [Bug] Properly handle UTCOFFSETs of the form +/-00mmss
- [Bug] Properly skip bogus dates (e.g. 2/30, 4/31) in RRULE:FREQ=MONTHLY
- [Bug] Properly handle RRULE:FREQ=MONTHLY;BYDAY;BYMONTHDAY when DTSTART isn't on BYDAY
- [Bug] Fix RRULE:FREQ=YEARLY;BYDAY;BYWEEKNO - MUST use ISO weeks
<!-- markdownlint-disable MD052 -->
- [Bug] Properly skip bogus dates (e.g. 2/29) in RRULE:FREQ=YEARLY[;BYMONTH][;BYMONTHDAY]

- [New] Update tzdata to version 2014g
- [New] Support added for schedule params: agent, status, force-send
- [New] Added a UID to the VFREEBUSY component
- [New] Allow dates > 2038 if sizeof(time_t) > 4
- [New] Add properties from draft-ietf-tzdist-service
- [New] Add support for RRULE:FREQ=YEARLY;BYDAY;BYYEARDAY
- [New] Fixed RRULE:FREQ=YEARLY;BYYEARDAY for negative days

- [Build] Autotools build system is removed
- [Build] CMake version 2.8.9 (or higher) is required (was CMake version 2.4.0)
- [Build] Add new -DSHARED_ONLY and -DSTATIC_ONLY CMake options
- [Build] Remove -DSTATIC_LIBRARY CMake option
- [Build] MSYS2 builds (fixed instructions)
- [Build/Doc] Now can build api documentation with make docs

- [Quality] More regression tests added, in particular for recurrence
- [Quality] Almost all compile warnings silenced
- [Quality] A bunch of Coverity Scan warnings silenced

## [1.0.0] - 2014-06-28

- Reborn
