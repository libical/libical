# Announcing Libical 4.0.1

Announcing Libical 4.0.1.

Version 4.0.1 is a patch release.
This release is binary and source compatible with version 4.0.0.

Highlights of this Release:

* Increase the ICAL_LIMIT_RRULE_SEARCH limit from 100 to 500. ICAL_LIMIT_RRULE_SEARCH is the
  maximum times to search vtimezone rrules for an occurrence before the specified end year.
* Fix an unsafe exec() in the `vzic` tool when compiled with the CREATE_SYMLINK option.

The source code can be found on GitHub at: <https://github.com/libical/libical>

Tarballs and zipballs for v4.0.1 are available from: <https://github.com/libical/libical/releases/tag/v4.0.1>

"Libical is an Open Source implementation of the iCalendar protocols and protocol data units.
The iCalendar specification describes how calendar clients can communicate with calendar servers
so users can store their calendar data and arrange meetings with other users."

Libical implements
(see [RFC calendar standards support](https://github.com/libical/libical/blob/4.0/docs/rfcs.md)):

* RFC5545, RFC5546, RFC7529
* New Properties for iCalendar (RFC7986)
* Event Publishing Extensions to iCalendar (RFC9073)
* VALARM Extensions for iCalendar (RFC9074)
* Support for iCalendar Relationships (RFC9253)
* iCalendar Message-Based Interoperability Protocol (iMIP) (RFC6047)
* Scheduling Extensions to CalDAV (RFC6638)

For more information about Libical, please visit <http://libical.github.io/libical/book>
