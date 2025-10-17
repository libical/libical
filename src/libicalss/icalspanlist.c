/*======================================================================
 FILE: icalspanlist.c
 CREATOR: ebusboom 23 aug 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalspanlist.h"
#include "icaltimezone.h"

#include <stdlib.h>

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
    const struct icaltime_span *span_a = (struct icaltime_span *)a;
    const struct icaltime_span *span_b = (struct icaltime_span *)b;

    if (span_a->start == span_b->start) {
        return 0;
    } else if (span_a->start < span_b->start) {
        return -1;
    } else { /*if(span_a->start > span->b.start) */
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

static void icalspanlist_new_callback(const icalcomponent *comp, const struct icaltime_span *span, void *data)
{
    icaltime_span *s;
    icalspanlist *sl = (icalspanlist *)data;

    _unused(comp);

    if (span->is_busy == 0) {
        return;
    }

    if ((s = (icaltime_span *)malloc(sizeof(icaltime_span))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return;
    }

    /* copy span data into allocated memory.. **/
    *s = *span;
    icalpvl_insert_ordered(sl->spans, compare_span, (void *)s);
}

icalspanlist *icalspanlist_new(icalset *set, struct icaltimetype start, struct icaltimetype end)
{
    struct icaltime_span range;
    icalpvl_elem itr;
    icalcomponent_kind inner_kind;
    icalspanlist *sl;
    struct icaltime_span *freetime;

    if ((sl = (struct icalspanlist_impl *)malloc(sizeof(struct icalspanlist_impl))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    sl->spans = icalpvl_newlist();
    sl->start = start;
    sl->end = end;

    range.start = icaltime_as_timet(start);
    range.end = icaltime_as_timet(end);

    /* Gets a list of spans of busy time from the events in the set
       and order the spans based on the start time */

    for (icalcomponent *c = icalset_get_first_component(set);
         c != 0;
         c = icalset_get_next_component(set)) {
        icalcomponent_kind kind = icalcomponent_isa(c);
        icalcomponent *inner = icalcomponent_get_inner(c);

        if (!inner) {
            continue;
        }

        inner_kind = icalcomponent_isa(inner);

        if (kind != ICAL_VEVENT_COMPONENT && inner_kind != ICAL_VEVENT_COMPONENT) {
            continue;
        }

        icalerror_clear_errno();

        icalcomponent_foreach_recurrence(c, start, end, icalspanlist_new_callback, (void *)sl);
    }

    /* Now Fill in the free time spans. loop through the spans. if the
       start of the range is not within the span, create a free entry
       that runs from the start of the range to the start of the
       span. */

    for (itr = icalpvl_head(sl->spans); itr != 0; itr = icalpvl_next(itr)) {
        const struct icaltime_span *s = (struct icaltime_span *)icalpvl_data(itr);

        if (!s) {
            continue;
        }

        if ((freetime = (struct icaltime_span *)malloc(sizeof(struct icaltime_span))) == 0) {
            icalerror_set_errno(ICAL_NEWFAILED_ERROR);
            icalspanlist_free(sl);
            return 0;
        }

        if (range.start < s->start) {
            freetime->start = range.start;
            freetime->end = s->start;

            freetime->is_busy = 0;

            icalpvl_insert_ordered(sl->spans, compare_span, (void *)freetime);
        } else {
            free(freetime);
        }

        range.start = s->end;
    }

    /* If the end of the range is null, then assume that everything
       after the last item in the calendar is open and add a span
       that indicates this */

    if (icaltime_is_null_time(end)) {
        const struct icaltime_span *last_span;

        last_span = (struct icaltime_span *)icalpvl_data(icalpvl_tail(sl->spans));

        if (last_span != 0) {
            if ((freetime = (struct icaltime_span *)malloc(sizeof(struct icaltime_span))) == 0) {
                icalerror_set_errno(ICAL_NEWFAILED_ERROR);
                icalspanlist_free(sl);
                return 0;
            }

            freetime->is_busy = 0;
            freetime->start = last_span->end;
            freetime->end = freetime->start;
            icalpvl_insert_ordered(sl->spans, compare_span, (void *)freetime);
        }
    }

    return sl;
}

void icalspanlist_free(icalspanlist *sl)
{
    struct icaltime_span *span;

    if (sl == NULL) {
        return;
    }

    while ((span = icalpvl_pop(sl->spans)) != 0) {
        free(span);
    }

    icalpvl_free(sl->spans);

    sl->spans = 0;

    free(sl);
}

void icalspanlist_dump(icalspanlist *sl)
{
    int i = 0;
    icalpvl_elem itr;

    for (itr = icalpvl_head(sl->spans); itr != 0; itr = icalpvl_next(itr)) {
        struct icaltime_span *s = (struct icaltime_span *)icalpvl_data(itr);
        if (s) {
            printf("#%02d %d start: %s", ++i, s->is_busy, icalctime(&s->start));
            printf("      end  : %s", icalctime(&s->end));
        }
    }
}

icalcomponent *icalspanlist_make_busy_list(icalspanlist *sl);

struct icalperiodtype icalspanlist_next_free_time(icalspanlist *sl, struct icaltimetype t)
{
    icalpvl_elem itr;
    struct icalperiodtype period;
    struct icaltime_span *s;

    icaltime_t rangett = icaltime_as_timet(t);

    period.start = icaltime_null_time();
    period.end = icaltime_null_time();
    period.duration = icaldurationtype_null_duration();

    itr = icalpvl_head(sl->spans);
    s = (struct icaltime_span *)icalpvl_data(itr);

    if (s == 0) {
        /* No elements in span */
        return period;
    }

    /* Is the reference time before the first span? If so, assume
       that the reference time is free */
    if (rangett < s->start) {
        /* End of period is start of first span if span is busy, end
           of the span if it is free */
        period.start = t;

        if (s->is_busy == 1) {
            period.end = icaltime_from_timet_with_zone(s->start, 0, NULL);
        } else {
            period.end = icaltime_from_timet_with_zone(s->end, 0, NULL);
        }

        return period;
    }

    /* Otherwise, find the first free span that contains the
       reference time. */
    for (itr = icalpvl_head(sl->spans); itr != 0; itr = icalpvl_next(itr)) {
        s = (struct icaltime_span *)icalpvl_data(itr);

        if (!s) {
            continue;
        }

        if (s->is_busy == 0 && s->start >= rangett && (rangett < s->end || s->end == s->start)) {
            if (rangett < s->start) {
                period.start = icaltime_from_timet_with_zone(s->start, 0, NULL);
            } else {
                period.start = icaltime_from_timet_with_zone(rangett, 0, NULL);
            }

            period.end = icaltime_from_timet_with_zone(s->end, 0, NULL);

            return period;
        }
    }

    period.start = icaltime_null_time();
    period.end = icaltime_null_time();

    return period;
}

int *icalspanlist_as_freebusy_matrix(icalspanlist *spanlist, int delta_t)
{
    icalpvl_elem itr;
    icaltime_t spanduration_secs;
    int *matrix;
    icaltime_t matrix_slots;
    icaltime_t sl_start, sl_end;

    icalerror_check_arg_rz((spanlist != 0), "spanlist");

    if (!delta_t) {
        delta_t = 3600;
    }

    /* calculate the start and end time as icaltime_t **/
    sl_start = icaltime_as_timet_with_zone(spanlist->start, icaltimezone_get_utc_timezone());
    sl_end = icaltime_as_timet_with_zone(spanlist->end, icaltimezone_get_utc_timezone());

    /* insure that the time period falls on a time boundary divisible
      by delta_t */

    sl_start /= delta_t;
    sl_start *= delta_t;

    sl_end /= delta_t;
    sl_end *= delta_t;

    /* find the duration of this spanlist **/
    spanduration_secs = sl_end - sl_start;

    /* malloc our matrix, add one extra slot for a final -1 **/
    matrix_slots = spanduration_secs / delta_t + 1;

    matrix = (int *)malloc(sizeof(int) * (size_t)matrix_slots);
    if (matrix == NULL) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }
    memset(matrix, 0, sizeof(int) * (size_t)matrix_slots);
    matrix[matrix_slots - 1] = -1;

    /* loop through each span and mark the slots in the array */

    for (itr = icalpvl_head(spanlist->spans); itr != 0; itr = icalpvl_next(itr)) {
        const struct icaltime_span *s = (struct icaltime_span *)icalpvl_data(itr);

        if (s && s->is_busy == 1) {
            icaltime_t offset_start = s->start / delta_t - sl_start / delta_t;
            icaltime_t offset_end = (s->end - 1) / delta_t - sl_start / delta_t + 1;
            icaltime_t i;

            if (offset_end >= matrix_slots) {
                offset_end = matrix_slots - 1;
            }

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
    struct icaltimetype atime = icaltime_from_timet_with_zone(time(0), 0, NULL);
    icalpvl_elem itr;
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

    for (itr = icalpvl_head(sl->spans); itr != 0; itr = icalpvl_next(itr)) {
        struct icalperiodtype period;
        struct icaltime_span *s = (struct icaltime_span *)icalpvl_data(itr);

        if (s && s->is_busy == 1) {
            period.start = icaltime_from_timet_with_zone(s->start, 0, utc_zone);
            period.end = icaltime_from_timet_with_zone(s->end, 0, utc_zone);
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
    if (!inner) {
        return NULL;
    }

    if ((sl = (icalspanlist *)malloc(sizeof(icalspanlist))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }
    sl->spans = icalpvl_newlist();

    /* cycle through each FREEBUSY property, adding to the spanlist */
    for (prop = icalcomponent_get_first_property(inner, ICAL_FREEBUSY_PROPERTY);
         prop != NULL;
         prop = icalcomponent_get_next_property(inner, ICAL_FREEBUSY_PROPERTY)) {
        icaltime_span *s = (icaltime_span *)malloc(sizeof(icaltime_span));
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
        s->start = icaltime_as_timet_with_zone(period.start, icaltimezone_get_utc_timezone());
        s->end = icaltime_as_timet_with_zone(period.end, icaltimezone_get_utc_timezone());

        icalpvl_insert_ordered(sl->spans, compare_span, (void *)s);
    }
    /** @todo calculate start/end limits.. fill in holes? **/
    return sl;
}
