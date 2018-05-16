/*======================================================================
 FILE: icalrecur_test.c
 CREATOR: Ken Murchison 26 September 2014

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

/*
 * Program for testing libical recurrence iterator on RFC 5545 examples.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libical/ical.h>
#include <stdlib.h>

#if defined(HAVE_LIBICU)
#include <unicode/ucal.h>
#endif

struct recur
{
    const char *dtstart;
    const char *rrule;
    const char *start_at;
};

const struct recur rfc5545[] = {

    /* Every day in January, for 3 years */
    {"19980101T090000",
     "FREQ=YEARLY;UNTIL=20000131T140000Z;BYMONTH=1;BYDAY=SU,MO,TU,WE,TH,FR,SA",
     NULL},

    /* Yearly in June and July for 10 occurrences */
    {"19970610T090000",
     "FREQ=YEARLY;COUNT=10;BYMONTH=6,7",
     NULL},

    /* Every other year on January, February, and March for 10 occurrences */
    {"19970310T090000",
     "FREQ=YEARLY;INTERVAL=2;COUNT=10;BYMONTH=1,2,3",
     NULL},

    /* Every third year on the 1st, 100th, and 200th day for 10 occurrences */
    {"19970101T090000",
     "FREQ=YEARLY;INTERVAL=3;COUNT=10;BYYEARDAY=1,100,200",
     NULL},

    /* Every 20th Monday of the year */
    {"19970519T090000",
     "FREQ=YEARLY;BYDAY=20MO;COUNT=3",
     NULL},

    /* Monday of week number 20
       (where the default start of the week is Monday) */
    {"19970512T090000",
     "FREQ=YEARLY;BYWEEKNO=20;BYDAY=MO;COUNT=3",
     NULL},

    /* Monday of week number 20
       (where the start of the week is Sunday) */
    {"19970512T090000",
     "FREQ=YEARLY;BYWEEKNO=20;BYDAY=MO;WKST=SU;COUNT=3",
     NULL},

    /* Monday of week number 20
       (where the start of the week is Friday) */
    {"19970512T090000",
     "FREQ=YEARLY;BYWEEKNO=20;BYDAY=MO;WKST=FR;COUNT=3",
     NULL},

    /* Every Thursday in March */
    {"19970313T090000",
     "FREQ=YEARLY;BYMONTH=3;BYDAY=TH;COUNT=11",
     NULL},

    /* Every Thursday, but only during June, July, and August */
    {"19970605T090000",
     "FREQ=YEARLY;BYDAY=TH;BYMONTH=6,7,8;COUNT=39",
     NULL},

    /* Every 4 years, the first Thuesday after a Monday in November
       (U.S. Presidential Election Day) */
    {"19961105T090000",
     "FREQ=YEARLY;INTERVAL=4;BYMONTH=11;BYDAY=TU;BYMONTHDAY=2,3,4,5,6,7,8;COUNT=3",
     NULL},

    /* Friday in March occurring on or after the 26th for 5 years */
    {"20060331T020000",
     "FREQ=YEARLY;UNTIL=20100326T000000Z;BYDAY=FR;BYYEARDAY=-275,-276,-277,-278,-279,-280,-281",
     NULL},

    /* Example where leap day gets skipped */
    {"20120229T120000Z",
     "FREQ=YEARLY;UNTIL=20140301T115959Z",
     NULL},

    /* Monthly on the first Friday for 10 occurrences */
    {"19970905T090000",
     "FREQ=MONTHLY;COUNT=10;BYDAY=1FR",
     NULL},

    /* Monthly on the first Friday until December 24, 1997 */
    {"19970905T090000",
     "FREQ=MONTHLY;UNTIL=19971224T000000Z;BYDAY=1FR",
     NULL},

    /* Every other month on the first and last Sunday of the month
       for 10 occurrences */
    {"19970907T090000",
     "FREQ=MONTHLY;INTERVAL=2;COUNT=10;BYDAY=1SU,-1SU",
     NULL},

    /* Monthly on the second-to-last Monday of the month for 6 months */
    {"19970922T090000",
     "FREQ=MONTHLY;COUNT=6;BYDAY=-2MO",
     NULL},

    /* Monthly on the third-to-last day of the month */
    {"19970928T090000",
     "FREQ=MONTHLY;BYMONTHDAY=-3;COUNT=6",
     NULL},

    /* Monthly on the 2nd and 15th of the month for 10 occurrences */
    {"19970902T090000",
     "FREQ=MONTHLY;COUNT=10;BYMONTHDAY=2,15",
     NULL},

    /* Monthly on the first and last day of the month for 10 occurrences */
    {"19970930T090000",
     "FREQ=MONTHLY;COUNT=10;BYMONTHDAY=1,-1",
     NULL},

    /* Every 18 months on the 10th thru 15th of the month
       for 10 occurrences */
    {"19970910T090000",
     "FREQ=MONTHLY;INTERVAL=18;COUNT=10;BYMONTHDAY=10,11,12,13,14,15",
     NULL},

    /* Every Thuesday, every other month for 18 occurrences */
    {"19970902T090000",
     "FREQ=MONTHLY;INTERVAL=2;BYDAY=TU;COUNT=18",
     NULL},

    /* Every Friday the 13th */
    {"19970902T090000",
     "FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13;COUNT=5",
     NULL},

    /* The first Saturday that follows the first Sunday of the month */
    {"19970913T090000",
     "FREQ=MONTHLY;BYDAY=SA;BYMONTHDAY=7,8,9,10,11,12,13;COUNT=10",
     NULL},

    /* The third instance into the month of one of Tuesday, Wednesday,
       or Thursday, for the next 3 months */
    {"19970904T090000",
     "FREQ=MONTHLY;COUNT=3;BYDAY=TU,WE,TH;BYSETPOS=3",
     NULL},

    /* The second-to-last weekday of the month */
    {"19970929T090000",
     "FREQ=MONTHLY;BYDAY=MO,TU,WE,TH,FR;BYSETPOS=-2;COUNT=7",
     NULL},

    /* An example where an invalid date (i.e., February 30) is ignored */
    {"20070115T090000",
     "FREQ=MONTHLY;BYMONTHDAY=15,30;COUNT=5",
     NULL},

    /* Another example where invalid dates are ignored */
    {"20070131T090000",
     "FREQ=MONTHLY;COUNT=12",
     NULL},

    /* Weekly for 10 occurrences */
    {"19970902T090000",
     "FREQ=WEEKLY;COUNT=10",
     NULL},

    /* Weekly until December 24, 1997 */
    {"19970902T090000",
     "FREQ=WEEKLY;UNTIL=19971224T000000Z",
     NULL},

    /* Every other week */
    {"19970902T090000",
     "FREQ=WEEKLY;INTERVAL=2;WKST=SU;COUNT=13",
     NULL},

    /* Weekly on Tuesday and Thursday for five weeks */
    {"19970902T090000",
     "FREQ=WEEKLY;UNTIL=19971007T000000Z;WKST=SU;BYDAY=TU,TH",
     NULL},

    /* Weekly on Tuesday and Thursday for five weeks */
    {"19970902T090000",
     "FREQ=WEEKLY;COUNT=10;WKST=SU;BYDAY=TU,TH",
     NULL},

    /* Weekly on Sunday every four weeks */
    {"20150322T000000",
     "FREQ=WEEKLY;INTERVAL=4;BYDAY=SU;COUNT=4",
     NULL},

    /* Every other week on Monday, Wednesday, and Friday
       until December 24, 1997, starting on Monday, September 1, 1997 */
    {"19970901T090000",
     "FREQ=WEEKLY;INTERVAL=2;UNTIL=19971224T000000Z;WKST=SU;BYDAY=MO,WE,FR",
     NULL},

    /* Every other week on Tuesday and Thursday, for 8 occurrences */
    {"19970902T090000",
     "FREQ=WEEKLY;INTERVAL=2;COUNT=8;WKST=SU;BYDAY=TU,TH",
     NULL},

    /* Tuesday and Sunday every other week for 4 occurrences
       (week starts on Monday) */
    {"19970805T090000",
     "FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=MO",
     NULL},

    /* Tuesday and Sunday every other week for 4 occurrences
       (week starts on Sunday) */
    {"19970805T090000",
     "FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=SU",
     NULL},

    /* Weekly (with a trailing semicolon that was seen in the wild on Google Calendar) */
    {"20141006T090000",
     "FREQ=WEEKLY;INTERVAL=1;COUNT=2;BYDAY=MO;",
     NULL},

    /* Daily for 10 occurrences */
    {"19970902T090000",
     "FREQ=DAILY;COUNT=10",
     NULL},

    /* Daily until December 24, 1997 */
    {"19970902T090000",
     "FREQ=DAILY;UNTIL=19971224T000000Z",
     NULL},

    /* Every other day, 20 occurrences */
    {"19970902T090000",
     "FREQ=DAILY;INTERVAL=2;COUNT=20",
     NULL},

    /* Every 10 days, 5 occurrences */
    {"19970902T090000",
     "FREQ=DAILY;INTERVAL=10;COUNT=5",
     NULL},

    /* Every day in January, for 3 years */
    {"19980101T090000",
     "FREQ=DAILY;UNTIL=20000131T140000Z;BYMONTH=1",
     NULL},

    /* Every 20 minutes from 9:00 AM to 4:40 PM every day */
    {"19970902T090000",
     "FREQ=DAILY;BYHOUR=9,10,11,12,13,14,15,16;BYMINUTE=0,20,40;COUNT=48",
     NULL},

    /* Every 3 hours from 9:00 AM to 5:00 PM on a specific day */
    {"19970902T090000",
     "FREQ=HOURLY;INTERVAL=3;UNTIL=19970902T170000Z",
     NULL},

    /* Every 15 minutes for 6 occurrences */
    {"19970902T090000",
     "FREQ=MINUTELY;INTERVAL=15;COUNT=6",
     NULL},

    /* Every hour and a half for 4 occurrences */
    {"19970902T090000",
     "FREQ=MINUTELY;INTERVAL=90;COUNT=4",
     NULL},

    /* Every 20 minutes from 9:00 AM to 4:40 PM every day */
    {"19970902T090000",
     "FREQ=DAILY;BYHOUR=9,10,11,12,13,14,15,16;BYMINUTE=0,20,40;COUNT=48",
     NULL},

    /* Every week on Thursday and Sundays (35 times in total) */
    {"20111120T100000Z",
     "FREQ=WEEKLY;COUNT=35;BYDAY=SU,TH",
     NULL},

    /* github issue180 */
    {"20141030T000000Z",
     "FREQ=DAILY;UNTIL=20141206T000000Z;BYMONTH=11,12,1,2,3,4,10",
     NULL},

    /* CalendarServer test: Specific in first 2 weeks of year */
    {"20130101T000000",
     "FREQ=YEARLY;BYWEEKNO=1,2;UNTIL=20170101T000000Z",
     NULL},

    /* CalendarServer test: Last day of year in leap year */
    {"20121231T120000",
     "FREQ=YEARLY;BYYEARDAY=366;UNTIL=20200101T000000",
     NULL},

    /* CalendarServer test: Last Friday in October */
    {"20101029T120000",
     "FREQ=YEARLY;BYDAY=-1FR;BYMONTH=10;UNTIL=20150101T000000Z",
     NULL},

    /* CalendarServer test: First Friday in April */
    {"20100402T120000",
     "FREQ=YEARLY;BYDAY=1FR;BYMONTH=4;UNTIL=20150101T000000Z",
     NULL},

    /* Test monthly with largest day of month */
    {"20150131T000000Z",
     "FREQ=MONTHLY;BYMONTHDAY=31;COUNT=12",
     NULL},

    /* Test monthly with -largest day of month */
    {"20150101T000000Z",
     "FREQ=MONTHLY;BYMONTHDAY=-31;COUNT=12",
     NULL},

    /* Test yearly with largest day of year */
    {"20121231T120000",
     "FREQ=YEARLY;BYYEARDAY=366;COUNT=3",
     NULL},

    /* Test yearly with -largest day of year */
    {"20120101T120000",
     "FREQ=YEARLY;BYYEARDAY=-366;COUNT=3",
     NULL},

    /* Every Wednesday in February, every other year, for 5 years,
       starting at January 1, 2020 */
    {"20170201T090000",
     "FREQ=YEARLY;INTERVAL=2;UNTIL=20211231T140000Z;BYMONTH=2;BYDAY=WE",
     "20200101T100000"},

    /* Monthly on the first Friday until December 29, 2017,
       starting September 15, 2017 */
    {"20170901T090000",
     "FREQ=MONTHLY;UNTIL=20171229T000000Z;BYDAY=1FR",
     "20170915T090000"},

    /* Bi-weekly on Sunday, Tuesday, and Thursday for five weeks,
       starting last day of 2016 */
    {"20161229T090000",
     "FREQ=WEEKLY;UNTIL=20170127T000000Z;WKST=MO;BYDAY=SU,TU,TH;INTERVAL=2",
     "20161231T090000"},

    /* Every 3rd day in January, starting on the 10th */
    {"20170101T090000",
     "FREQ=DAILY;UNTIL=20170131T140000Z;BYMONTH=1;INTERVAL=3",
     "20170103T090000"},

    /* Every 3 hours from 9:00 AM to 5:00 PM over 2 days,
       starting at 11PM on September 2 */
    {"20170902T090000",
     "FREQ=HOURLY;INTERVAL=3;UNTIL=20170903T170000Z",
     "20170902T230000"},

    /* Every 13 months, on May 12 */
    {"20100212T000000",
     "FREQ=MONTHLY;INTERVAL=13;BYMONTH=5;COUNT=3",
     NULL},

    {NULL, NULL, NULL}
};

const struct recur rscale[] = {

    /* Bad RSCALE */
    {"20131025",
     "RSCALE=RUSSIAN;FREQ=YEARLY",
     NULL},

    /* Ethiopic last day of year */
    {"20140910",
     "RSCALE=ETHIOPIC;FREQ=YEARLY;BYMONTH=13;BYMONTHDAY=-1;COUNT=6",
     NULL},

    /* Chinese New Year */
    {"20130210",
     "RSCALE=CHINESE;FREQ=YEARLY;UNTIL=20180101",
     NULL},

    /* Chinese monthly */
    {"20140920",
     "RSCALE=CHINESE;FREQ=MONTHLY;COUNT=4",
     NULL},

    /* Islamic monthly */
    {"20131025",
     "RSCALE=ISLAMIC-CIVIL;FREQ=MONTHLY;COUNT=4",
     NULL},

    /* Islamic Ramadan */
    {"20130709",
     "RSCALE=ISLAMIC-CIVIL;FREQ=YEARLY;BYMONTH=9;COUNT=5",
     NULL},
#if defined(HAVE_ICU_DANGI)
    /* Korean Buddha birthday */
    {"20131025",
     "RSCALE=DANGI;FREQ=YEARLY;BYMONTHDAY=8;BYMONTH=4;UNTIL=20160101",
     NULL},
#endif
    /* Chinese leap month (no leap) */
    {"20131025",
     "RSCALE=CHINESE;FREQ=YEARLY;BYMONTHDAY=10;BYMONTH=9;COUNT=3",
     NULL},

    /* Chinese leap month (omit) */
    {"20131025",
     "RSCALE=CHINESE;FREQ=YEARLY;BYMONTHDAY=10;BYMONTH=9L;SKIP=OMIT;COUNT=2",
     NULL},

    {"20131025",
     "RSCALE=CHINESE;FREQ=YEARLY;BYMONTHDAY=10;BYMONTH=4L;SKIP=OMIT;UNTIL=21000101",
     NULL},

    /* Chinese leap month (skip forward) */
    {"20131025",
     "RSCALE=CHINESE;FREQ=YEARLY;BYMONTHDAY=10;BYMONTH=9L;SKIP=FORWARD;COUNT=3",
     NULL},

    /* Chinese leap month (skip back) */
    {"20131025",
     "RSCALE=CHINESE;FREQ=YEARLY;BYMONTHDAY=10;BYMONTH=9L;SKIP=BACKWARD;COUNT=3",
     NULL},

    /* Chinese leap month (both) */
    {"20131025",
     "RSCALE=CHINESE;FREQ=YEARLY;BYMONTHDAY=10;BYMONTH=9,9L;SKIP=BACKWARD;COUNT=4",
     NULL},

    /* Jewish leap month (omit) */
    {"20140205",
     "RSCALE=HEBREW;FREQ=YEARLY;COUNT=4",
     NULL},

    /* Jewish leap month (skip forward) */
    {"20140205",
     "RSCALE=HEBREW;FREQ=YEARLY;SKIP=FORWARD;COUNT=4",
     NULL},

    /* Jewish leap month (skip forward by month) */
    {"20140208",
     "RSCALE=HEBREW;FREQ=YEARLY;BYMONTH=5L;BYMONTHDAY=8;SKIP=FORWARD;COUNT=5",
     NULL},

    /* 30th day of Jewish leap month (skip forward) */
    {"20140302",
     "RSCALE=HEBREW;FREQ=YEARLY;BYMONTH=5L;BYMONTHDAY=30;SKIP=FORWARD;COUNT=5",
     NULL},

    /* Last day of Jewish leap month (skip forward) */
    {"20140302",
     "RSCALE=HEBREW;FREQ=YEARLY;BYMONTH=5L;BYMONTHDAY=-1;SKIP=FORWARD;COUNT=5",
     NULL},

    /* 30th day of Jewish leap month (skip backward) */
    {"20140302",
     "RSCALE=HEBREW;FREQ=YEARLY;BYMONTH=5L;BYMONTHDAY=30;SKIP=BACKWARD;COUNT=5",
     NULL},

    /* Last day of Jewish leap month (skip backward) */
    {"20140302",
     "RSCALE=HEBREW;FREQ=YEARLY;BYMONTH=5L;BYMONTHDAY=-1;SKIP=BACKWARD;COUNT=5",
     NULL},

    /* Gregorian leap day (omit) */
    {"20120229",
     "RSCALE=GREGORIAN;FREQ=YEARLY;COUNT=4",
     NULL},

    /* Gregorian (skip back) */
    {"20140131",
     "RSCALE=GREGORIAN;FREQ=MONTHLY;SKIP=BACKWARD;COUNT=4",
     NULL},

    /* Gregorian (skip forward) */
    {"20140131",
     "RSCALE=GREGORIAN;FREQ=MONTHLY;SKIP=FORWARD;COUNT=4",
     NULL},

    /* Gregorian (skip forward both) */
    {"20150201",
     "RSCALE=GREGORIAN;FREQ=YEARLY;BYMONTH=2;BYMONTHDAY=28,29;SKIP=FORWARD;COUNT=5",
     NULL},

    /* Gregorian (skip forward interval) */
    {"20140131",
     "RSCALE=GREGORIAN;FREQ=MONTHLY;INTERVAL=3;SKIP=FORWARD;COUNT=4",
     NULL},

    /* Test monthly with largest day of month */
    {"20150131T000000Z",
     "RSCALE=GREGORIAN;FREQ=MONTHLY;BYMONTHDAY=31;COUNT=12;SKIP=FORWARD",
     NULL},

    /* Test monthly with -largest day of month */
    {"20150101T000000Z",
     "RSCALE=GREGORIAN;FREQ=MONTHLY;BYMONTHDAY=-31;COUNT=12;SKIP=FORWARD",
     NULL},

    /* Test monthly with largest day of month */
    {"20150131T000000Z",
     "RSCALE=GREGORIAN;FREQ=MONTHLY;BYMONTHDAY=31;COUNT=12;SKIP=BACKWARD",
     NULL},

    /* Test monthly with -largest day of month */
    {"20150101T000000Z",
     "RSCALE=GREGORIAN;FREQ=MONTHLY;BYMONTHDAY=-31;COUNT=12;SKIP=BACKWARD",
     NULL},

    /* Test yearly with largest day of year */
    {"20121231T120000",
     "RSCALE=GREGORIAN;FREQ=YEARLY;BYYEARDAY=366;SKIP=FORWARD;COUNT=9",
     NULL},

    /* Test yearly with largest day of year */
    {"20121231T120000",
     "RSCALE=GREGORIAN;FREQ=YEARLY;BYYEARDAY=366;SKIP=BACKWARD;COUNT=9",
     NULL},

    /* Test yearly with -largest day of year */
    {"20120101T120000",
     "RSCALE=GREGORIAN;FREQ=YEARLY;BYYEARDAY=-366;SKIP=FORWARD;COUNT=9",
     NULL},

    /* Test yearly with -largest day of year */
    {"20120101T120000",
     "RSCALE=GREGORIAN;FREQ=YEARLY;BYYEARDAY=-366;SKIP=BACKWARD;COUNT=9",
     NULL},

    /* Every other Chinese New Year, starting at Jan 1, 2016 */
    {"20130210",
     "RSCALE=CHINESE;FREQ=YEARLY;UNTIL=20200101;INTERVAL=2",
     "20160101"},

    {NULL, NULL, NULL}
};

int main(int argc, char *argv[])
{
    int opt;

    /* Default to RFC 5545 tests */
    const struct recur *r = rfc5545;
    FILE *fp = fopen("test.out", "w");
    int verbose = 0;

    if (fp == NULL) {
        fprintf(stderr, "unable to open the output file test.out\n");
        return (1);
    }

    while ((opt = getopt(argc, argv, "rv")) != EOF) {
        switch (opt) {
#if defined(HAVE_LIBICU)
        case 'r':      /* Do RSCALE tests */
            if (!icalrecurrencetype_rscale_is_supported()) {
                fprintf(stderr, "error: RSCALE not supported\n");
                fclose(fp);
                return (1);
            }
            r = rscale;
            break;
#endif

        case 'v':      /* Verbose output to stdout */
            verbose = 1;
            break;

        default:
            fprintf(stderr, "usage: %s [-r]\n", argv[0]);
            fclose(fp);
            return (1);
            break;
        }
    }

    for (; r->dtstart; r++) {
        struct icalrecurrencetype rrule;
        struct icaltimetype dtstart, next;
        icalrecur_iterator *ritr;
        const char *sep = "";

        fprintf(fp, "\nRRULE:%s\n", r->rrule);
        fprintf(fp, "DTSTART:%s\n", r->dtstart);
        if (r->start_at) fprintf(fp, "START-AT:%s\n", r->start_at);
        fprintf(fp, "INSTANCES:");

        if (verbose) {
            printf("Processing %s\n", r->rrule);
        }

        dtstart = icaltime_from_string(r->dtstart);
        rrule = icalrecurrencetype_from_string(r->rrule);
        ritr = icalrecur_iterator_new(rrule, dtstart);

        if (!ritr) {
            fprintf(fp, " *** %s\n", icalerror_strerror(icalerrno));
        } else {
            if (r->start_at) {
                struct icaltimetype start = icaltime_from_string(r->start_at);
                icalrecur_iterator_set_start(ritr, start);
            }

            for (next = icalrecur_iterator_next(ritr);
                 !icaltime_is_null_time(next);
                 next = icalrecur_iterator_next(ritr)) {

                fprintf(fp, "%s%s", sep, icaltime_as_ical_string(next));
                sep = ",";
            }
            fprintf(fp, "\n");
        }

        icalrecur_iterator_free(ritr);
        free(rrule.rscale);
    }
    fclose(fp);

    return (0);
}
