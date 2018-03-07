/*======================================================================
 FILE: process.c
 CREATOR: eric 11 February 2000

 (C) COPYRIGHT 2000 Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libical/ical.h"
#include "libicalss/icalss.h"

void send_message(icalcomponent *reply, const char *this_user)
{
    printf("From: %s\n\n%s\n", this_user, icalcomponent_as_ical_string(reply));
}

int main(int argc, char *argv[])
{
    icalcomponent *c, *next_c = NULL;
    int i = 0;
    int dont_remove;
    icalfileset_options options = { O_RDONLY, 0644, 0, NULL };
    icalset *f;
    icalset *trash;
    icalset *cal;
    icalset *out;
    const char *this_user;

    _unused(argc);
    _unused(argv);

#if ICAL_USE_MALLOC == 0
    icalmemory_set_mem_alloc_funcs(&malloc, &realloc, &free);
#endif

    f = icalset_new(ICAL_FILE_SET, TEST_DATADIR "/process-incoming.ics", &options);
    trash = icalset_new_file("trash.ics");
    cal    = icalset_new(ICAL_FILE_SET, TEST_DATADIR "/process-calendar.ics", &options);
    out = icalset_new_file("outgoing.ics");

    this_user = "alice@cal.softwarestudio.org";

    assert(f != 0);
    assert(cal != 0);
    assert(trash != 0);
    assert(out != 0);

    /* Foreach incoming message */
    for (c = icalset_get_first_component(f); c != 0; c = next_c) {

        icalproperty_xlicclass class;
        icalcomponent *match;
        icalcomponent *inner;
        icalcomponent *reply = 0;

        assert(c != 0);

        inner = icalcomponent_get_first_real_component(c);

        i++;
        reply = 0;
        dont_remove = 0;

        if (inner == 0) {
            printf("Bad component, no inner\n %s\n", icalcomponent_as_ical_string(c));
            continue;
        }

        /* Find a booked component that is matched to the incoming
           message, based on the incoming component's UID, SEQUENCE
           and RECURRENCE-ID */

        match = icalset_fetch_match(cal, c);

        class = icalclassify(c, match, this_user);

        /* Print out the notes associated with the incoming component
           and the matched component in the */
        {
            const char *inc_note = 0;
            const char *match_note = 0;
            icalproperty *p;

            for (p = icalcomponent_get_first_property(c, ICAL_X_PROPERTY);
                 p != 0; p = icalcomponent_get_next_property(c, ICAL_X_PROPERTY)) {

                if (strcmp(icalproperty_get_x_name(p), "X-LIC-NOTE") == 0) {
                    inc_note = icalproperty_get_x(p);
                }
            }

            if (match != 0) {
                for (p = icalcomponent_get_first_property(match, ICAL_X_PROPERTY);
                     p != 0;
                     p = icalcomponent_get_next_property(match, ICAL_X_PROPERTY)) {
                    if (strcmp(icalproperty_get_x_name(p), "X-LIC-NOTE") == 0) {
                        match_note = icalproperty_get_x(p);
                    }
                }
            }

            if (inc_note != 0) {
                printf("Incoming: %s\n", inc_note);
            }
            if (match_note != 0) {
                printf("Match   : %s\n", match_note);
            }
        }

        /* Main processing structure */

        switch (class) {
        case ICAL_XLICCLASS_NONE:{
                char temp[1024];

                /* Huh? Return an error to sender */
                icalrestriction_check(c);
                icalcomponent_convert_errors(c);

                snprintf(temp, 1024,
                         "I can't understand the component you sent.\n"
                         "Here is the component you sent, possibly with error messages:\n"
                         "%s",
                         icalcomponent_as_ical_string(c));

                reply = icalmessage_new_error_reply(c, this_user, temp, "", ICAL_UNKNOWN_STATUS);

                break;
            }
        case ICAL_XLICCLASS_PUBLISHNEW:{

                /* Don't accept published events from anyone but
                   self. If self, fall through to ICAL_XLICCLASS_REQUESTNEW */
            }
        case ICAL_XLICCLASS_REQUESTNEW:{

                /* Book the new component if it does not overlap
                   anything. If the time is busy and the start time is
                   an even modulo 4, delegate to
                   bob@cal.softwarestudio.org. If the time is busy and
                   is 1 modulo 4, counterpropose for the first
                   available free time. Otherwise, deline the meeting */

                icalcomponent *overlaps = icalclassify_find_overlaps(cal, c);

                if (overlaps == 0) {
                    /* No overlaps, book the meeting */
/*                  icalset_add_component(cal,icalcomponent_new_clone(c));*/

                    /* Return a reply */
                    reply =
                        icalmessage_new_accept_reply(
                            c, this_user, "I can make it to this meeting");

                    (void)icalset_add_component(out, reply);

                } else {
                    /* There was a conflict, so delegate, counterpropose
                       or decline it */
                    struct icaltimetype dtstart = icalcomponent_get_dtstart(c);

                    if (dtstart.hour % 4 == 0) {
                        /* Delegate the meeting */
                        reply =
                            icalmessage_new_delegate_reply(
                                c,
                                this_user,
                                "bob@cal.softwarestudio.org",
                                "Unfortunately, I have another commitment that conflicts "
                                "with this meeting. I am delegating my attendance to Bob.");
                        (void)icalset_add_component(out, reply);

                    } else if (dtstart.hour % 4 == 1) {
                        /* Counter propose to next available time */
                        icalcomponent *newc;
                        struct icalperiodtype next_time;

                        icalspanlist *spanl = icalspanlist_new(cal, dtstart,
                                                               icaltime_null_time());

                        next_time =
                            icalspanlist_next_free_time(spanl, icalcomponent_get_dtstart(c));

                        newc = icalcomponent_new_clone(c);

                        icalcomponent_set_dtstart(newc, next_time.start);

                        /* Hack, the duration of the counterproposed
                           meeting may be longer than the free time
                           available */
                        icalcomponent_set_duration(newc, icalcomponent_get_duration(c));

                        reply =
                            icalmessage_new_counterpropose_reply(
                                c,
                                newc,
                                this_user,
                                "Unfortunately, I have another commitment that conflicts with "
                                "this meeting. I am proposing a time that works better for me.");

                        (void)icalset_add_component(out, reply);
                        icalspanlist_free(spanl);
                        icalcomponent_free(newc);

                    } else {
                        /* Decline the meeting */

                        reply =
                            icalmessage_new_decline_reply(
                                c,
                                this_user,
                                "I can't make it to this meeting");

                        (void)icalset_add_component(out, reply);
                    }
                }
                icalcomponent_free(overlaps);
                break;
            }
        case ICAL_XLICCLASS_PUBLISHFREEBUSY:{
                /* Store the busy time information in a file named after
                   the sender */
                break;
            }

        case ICAL_XLICCLASS_PUBLISHUPDATE:{
                /* Only accept publish updates from self. If self, fall
                   through to ICAL_XLICCLASS_REQUESTUPDATE */
            }

        case ICAL_XLICCLASS_REQUESTUPDATE:{
                /* always accept the changes */
                break;
            }

        case ICAL_XLICCLASS_REQUESTRESCHEDULE:{
                /* Use same rules as REQUEST_NEW */
                (void)icalclassify_find_overlaps(cal, c);
                break;
            }
        case ICAL_XLICCLASS_REQUESTDELEGATE:{

                break;
            }
        case ICAL_XLICCLASS_REQUESTNEWORGANIZER:{
                break;
            }
        case ICAL_XLICCLASS_REQUESTFORWARD:{
                break;
            }
        case ICAL_XLICCLASS_REQUESTSTATUS:{
                break;
            }

        case ICAL_XLICCLASS_REQUESTFREEBUSY:{
                break;
            }
        case ICAL_XLICCLASS_REPLYACCEPT:{
                /* Change the PARTSTAT of the sender */
                break;
            }
        case ICAL_XLICCLASS_REPLYDECLINE:{
                /* Change the PARTSTAT of the sender */
                break;
            }
        case ICAL_XLICCLASS_REPLYCRASHERACCEPT:{
                /* Add the crasher to the ATTENDEE list with the
                   appropriate PARTSTAT */
                break;
            }
        case ICAL_XLICCLASS_REPLYCRASHERDECLINE:{
                /* Add the crasher to the ATTENDEE list with the
                   appropriate PARTSTAT */
                break;
            }
        case ICAL_XLICCLASS_ADDINSTANCE:{
                break;
            }
        case ICAL_XLICCLASS_CANCELEVENT:{
                /* Remove the component */
                break;
            }
        case ICAL_XLICCLASS_CANCELINSTANCE:{
                break;
            }
        case ICAL_XLICCLASS_CANCELALL:{
                /* Remove the component */
                break;
            }
        case ICAL_XLICCLASS_REFRESH:{
                /* Resend the latest copy of the request */
                break;
            }
        case ICAL_XLICCLASS_COUNTER:{
                break;
            }
        case ICAL_XLICCLASS_DECLINECOUNTER:{
                break;
            }
        case ICAL_XLICCLASS_MALFORMED:{
                /* Send back an error */
                break;
            }
        case ICAL_XLICCLASS_OBSOLETE:{
                printf(" ** Got an obsolete component:\n%s", icalcomponent_as_ical_string(c));
                /* Send back an error */
                break;
            }
        case ICAL_XLICCLASS_MISSEQUENCED:{
                printf(" ** Got a missequenced component:\n%s", icalcomponent_as_ical_string(c));
                /* Send back an error */
                break;
            }
        case ICAL_XLICCLASS_UNKNOWN:{
                printf(" ** Don't know what to do with this component:\n%s",
                       icalcomponent_as_ical_string(c));
                /* Send back an error */
                break;
            }
        case ICAL_XLICCLASS_X:
        case ICAL_XLICCLASS_REPLYDELEGATE:
        default:{
            }
        }

#if 0
        if (reply != 0) {

            /* Don't send the reply if the RSVP parameter indicates not to */
            icalcomponent *reply_inner;
            icalproperty *attendee;
            icalparameter *rsvp;

            reply_inner = icalcomponent_get_first_real_component(reply);
            attendee = icalcomponent_get_first_property(reply_inner, ICAL_ATTENDEE_PROPERTY);
            rsvp = icalproperty_get_first_parameter(attendee, ICAL_RSVP_PARAMETER);

            if (rsvp == 0 || icalparameter_get_rsvp(rsvp) == 1) {
                icalrestriction_check(reply);
                send_message(reply, this_user);
            }

            icalcomponent_free(reply);
        }
#endif

        if (reply != 0) {
            printf("%s\n", icalcomponent_as_ical_string(reply));
        }

        next_c = icalset_get_next_component(f);

        if (dont_remove == 0) {
            /*icalset_remove_component(f,c);
               icalset_add_component(trash,c); */
        }
    }

#if 0

    for (c = icalset_get_first_component(out); c != 0; c = icalset_get_next_component(out)) {

        printf("%s", icalcomponent_as_ical_string(c));
    }
#endif

    icalset_free(f);
    icalset_free(trash);
    icalset_free(cal);
    icalset_free(out);

    return 0;
}
