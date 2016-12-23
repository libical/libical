/*======================================================================
 FILE: icalcalendar.c
 CREATOR: eric 23 December 1999

 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
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

struct icalcalendar_impl
{
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

static icalerrorenum icalcalendar_create(struct icalcalendar_impl *impl)
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
        free(impl);
        return 0;
    }

    return impl;
}

void icalcalendar_free(icalcalendar *impl)
{
    if (impl->dir != 0) {
        free(impl->dir);
    }

    if (impl->freebusy != 0) {
        icalset_free(impl->freebusy);
    }

    if (impl->properties != 0) {
        icalset_free(impl->properties);
    }

    if (impl->booked != 0) {
        icalset_free(impl->booked);
    }

    if (impl->incoming != 0) {
        icalset_free(impl->incoming);
    }

    impl->dir = 0;
    impl->freebusy = 0;
    impl->properties = 0;
    impl->booked = 0;
    impl->incoming = 0;

    free(impl);
}

int icalcalendar_lock(icalcalendar *impl)
{
    icalerror_check_arg_rz((impl != 0), "impl");
    return 0;
}

int icalcalendar_unlock(icalcalendar *impl)
{
    icalerror_check_arg_rz((impl != 0), "impl");
    return 0;
}

int icalcalendar_islocked(icalcalendar *impl)
{
    icalerror_check_arg_rz((impl != 0), "impl");
    return 0;
}

int icalcalendar_ownlock(icalcalendar *impl)
{
    icalerror_check_arg_rz((impl != 0), "impl");
    return 0;
}

icalset *icalcalendar_get_booked(icalcalendar *impl)
{
    char dir[MAXPATHLEN];

    icalerror_check_arg_rz((impl != 0), "impl");

    dir[0] = '\0';
    strncpy(dir, impl->dir, MAXPATHLEN - 1);
    strncat(dir, "/", MAXPATHLEN - strlen(dir) - 1);
    strncat(dir, BOOKED_DIR, MAXPATHLEN - strlen(dir) - 1);
    dir[MAXPATHLEN - 1] = '\0';

    if (impl->booked == 0) {
        icalerror_clear_errno();
        impl->booked = icaldirset_new(dir);
        assert(icalerrno == ICAL_NO_ERROR);
    }

    return impl->booked;
}

icalset *icalcalendar_get_incoming(icalcalendar *impl)
{
    char path[MAXPATHLEN];

    icalerror_check_arg_rz((impl != 0), "impl");

    path[0] = '\0';
    strncpy(path, impl->dir, MAXPATHLEN - 1);
    strncat(path, "/", MAXPATHLEN - strlen(path) - 1);
    strncat(path, INCOMING_FILE, MAXPATHLEN - strlen(path) - 1);
    path[MAXPATHLEN - 1] = '\0';

    if (impl->properties == 0) {
        impl->properties = icalfileset_new(path);
    }

    return impl->properties;
}

icalset *icalcalendar_get_properties(icalcalendar *impl)
{
    char path[MAXPATHLEN];

    icalerror_check_arg_rz((impl != 0), "impl");

    path[0] = '\0';
    strncpy(path, impl->dir, MAXPATHLEN - 1);
    strncat(path, "/", MAXPATHLEN - strlen(path) - 1);
    strncat(path, PROP_FILE, MAXPATHLEN - strlen(path) - 1);
    path[MAXPATHLEN - 1] = '\0';

    if (impl->properties == 0) {
        impl->properties = icalfileset_new(path);
    }

    return impl->properties;
}

icalset *icalcalendar_get_freebusy(icalcalendar *impl)
{
    char path[MAXPATHLEN];

    icalerror_check_arg_rz((impl != 0), "impl");

    path[0] = '\0';
    strncpy(path, impl->dir, MAXPATHLEN - 1);
    strncat(path, "/", MAXPATHLEN - strlen(path) - 1);
    strncat(path, FBLIST_FILE, MAXPATHLEN - strlen(path) - 1);
    path[MAXPATHLEN - 1] = '\0';

    if (impl->freebusy == 0) {
        impl->freebusy = icalfileset_new(path);
    }

    return impl->freebusy;
}
