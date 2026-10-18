# Announcing Libical 4.0.7

Announcing Libical 4.0.7.

Version 4.0.7 is a patch release.
This release is binary and source compatible with version 4.0.0.

[Note: the libicalvcard is a released as a *Technical Preview*. As such,
its API is not finalized and no source or binary compatibility is guaranteed
with that library in the 4.0.x series.]

Highlights of this Release:

- libicalvcard:
  - `vcardcomponent_clone()` and friends now deep-copy TEXTLIST values (e.g. NICKNAME, CATEGORIES)
    and structured parameters (JSCOMPS), and keep the property group. Previously the clone shared
    them with the original, so freeing both freed the same memory twice.
  - Fixed a use-after-free and a memory leak when converting a vCalendar procedure alarm (PALARM)
    whose attachment is a plain path name.
- libical:
  - Fix busy status of recurring events in `icalcomponent_foreach_recurrence()`.
  - Fix `icalvalue_decode_ical_string()` and `icallangbind_property_eval_string()` reading past
    the end of a string that ends in a backslash or a newline.
  - `icaltimezone_truncate_vtimezone()` no longer reads the RRULE after freeing it when the new
     DTSTART is within a year of UNTIL (RSCALE rules), and no longer uses an uninitialized onset
     when no RRULE occurrence falls before the window close.
  - Fix IMAGE and STRUCTURED-DATA given by URI (with or without VALUE=URI) parsing to a URI value,
    whose string `icalproperty_get_image()` and `icalproperty_get_structureddata()` then read as
    an icalattach pointer.
  - Fix use-after-free and double free in `icalcomponent_merge_component()` when both calendars
    have a VTIMEZONE with the same TZID. Every VTIMEZONE of the merged calendar is now merged,
    and a conflicting VTIMEZONE is added under a renamed TZID (e.g. "Zone1") that its events use.
  - Fix out-of-bounds access to the year days bitmask in the recurrence iterator when an RSCALE
    date falls outside the year (e.g. BYWEEKNO with a DTSTART before the calendar's epoch).
  - Fix `icalrecur_iterator_set_range()` for a reverse range on a rule without UNTIL, which
    started the iteration at year 0 instead of at 'from'.
  - Fix `icalparser_clean()` for proper reuse by resetting internal structure values and clearing
    the root component to avoid a use-after-free.
  - Behavior change: The typed icalvalue getters now check that the value type of the icalvalue
    uses the same union member as the expected value type of the getter, as the vcardvalue getters
    have done since version 4.0.6. If the type mismatches, then they return the zero value for the
    expected value type. The typed setters error with ICAL_BADARG_ERROR instead of overwriting a
    value held in another union member.
- libicalss:
  - Fix heap-buffer-overflow in `icalspanlist_as_freebusy_matrix()` for busy spans that start
    before or end after the spanlist. `icalspanlist_from_vfreebusy()` now sets the spanlist
    start/end from DTSTART/DTEND and marks FBTYPE=FREE periods as free (all others as busy).
- Buildsystem changes:
  - Fix CMake command target_link_libraries() to use all-keywords as required since CMake v4.4.4.
  - When cross-compiling, don't build/install ical-glib-src-generator w/o existing binary

The source code can be found on GitHub at: <https://github.com/libical/libical>

Tarballs and zipballs for v4.0.7 are available from: <https://github.com/libical/libical/releases/tag/v4.0.7>

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
