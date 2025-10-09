/*======================================================================
 FILE: testvcal.c
 CREATOR: eric 26 May 2000

 SPDX-FileCopyrightText: 2000 Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

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

/* cppcheck-suppress constParameter */
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

    vcal = Parse_MIME_FromFileName(file);

    assert(vcal != 0);

    comp = icalvcal_convert(vcal);

    printf("%s\n", icalcomponent_as_ical_string(comp));

    icalcomponent_free(comp);
    return 0;
}
