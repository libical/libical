# Announcing Libical 4.0.5

Announcing Libical 4.0.5.

Version 4.0.5 is a patch release.
This release is binary and source compatible with version 4.0.0.

[Note: the libicalvcard is a released as a *Technical Preview*. As such,
its API is not finalized and no source or binary compatibility is guaranteed
with that library in the 4.0.x series.]

Highlights of this Release:

- Fix 32-bit time_t upper bound off by two days.
- Buildsystem: fix libical_deprecated_option() writing the wrong cache value
- Bogus TZIDs no longer cause an error.
- DTEND, DURATION, DUE property types and values are now validated per RFC 5545 Sections 3.8.2.2 - 3.8.2.5.
- Built-in timezones updated to tzdata2026c.

The source code can be found on GitHub at: <https://github.com/libical/libical>

Tarballs and zipballs for v4.0.5 are available from: <https://github.com/libical/libical/releases/tag/v4.0.5>

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
