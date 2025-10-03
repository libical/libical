/*======================================================================
 FILE: icalversion.h
 CREATOR: eric 20 March 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifndef ICAL_VERSION_H
#define ICAL_VERSION_H

#define ICAL_PACKAGE "@CMAKE_PROJECT_NAME@"
#define ICAL_VERSION "@PROJECT_VERSION@"

#define ICAL_MAJOR_VERSION (@CMAKE_PROJECT_VERSION_MAJOR@)
#define ICAL_MINOR_VERSION (@CMAKE_PROJECT_VERSION_MINOR@)
#define ICAL_PATCH_VERSION (@CMAKE_PROJECT_VERSION_PATCH@)
#define ICAL_MICRO_VERSION ICAL_PATCH_VERSION

/**
 * ICAL_CHECK_VERSION:
 * @param major: major version (e.g. 1 for version 1.2.5)
 * @param minor: minor version (e.g. 2 for version 1.2.5)
 * @param micro: micro version (e.g. 5 for version 1.2.5)
 *
 * @return TRUE if the version of the LIBICAL header files
 * is the same as or newer than the passed-in version.
 */
#define ICAL_CHECK_VERSION(major,minor,micro)                          \
    (ICAL_MAJOR_VERSION > (major) ||                                   \
    (ICAL_MAJOR_VERSION == (major) && ICAL_MINOR_VERSION > (minor)) || \
    (ICAL_MAJOR_VERSION == (major) && ICAL_MINOR_VERSION == (minor) && \
    ICAL_MICRO_VERSION >= (micro)))

#endif
