# Announcing Libical 4.0.8

Announcing Libical 4.0.8

Version 4.0.8 is a patch release.
This release is binary and source compatible with version 4.0.0.

Version 4.0.8 will likely be the last release in the 4.0.x series.
Version 4.1.0 is on our planning board for the next release.

[Note: the libicalvcard is a released as a *Technical Preview*. As such,
its API is not finalized and no source or binary compatibility is guaranteed
with that library in the 4.0.x series.]

Highlights of this Release:

- Built-in timezones updated to tzdata2026f.

- libicalvcard:
  - BDAY, ANNIVERSARY and DEATHDATE with a timestamp value, such as `19531015T231000Z`, were written
    with `VALUE=TIMESTAMP`. RFC 6350 does not allow TIMESTAMP for these properties, but a TIMESTAMP
    is a valid DATE-TIME value. The VALUE parameter is now omitted.
  - Various fixes for fuzzer issues (OOM and cpu time).

- libicalvcal:
  - Fix an out-of-bounds write when decoding a BASE64 value whose padding contains more than
    two `=` characters. The decoder computed the output length as `3 - pad`, which wrapped
    to a huge value when `pad` exceeded 3 and was then used as the size of a `memcpy`.
  - Reject a vCalendar RRULE/EXRULE interval of 0 or one that does not fit in a short.
    Such an interval was converted into an iCalendar RRULE that never advances, so expanding it
    (e.g. with `icalcomponent_foreach_recurrence()`) looped forever.
  - Fix an out-of-bounds read in the lexer. When a value folded across lines put a newline at the
    last slot of the lookahead ring buffer, `lexLookahead()` peeked at the next character with
    `lexBuf.buf[lexBuf.getPtr+1]` without wrapping the index, reading one element
    past the buffer. The sibling carriage-return branch already wrapped with `% MAX_LEX_LOOKAHEAD`.
  - Fix an endless loop with unbounded memory growth when parsing a QUOTED-PRINTABLE value with
    an `=` that is not followed by two hex digits, or a BASE64 or QUOTED-PRINTABLE value that runs
    to the end of the input. A bare `=` is now kept as is.

- libical:
  - A UID can no longer contain non-printable characters.
  - Fix unbounded recursion when parsing too many (100 or more) nested components
  - Reject a timezone location containing a `..` path component or an absolute path when resolving
    a VTIMEZONE TZID to a file under the zone directory (builds that read on-disk zoneinfo rather
    than the builtin data). A crafted TZID such as `../../some/file` could otherwise make
    `icaltimezone_fetch_timezone()` open a file outside the configured zone directory.
  - Drop CR and LF when writing values that are output without escaping (URI, CAL-ADDRESS, ATTACH URLs
    and enum x-values). A vCal quoted-printable value could decode to a line break that
    was written out verbatim, ending the content line early and injecting another property.
  - In builds with ICU, recurrences with a DAILY or longer frequency no longer move to a later time
    of day after an occurrence or DTSTART falls in a DST gap. For example, a weekly 02:30 Europe/Berlin
    event returned 03:30 for the week after the spring change too.
  - Fix `BYWEEKNO` with `BYDAY` at the ends of the week-numbering year. The last days of the last
    week were dropped when week 1 starts after January 1, days of the next year's week 1 were added
    (e.g. a 53rd week in a 52-week year), and with a WKST other than Monday the number of weeks in the
    year, and so negative `BYWEEKNO` values, followed ISO weeks.
  - Fix `RSCALE SKIP:` dates moved into another month are dropped when BYDAY is present. Also covers
    dates moved backward across a month boundary.
  - Fix heap-buffer-overflow read in `icalrecur_iterator_prev()` after the iteration of an HOURLY,
    MINUTELY or SECONDLY rule with a matching BYHOUR, BYMINUTE or BYSECOND reached the last supported
    year, which left the BY index past the end of the BY data.
  - `icalrecur_iterator_new()` rejects a SECONDLY, MINUTELY or HOURLY rule with a DATE DTSTART.
    Such a rule never advanced past DTSTART, so `icalcomponent_foreach_recurrence()` looped
    forever on e.g. `DTSTART;VALUE=DATE:20260101` with `RRULE:FREQ=HOURLY`, and
    `icalrecur_iterator_prev()` hung.
  - `FREQ=WEEKLY` recurrences with `INTERVAL` greater than 1 and a WKST other than Sunday (including
    the default Monday) now count weeks from the week that contains DTSTART when DTSTART is not the
    first BYDAY day of its week. For example, `FREQ=WEEKLY;INTERVAL=2;BYDAY=SU` with a Tuesday DTSTART
    used to skip the first Sunday and put every occurrence in the other weeks.
  - Various fixes for fuzzer issues (OOM and cputime).

- Buildsystem changes:
  - Generate export headers with CMake, fixing Windows static linking without requiring consumers
    to define `LIBICAL_*_STATIC_DEFINE`.
  - Give ical_cxx and ical_glib and icalss_cxx their own export macros to distinguish their symbols
    from those imported from the underlying libraries.

The source code can be found on GitHub at: <https://github.com/libical/libical>

Tarballs and zipballs for v4.0.8 are available from: <https://github.com/libical/libical/releases/tag/v4.0.8>

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
