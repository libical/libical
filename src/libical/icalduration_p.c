/*======================================================================
 * FILE: icalduration_p.c
 * SPDX-FileCopyrightText: 2025 Contributors to the libical project <git@github.com:libical/libical>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * The original code is icalduration.c by Eric Busboom
 *======================================================================*/

#include "icalduration_p.h"

/*************************************************************************
 * WARNING: USE AT YOUR OWN RISK                                         *
 * These are library internal-only functions.                            *
 * Be warned that these functions can change at any time without notice. *
 *************************************************************************/

/* From Seth Alves, <alves@hungry.com>   */
struct icaldurationtype icaldurationtype_from_int(int t)
{
    struct icaldurationtype dur;

    dur = icaldurationtype_null_duration();

    if (t < 0) {
        dur.is_neg = 1;
        t = -t;
    }
    dur.seconds = (unsigned int)t;

    return dur;
}

int icaldurationtype_as_int(struct icaldurationtype dur)
{
    if (dur.days != 0 || dur.weeks != 0) {
        /* The length of a day is position-dependent */
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }
    return (int)(((int)dur.seconds +
                  60 * ((int)dur.minutes +
                        60 * ((int)dur.hours))) *
                 (dur.is_neg == 1 ? -1 : 1));
}
