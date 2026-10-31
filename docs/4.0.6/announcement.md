# Announcing Libical 4.0.6

Announcing Libical 4.0.6.

Version 4.0.6 is a patch release.
This release is binary and source compatible with version 4.0.0.

[Note: the libicalvcard is a released as a *Technical Preview*. As such,
its API is not finalized and no source or binary compatibility is guaranteed
with that library in the 4.0.x series.]

Highlights of this Release:

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

The source code can be found on GitHub at: <https://github.com/libical/libical>

Tarballs and zipballs for v4.0.6 are available from: <https://github.com/libical/libical/releases/tag/v4.0.6>

"Libical is an Open Source implementation of the iCalendar protocols and protocol data units.
The iCalendar specification describes how calendar clients can communicate with calendar servers
so users can store their calendar data and arrange meetings with other users."

Libical implements
(see [RFC calendar standards support](https://github.com/libical/libical/blob/4.0/docs/rfcs.md)):

- RFC5545, RFC5546, RFC7529
- New Properties for iCalendar (RFC7986)
- Event Publishing Extensions to iCalendar (RFC9073)
- VALARM Extensions for iCalendar (RFC9074)
- Support for iCalendar Relationships (RFC9253)
- iCalendar Message-Based Interoperability Protocol (iMIP) (RFC6047)
- Scheduling Extensions to CalDAV (RFC6638)

For more information about Libical, please visit <http://libical.github.io/libical/book>
