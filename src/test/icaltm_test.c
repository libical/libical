/*======================================================================
 FILE: icaltm_test.c

 Copyright (C) 2017 Red Hat, Inc. <www.redhat.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/

 The Initial Developer of the Original Code is Milan Crha
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libical/ical.h"

#include <pthread.h>

static icaltimezone *zone, *utc;

static void *test_tread()
{
    struct icaltimetype itt;
    int ii;

    itt = icaltime_from_string("19710203T040506");
    itt.zone = zone;

    for (ii = 0; ii < 200; ii++, itt.year++) {
        icaltime_convert_to_zone(itt, utc);
    }

    return NULL;
}

int main()
{
    pthread_t thread[2];
    int ii;

    zone = icaltimezone_get_builtin_timezone("America/New_York");
    utc = icaltimezone_get_utc_timezone();

    for (ii = 0; ii < 2; ii++) {
        pthread_create(&thread[ii], NULL, test_tread, NULL);
    }

    for (ii = 0; ii < 2; ii++) {
        pthread_join(thread[ii], NULL);
    }

    return 0;
}
