# Announcing Libical 4.0.2

Announcing Libical 4.0.2.

Version 4.0.2 is a patch release.
This release is binary and source compatible with version 4.0.0.

Highlights of this Release:

- Fix a heap-buffer-overflow in `icalcomponent_merge_component()`.
- Remove "-Wl,-z,nodlopen" from gcc/clang compile options.
- icaltime: Prefer passed-in time's timezone in `icaltime_compare_date_only()`.
- icaltime: Check for month out-of-range in 1icaltime_day_of_year()1.

The source code can be found on GitHub at: <https://github.com/libical/libical>

Tarballs and zipballs for v4.0.2 are available from: <https://github.com/libical/libical/releases/tag/v4.0.2>

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
