/*======================================================================
 FILE: testvcal.c
 CREATOR: eric 26 May 2000

 (C) COPYRIGHT 2000 Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libicalvcal/icalvcal.h"
#include "libicalvcal/vcc.h"

#include <assert.h>

/* Given a vCal data file as its first argument, this program will
   print out an equivalent iCal component.

   For instance:

       ./testvcal ../../test-data/user-cal.vcf

*/

int main(int argc, char *argv[])
{
    VObject *vcal = 0;
    icalcomponent *comp;
    const char *file;

    if (argc != 2) {
        file = TEST_DATADIR "/user-cal.vcf";
    } else {
        file = argv[1];
    }

#if ICAL_USE_MALLOC == 0
    icalmemory_set_mem_alloc_funcs(&malloc, &realloc, &free);
#endif

    vcal = Parse_MIME_FromFileName(file);

    assert(vcal != 0);

    comp = icalvcal_convert(vcal);

    printf("%s\n", icalcomponent_as_ical_string(comp));

    icalcomponent_free(comp);
    return 0;
}
