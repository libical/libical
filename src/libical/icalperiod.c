/*======================================================================
 FILE: icalperiod.c
 CREATOR: eric 02 June 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalperiod.h"
#include "icalerror.h"
#include "icalmemory.h"

struct icalperiodtype icalperiodtype_from_string(const char *str)
{
    struct icalperiodtype p, null_p;
    char *s = icalmemory_strdup(str);
    char *start, *end;
    icalerrorstate es;

    /* Errors are normally generated in the following code, so save
       the error state for resoration later */

    icalerrorenum e = icalerrno;

    p.start = p.end = icaltime_null_time();
    p.duration = icaldurationtype_null_duration();

    null_p = p;

    if (s == 0) {
        goto error;
    }

    start = s;
    end = strchr(s, '/');

    if (end == 0) {
        goto error;
    }

    *end = 0;
    end++;

    p.start = icaltime_from_string(start);

    if (icaltime_is_null_time(p.start)) {
        goto error;
    }

    es = icalerror_get_error_state(ICAL_MALFORMEDDATA_ERROR);
    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, ICAL_ERROR_NONFATAL);

    p.end = icaltime_from_string(end);

    icalerror_set_error_state(ICAL_MALFORMEDDATA_ERROR, es);

    if (icaltime_is_null_time(p.end)) {
        p.duration = icaldurationtype_from_string(end);

        if (icaldurationtype_is_null_duration(p.duration)) {
            goto error;
        }
    }

    icalerrno = e;
    icalmemory_free_buffer(s);
    return p;

error:
    icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
    icalmemory_free_buffer(s);
    return null_p;
}

const char *icalperiodtype_as_ical_string(struct icalperiodtype p)
{
    char *buf;

    buf = icalperiodtype_as_ical_string_r(p);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icalperiodtype_as_ical_string_r(struct icalperiodtype p)
{
    const char *start;
    const char *end;

    char *buf;
    size_t buf_size = 40;
    char *buf_ptr = 0;

    buf = (char *)icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    start = icaltime_as_ical_string_r(p.start);
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, start);
    icalmemory_free_buffer((void *)start);

    if (!icaltime_is_null_time(p.end)) {
        end = icaltime_as_ical_string_r(p.end);
    } else {
        end = icaldurationtype_as_ical_string_r(p.duration);
    }

    icalmemory_append_char(&buf, &buf_ptr, &buf_size, '/');

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, end);
    icalmemory_free_buffer((void *)end);

    return buf;
}

struct icalperiodtype icalperiodtype_null_period(void)
{
    struct icalperiodtype p;

    p.start = icaltime_null_time();
    p.end = icaltime_null_time();
    p.duration = icaldurationtype_null_duration();

    return p;
}

bool icalperiodtype_is_null_period(struct icalperiodtype p)
{
    if (icaltime_is_null_time(p.start) &&
        icaltime_is_null_time(p.end) && icaldurationtype_is_null_duration(p.duration)) {
        return true;
    } else {
        return false;
    }
}

bool icalperiodtype_is_valid_period(struct icalperiodtype p)
{
    if (icaltime_is_valid_time(p.start) &&
        (icaltime_is_valid_time(p.end) || icaltime_is_null_time(p.end))) {
        return true;
    }

    return false;
}
