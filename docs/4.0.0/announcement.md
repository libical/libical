# Announcing Libical 4.0.0

Announcing Libical 4.0.0.

This is a major release and is **not** source or binary compatible with version 3.x.

Please see our [Version 4 Migration Guide](https://github.com/libical/libical/blob/4.0/docs/MigrationGuide_to_4.md)
for details about API changes since libical 3.x.

Note that the libical-glib library is considered stable as of this release.

Also included in this release is a **Technical Preview (TP)** of our new library libicalvcard
for handling VCARD formatted data.  We encourage users to try-out this new library
and provide feedback to our issue tracker at <https://github.com/libical/libical/issues>.
Note that the libicalvcard API is not finalized and no source or binary compatibility
is guaranteed until sometime later in 4.x release cycle.

Highlights of this Release:

* Too much to mention here. Please see the [CHANGELOG](https://github.com/libical/libical/blob/4.0/CHANGELOG.md).

The source code can be found on GitHub at: <https://github.com/libical/libical>

Tarballs and zipballs for v4.0.0 are available from: <https://github.com/libical/libical/releases/tag/v4.0.0>

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
