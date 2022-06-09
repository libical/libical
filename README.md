# Libical — an implementation of iCalendar protocols and data formats

[![Appveyor status](https://ci.appveyor.com/api/projects/status/github/libical/libical?branch=master?svg=true)](https://ci.appveyor.com/api/projects/status/github/libical/libical)
[![Packaging status](https://repology.org/badge/tiny-repos/libical.svg)](https://repology.org/metapackage/libical)

## About

Libical — an implementation of iCalendar protocols and data formats

Most of the code in here was written by Eric Busboom at the end
of the last millennium with help from dozens of contributors.
It is currently maintained by Allen Winter and the libical team
at <https://github.com/libical/libical>.

Libical is an Open Source implementation of the iCalendar protocols
and protocol data units. The iCalendar specification describes how
calendar clients can communicate with calendar servers so users can
store their calendar data and arrange meetings with other users.

Libical implements [RFC5545][], [RFC5546][], [RFC7529][];
the CalDav scheduling extensions in [RFC6638][];
iCalendar extensions in [RFC7986][], [RFC9073][], [RFC9074][];
plus the iCalendar iMIP protocol in [RFC6047][].

[RFC5545]: https://tools.ietf.org/html/rfc5545
[RFC5546]: https://tools.ietf.org/html/rfc5546
[RFC7529]: https://tools.ietf.org/html/rfc7529
[RFC6638]: https://tools.ietf.org/html/rfc6638
[RFC6047]: https://tools.ietf.org/html/rfc6047
[RFC7986]: https://tools.ietf.org/html/rfc7986
[RFC9073]: https://tools.ietf.org/html/rfc9073
[RFC9074]: https://tools.ietf.org/html/rfc9074

## License

The code and datafiles in this distribution are licensed under the
Mozilla Public License (MPL) v2.0. See <https://www.mozilla.org/MPL>
for a copy of this license.

Alternately, you may use libical under the terms of the GNU Lesser
General Public License (LGPL) v2.1. See <https://www.gnu.org/licenses/lgpl-2.1.txt>
for a copy of this license.

This dual license ensures that the library can be incorporated into
both proprietary code and GPL'd programs, and will benefit from improvements
made by programmers in both realms. We (the libical developers) will only
accept changes to this library if they are similarly dual-licensed.

## Building

See the top-level [Install.txt](Install.txt) file.

## Documentation

Documentation is hosted at <https://libical.github.io/libical/apidocs/>.

For a conceptual overview of the library, see [UsingLibical.md](@ref UsingLibical).
There is other rudimentary, unfinished documentation in the `doc/` directory of
the source distribution,
and annotated examples in `examples/` and the test code in `src/test/`.

Additionally, progress is underway to add API documentation,
which is available at the apidocs site.

## Acknowledgments

Portions of this distribution are (C) Copyright 1996 Apple Computer,
Inc., AT&T Corp., International Business Machines Corporation and
Siemens Rolm Communications Inc. See
[src/libicalvcal/README.TXT](src/libicalvcal/README.txt) for
details.

## Libical Users

In no particular order:

* [Cyrus Email/Calendars/Contacts Server](https://www.cyrusimap.org)
* [syncEvolution](https://syncevolution.org)
* [Fantastical](https://flexibits.com/fantastical)
* GNOME's EDS (evolution-data-server) which serves data to:
  * [Evolution](https://wiki.gnome.org/Apps/Evolution)
  * [GNOME Calendar](https://wiki.gnome.org/Apps/Calendar)
  * [GNOME Notes](https://wiki.gnome.org/Apps/Notes)
  * [GNOME Todo](https://wiki.gnome.org/Apps/Todo)
  * and more GNOME apps...
* KDE's [Kontact Suite](https://kontact.kde.org) including:
  * [Akonadi framework](https://kontact.kde.org/components/akonadi.html)
  * [KOrganizer calendar and scheduling component](https://kontact.kde.org/components/korganizer.html)
  * and more KDE apps...

## Get Involved

Subscribe to our mailing lists:

For developer discussions
  <https://lists.infradead.org/mailman/listinfo/libical-devel>

For general discussions about libical and related projects
  <https://lists.infradead.org/mailman/listinfo/libical-interest>

Report bugs to our issue tracker at
  <https://github.com/libical/libical/issues>
