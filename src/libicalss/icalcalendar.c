/*======================================================================
 FILE: icalcalendar.c
 CREATOR: eric 23 December 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalcalendar.h"
#include "icaldirset.h"
#include "icalfileset.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BOOKED_DIR "booked"
#define INCOMING_FILE "incoming.ics"
#define PROP_FILE "properties.ics"
#define FBLIST_FILE "freebusy.ics"

struct icalcalendar_impl {
    char *dir;
    icalset *freebusy;
    icalset *properties;
    icalset *booked;
    icalset *incoming;
};

struct icalcalendar_impl *icalcalendar_new_impl(void)
{
    struct icalcalendar_impl *impl;

    if ((impl = (struct icalcalendar_impl *)malloc(sizeof(struct icalcalendar_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    return impl;
}

static icalerrorenum icalcalendar_create(const struct icalcalendar_impl *impl)
{
    char path[MAXPATHLEN];
    struct stat sbuf;
    int r;

    icalerror_check_arg_re((impl != 0), "impl", ICAL_BADARG_ERROR);

    path[0] = '\0';
    strncpy(path, impl->dir, MAXPATHLEN - 1);
    strncat(path, "/", MAXPATHLEN - strlen(path) - 1);
    strncat(path, BOOKED_DIR, MAXPATHLEN - strlen(path) - 1);
    path[MAXPATHLEN - 1] = '\0';

    /* coverity[fs_check_call] */
    r = stat(path, &sbuf);

    if (r != 0 && errno == ENOENT) {
        if (mkdir(path, 0777) != 0) {
            icalerror_set_errno(ICAL_FILE_ERROR);
            return ICAL_FILE_ERROR;
        }
    }

    return ICAL_NO_ERROR;
}

icalcalendar *icalcalendar_new(const char *dir)
{
    struct icalcalendar_impl *impl;

    icalerror_check_arg_rz((dir != 0), "dir");

    impl = icalcalendar_new_impl();

    if (impl == 0) {
        return 0;
    }

    impl->dir = (char *)strdup(dir);
    impl->freebusy = 0;
    impl->properties = 0;
    impl->booked = 0;
    impl->incoming = 0;

    if (icalcalendar_create(impl) != ICAL_NO_ERROR) {
        free(impl->dir);
        free(impl);
        return 0;
    }

    return impl;
}

void icalcalendar_free(icalcalendar *calendar)
{
    if (calendar->dir != 0) {
        free(calendar->dir);
    }

    if (calendar->freebusy != 0) {
        icalset_free(calendar->freebusy);
    }

    if (calendar->properties != 0) {
        icalset_free(calendar->properties);
    }

    if (calendar->booked != 0) {
        icalset_free(calendar->booked);
    }

    if (calendar->incoming != 0) {
        icalset_free(calendar->incoming);
    }

    calendar->dir = 0;
    calendar->freebusy = 0;
    calendar->properties = 0;
    calendar->booked = 0;
    calendar->incoming = 0;

    free(calendar);
}

int icalcalendar_lock(const icalcalendar *calendar)
{
    icalerror_check_arg_rz((calendar != 0), "calendar");
    return 0;
}

int icalcalendar_unlock(const icalcalendar *calendar)
{
    icalerror_check_arg_rz((calendar != 0), "calendar");
    return 0;
}

int icalcalendar_islocked(const icalcalendar *calendar)
{
    icalerror_check_arg_rz((calendar != 0), "calendar");
    return 0;
}

int icalcalendar_ownlock(const icalcalendar *calendar)
{
    icalerror_check_arg_rz((calendar != 0), "calendar");
    return 0;
}

icalset *icalcalendar_get_booked(icalcalendar *calendar)
{
    char dir[MAXPATHLEN];

    icalerror_check_arg_rz((calendar != 0), "calendar");

    dir[0] = '\0';
    strncpy(dir, calendar->dir, MAXPATHLEN - 1);
    strncat(dir, "/", MAXPATHLEN - strlen(dir) - 1);
    strncat(dir, BOOKED_DIR, MAXPATHLEN - strlen(dir) - 1);
    dir[MAXPATHLEN - 1] = '\0';

    if (calendar->booked == 0) {
        icalerror_clear_errno();
        calendar->booked = icaldirset_new(dir);
        assert(icalerrno == ICAL_NO_ERROR);
    }

    return calendar->booked;
}

icalset *icalcalendar_get_incoming(icalcalendar *calendar)
{
    char path[MAXPATHLEN];

    icalerror_check_arg_rz((calendar != 0), "calendar");

    path[0] = '\0';
    strncpy(path, calendar->dir, MAXPATHLEN - 1);
    strncat(path, "/", MAXPATHLEN - strlen(path) - 1);
    strncat(path, INCOMING_FILE, MAXPATHLEN - strlen(path) - 1);
    path[MAXPATHLEN - 1] = '\0';

    if (calendar->properties == 0) {
        calendar->properties = icalfileset_new(path);
    }

    return calendar->properties;
}

icalset *icalcalendar_get_properties(icalcalendar *calendar)
{
    char path[MAXPATHLEN];

    icalerror_check_arg_rz((calendar != 0), "calendar");

    path[0] = '\0';
    strncpy(path, calendar->dir, MAXPATHLEN - 1);
    strncat(path, "/", MAXPATHLEN - strlen(path) - 1);
    strncat(path, PROP_FILE, MAXPATHLEN - strlen(path) - 1);
    path[MAXPATHLEN - 1] = '\0';

    if (calendar->properties == 0) {
        calendar->properties = icalfileset_new(path);
    }

    return calendar->properties;
}

icalset *icalcalendar_get_freebusy(icalcalendar *calendar)
{
    char path[MAXPATHLEN];

    icalerror_check_arg_rz((calendar != 0), "calendar");

    path[0] = '\0';
    strncpy(path, calendar->dir, MAXPATHLEN - 1);
    strncat(path, "/", MAXPATHLEN - strlen(path) - 1);
    strncat(path, FBLIST_FILE, MAXPATHLEN - strlen(path) - 1);
    path[MAXPATHLEN - 1] = '\0';

    if (calendar->freebusy == 0) {
        calendar->freebusy = icalfileset_new(path);
    }

    return calendar->freebusy;
}
