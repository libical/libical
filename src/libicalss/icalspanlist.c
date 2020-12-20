/*======================================================================
 FILE: icalspanlist.c
 CREATOR: ebusboom 23 aug 2000

 (C) COPYRIGHT 2000, Eric Busboom <eric@civicknowledge.com>

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at https://www.mozilla.org/MPL/
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalspanlist.h"
#include "icaltimezone.h"

#include <stdlib.h>

struct icalspanlist_impl
{
    pvl_list spans;             /**< list of icaltime_span data **/
    struct icaltimetype start;  /**< start time of span **/
    struct icaltimetype end;    /**< end time of span **/
};

/** @brief Internal comparison function for two spans
 *
 *  @param  a   a spanlist.
 *  @param  b   another spanlist.
 *
 *  @return     -1, 0, 1 depending on the comparison of the start times.
 *
 * Used to insert spans into the tree in sorted order.
 */

static int compare_span(void *a, void *b)
{
    struct icaltime_span *span_a = (struct icaltime_span *)a;
    struct icaltime_span *span_b = (struct icaltime_span *)b;

    if (icaltime_timespec_cmp(span_a->start, span_b->start) == 0) {
        return 0;
    } else if (icaltime_timespec_cmp(span_a->start, span_b->start) < 0) {
        return -1;
    } else {    /*if(span_a->start > span->b.start) */
        return 1;
    }
}

/** @brief callback function for collecting spanlists of a
 *         series of events.
 *
 *  @param   comp  A valid icalcomponent.
 *  @param   span  The span to insert into data.
 *  @param   data  The actual spanlist to insert into
 *
 *  This callback is used by icalcomponent_foreach_recurrence()
 *  to build up a spanlist.
 */

static void icalspanlist_new_callback(icalcomponent *comp, struct icaltime_span *span, void *data)
{
    icaltime_span *s;
    icalspanlist *sl = (icalspanlist *) data;

    _unused(comp);

    if (span->is_busy == 0)
        return;

    if ((s = (icaltime_span *) malloc(sizeof(icaltime_span))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return;
    }

  /* copy span data into allocated memory.. **/
    *s = *span;
    pvl_insert_ordered(sl->spans, compare_span, (void *)s);
}

icalspanlist *icalspanlist_new(icalset *set, struct icaltimetype start, struct icaltimetype end)
{
    struct icaltime_span range;
    pvl_elem itr;
    icalcomponent *c, *inner;
    icalcomponent_kind kind, inner_kind;
    icalspanlist *sl;
    struct icaltime_span *freetime;

    if ((sl = (struct icalspanlist_impl *)malloc(sizeof(struct icalspanlist_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    sl->spans = pvl_newlist();
    sl->start = start;
    sl->end = end;

    range.start = icaltime_as_timespec(start);
    range.end = icaltime_as_timespec(end);

    /* Gets a list of spans of busy time from the events in the set
       and order the spans based on the start time */

    for (c = icalset_get_first_component(set);
         c != 0;
         c = icalset_get_next_component(set)) {

        kind = icalcomponent_isa(c);
        inner = icalcomponent_get_inner(c);

        if (!inner)
            continue;

        inner_kind = icalcomponent_isa(inner);

        if (kind != ICAL_VEVENT_COMPONENT && inner_kind != ICAL_VEVENT_COMPONENT)
            continue;

        icalerror_clear_errno();

        icalcomponent_foreach_recurrence(c, start, end, icalspanlist_new_callback, (void *)sl);
    }

    /* Now Fill in the free time spans. loop through the spans. if the
       start of the range is not within the span, create a free entry
       that runs from the start of the range to the start of the
       span. */

    for (itr = pvl_head(sl->spans); itr != 0; itr = pvl_next(itr)) {
        struct icaltime_span *s = (struct icaltime_span *)pvl_data(itr);

        if (!s)
            continue;

        if ((freetime = (struct icaltime_span *)malloc(sizeof(struct icaltime_span))) == 0) {
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            icalspanlist_free(sl);
            return 0;
        }

        if (icaltime_timespec_cmp(range.start, s->start) < 0) {
            freetime->start = range.start;
            freetime->end = s->start;

            freetime->is_busy = 0;

            pvl_insert_ordered(sl->spans, compare_span, (void *)freetime);
        } else {
            free(freetime);
        }

        range.start = s->end;
    }

    /* If the end of the range is null, then assume that everything
       after the last item in the calendar is open and add a span
       that indicates this */

    if (icaltime_is_null_time(end)) {
        struct icaltime_span *last_span;

        last_span = (struct icaltime_span *)pvl_data(pvl_tail(sl->spans));

        if (last_span != 0) {

            if ((freetime = (struct icaltime_span *)malloc(sizeof(struct icaltime_span))) == 0) {
                icalerror_set_errno(ICAL_NEWFAILED_ERROR);
                icalspanlist_free(sl);
                return 0;
            }

            freetime->is_busy = 0;
            freetime->start = last_span->end;
            freetime->end = freetime->start;
            pvl_insert_ordered(sl->spans, compare_span, (void *)freetime);
        }
    }

    return sl;
}

void icalspanlist_free(icalspanlist *s)
{
    struct icaltime_span *span;

    if (s == NULL)
        return;

    while ((span = pvl_pop(s->spans)) != 0) {
        free(span);
    }

    pvl_free(s->spans);

    s->spans = 0;

    free(s);
}

void icalspanlist_dump(icalspanlist *sl)
{
    int i = 0;
    pvl_elem itr;

    for (itr = pvl_head(sl->spans); itr != 0; itr = pvl_next(itr)) {
        struct icaltime_span *s = (struct icaltime_span *)pvl_data(itr);
        if (s) {
            printf("#%02d %d start: %s", ++i, s->is_busy, icaltime_timespec_as_string(s->start));
            printf("      end  : %s", icaltime_timespec_as_string(s->end));
        }
    }
}

icalcomponent *icalspanlist_make_busy_list(icalspanlist *sl);

struct icalperiodtype icalspanlist_next_free_time(icalspanlist *sl, struct icaltimetype t)
{
    pvl_elem itr;
    struct icalperiodtype period;
    struct icaltime_span *s;

    timespec_t rangett = icaltime_as_timespec(t);

    period.start = icaltime_null_time();
    period.end = icaltime_null_time();

    itr = pvl_head(sl->spans);
    s = (struct icaltime_span *)pvl_data(itr);

    if (s == 0) {
        /* No elements in span */
        return period;
    }

    /* Is the reference time before the first span? If so, assume
       that the reference time is free */
    if (icaltime_timespec_cmp(rangett, s->start) < 0) {
        /* End of period is start of first span if span is busy, end
           of the span if it is free */
        period.start = t;

        if (s->is_busy == 1) {
            period.end = icaltime_from_timespec_with_zone(s->start, 0, NULL);
        } else {
            period.end = icaltime_from_timespec_with_zone(s->end, 0, NULL);
        }

        return period;
    }

    /* Otherwise, find the first free span that contains the
       reference time. */
    for (itr = pvl_head(sl->spans); itr != 0; itr = pvl_next(itr)) {
        s = (struct icaltime_span *)pvl_data(itr);

        if (!s)
            continue;

        if (s->is_busy == 0 &&
            (icaltime_timespec_cmp(s->start, rangett) > 0 ||
             icaltime_timespec_cmp(s->start, rangett) == 0) &&
	    (icaltime_timespec_cmp(rangett, s->end) < 0 ||
	     icaltime_timespec_cmp(s->end, s->start) == 0)) {

            if (icaltime_timespec_cmp(rangett, s->start) < 0) {
                period.start = icaltime_from_timespec_with_zone(s->start, 0, NULL);
            } else {
                period.start = icaltime_from_timespec_with_zone(rangett, 0, NULL);
            }

            period.end = icaltime_from_timespec_with_zone(s->end, 0, NULL);

            return period;
        }
    }

    period.start = icaltime_null_time();
    period.end = icaltime_null_time();

    return period;
}

int *icalspanlist_as_freebusy_matrix(icalspanlist *sl, int delta_t)
{
    pvl_elem itr;
    int64_t spanduration_ms;
    int *matrix;
    time_t matrix_slots;
    timespec_t sl_start, sl_end;
    int64_t sl_start_ms, sl_end_ms, delta_ms;

    icalerror_check_arg_rz((sl != 0), "spanlist");

    if (!delta_t) {
        delta_ms = 3600;
    } else {
        delta_ms = delta_t * 1000;
    }

  /* calculate the start and end time as time_t **/
    sl_start = icaltime_as_timespec_with_zone(sl->start, icaltimezone_get_utc_timezone());
    sl_end = icaltime_as_timespec_with_zone(sl->end, icaltimezone_get_utc_timezone());

  /* insure that the time period falls on a time boundary divisable
      by delta_ms */

    sl_start_ms = icaltime_timespec_to_msec(sl_start);
    sl_start_ms /= delta_ms;
    sl_start_ms *= delta_ms;

    sl_end_ms = icaltime_timespec_to_msec(sl_end);
    sl_end_ms /= delta_ms;
    sl_end_ms *= delta_ms;

  /* find the duration of this spanlist **/
    spanduration_ms = sl_end_ms - sl_start_ms;

  /* malloc our matrix, add one extra slot for a final -1 **/
    matrix_slots = spanduration_ms / delta_ms + 1;

    matrix = (int *)malloc((size_t)(sizeof(int) * matrix_slots));
    if (matrix == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }
    memset(matrix, 0, (size_t)(sizeof(int) * matrix_slots));
    matrix[matrix_slots - 1] = -1;

    /* loop through each span and mark the slots in the array */

    for (itr = pvl_head(sl->spans); itr != 0; itr = pvl_next(itr)) {
        struct icaltime_span *s = (struct icaltime_span *)pvl_data(itr);

        if (s && s->is_busy == 1) {
            time_t offset_start = icaltime_timespec_to_msec(s->start) / delta_ms -
                                  sl_start_ms / delta_ms;
            time_t offset_end = (icaltime_timespec_to_msec(s->end) - 1000) / delta_ms -
                                sl_start_ms / delta_ms + 1;
            time_t i;

            if (offset_end >= matrix_slots)
                offset_end = matrix_slots - 1;

            for (i = offset_start; i < offset_end; i++) {
                matrix[i]++;
            }
        }
    }
    return matrix;
}

icalcomponent *icalspanlist_as_vfreebusy(icalspanlist *sl,
                                         const char *organizer, const char *attendee)
{
    icalcomponent *comp;
    icalproperty *p;
    timespec_t ts_zero = icaltime_msec_to_timespec(0);
    struct icaltimetype atime = icaltime_from_timespec_with_zone(ts_zero, 0, NULL);
    pvl_elem itr;
    icaltimezone *utc_zone;
    icalparameter *param;

    if (!attendee) {
        icalerror_set_errno(ICAL_USAGE_ERROR);
        return 0;
    }

    utc_zone = icaltimezone_get_utc_timezone();

    comp = icalcomponent_new_vfreebusy();

    icalcomponent_add_property(comp, icalproperty_new_dtstart(sl->start));
    icalcomponent_add_property(comp, icalproperty_new_dtend(sl->end));
    icalcomponent_add_property(comp, icalproperty_new_dtstamp(atime));

    if (organizer) {
        icalcomponent_add_property(comp, icalproperty_new_organizer(organizer));
    }
    icalcomponent_add_property(comp, icalproperty_new_attendee(attendee));

    /* now add the freebusy sections.. */

    for (itr = pvl_head(sl->spans); itr != 0; itr = pvl_next(itr)) {
        struct icalperiodtype period;
        struct icaltime_span *s = (struct icaltime_span *)pvl_data(itr);

        if (s && s->is_busy == 1) {

            period.start = icaltime_from_timespec_with_zone(s->start, 0, utc_zone);
            period.end = icaltime_from_timespec_with_zone(s->end, 0, utc_zone);
            period.duration = icaldurationtype_null_duration();

            p = icalproperty_new_freebusy(period);
            param = icalparameter_new_fbtype(ICAL_FBTYPE_BUSY);
            icalproperty_add_parameter(p, param);

            icalcomponent_add_property(comp, p);
        }
    }

    return comp;
}

icalspanlist *icalspanlist_from_vfreebusy(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;
    icalspanlist *sl;

    icalerror_check_arg_rz((comp != NULL), "comp");

    inner = icalcomponent_get_inner(comp);
    if (!inner)
        return NULL;

    if ((sl = (icalspanlist *) malloc(sizeof(icalspanlist))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }
    sl->spans = pvl_newlist();

    /* cycle through each FREEBUSY property, adding to the spanlist */
    for (prop = icalcomponent_get_first_property(inner, ICAL_FREEBUSY_PROPERTY);
         prop != NULL;
         prop = icalcomponent_get_next_property(inner, ICAL_FREEBUSY_PROPERTY)) {
        icaltime_span *s = (icaltime_span *) malloc(sizeof(icaltime_span));
        icalparameter *param;
        struct icalperiodtype period;
        icalparameter_fbtype fbtype;

        if (s == 0) {
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            icalspanlist_free(sl);
            return 0;
        }

        param = icalproperty_get_first_parameter(prop, ICAL_FBTYPE_PARAMETER);
        fbtype = (param) ? icalparameter_get_fbtype(param) : ICAL_FBTYPE_BUSY;

        switch (fbtype) {
        case ICAL_FBTYPE_FREE:
        case ICAL_FBTYPE_NONE:
        case ICAL_FBTYPE_X:
            s->is_busy = 1;
            break;
        default:
            s->is_busy = 0;
        }

        period = icalproperty_get_freebusy(prop);
        s->start = icaltime_as_timespec_with_zone(period.start, icaltimezone_get_utc_timezone());
        s->end = icaltime_as_timespec_with_zone(period.end, icaltimezone_get_utc_timezone());

        pvl_insert_ordered(sl->spans, compare_span, (void *)s);
    }
  /** @todo calculate start/end limits.. fill in holes? **/
    return sl;
}
