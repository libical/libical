/*
 * Program for testing libical recurrence iterator on RFC 5545 examples.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libical/ical.h>
#if defined(HAVE_LIBICU)
#include <unicode/ucal.h>
#endif

struct recur {
    const char *dtstart;
    const char *rrule;
};


const struct recur rfc5545[] = {

    /* Every day in January, for 3 years */
    { "19980101T090000",
      "FREQ=YEARLY;UNTIL=20000131T140000Z;BYMONTH=1;BYDAY=SU.MO,TU.WE.TH.FR.SA" },

    /* Yearly in June and July for 10 occurrences */
    { "19970610T090000",
      "FREQ=YEARLY;COUNT=10;BYMONTH=6,7" },

    /* Every other year on January, February, and March for 10 occurrences */
    { "19970310T090000",
      "FREQ=YEARLY;INTERVAL=2;COUNT=10;BYMONTH=1,2,3" },

    /* Every third year on the 1st, 100th, and 200th day for 10 occurrences */
    { "19970101T090000",
      "FREQ=YEARLY;INTERVAL=3;COUNT=10;BYYEARDAY=1,100,200" },

    /* Every 20th Monday of the year */
    { "19970519T090000",
      "FREQ=YEARLY;BYDAY=20MO;COUNT=3" },

    /* Monday of week number 20
       (where the default start of the week is Monday) */
    { "19970512T090000",
      "FREQ=YEARLY;BYWEEKNO=20;BYDAY=MO;COUNT=3" },

    /* Monday of week number 20
       (where the start of the week is Sunday) */
    { "19970512T090000",
      "FREQ=YEARLY;BYWEEKNO=20;BYDAY=MO;WKST=SU;COUNT=3" },

    /* Monday of week number 20
       (where the start of the week is Friday) */
    { "19970512T090000",
      "FREQ=YEARLY;BYWEEKNO=20;BYDAY=MO;WKST=FR;COUNT=3" },

    /* Every Thursday in March */
    { "19970313T090000",
      "FREQ=YEARLY;BYMONTH=3;BYDAY=TH;COUNT=11" },

    /* Every Thursday, but only during June, July, and August */
    { "19970605T090000",
      "FREQ=YEARLY;BYDAY=TH;BYMONTH=6,7,8;COUNT=39" },

    /* Every 4 years, the first Thuesday after a Monday in November
       (U.S. Presidential Election Day) */
    { "19961105T090000",
      "FREQ=YEARLY;INTERVAL=4;BYMONTH=11;BYDAY=TU;BYMONTHDAY=2,3,4,5,6,7,8;COUNT=3" },

    /* Friday in March occurring on or after the 26th for 5 years */
    { "20060331T020000",
      "FREQ=YEARLY;UNTIL=20100326T000000Z;BYDAY=FR;BYYEARDAY=-275,-276,-277,-278,-279,-280,-281" },

    /* Example where leap day gets skipped */
    { "20120229T120000Z",
      "FREQ=YEARLY;UNTIL=20140301T115959Z" },


    /* Monthly on the first Friday for 10 occurrences */
    { "19970905T090000",
      "FREQ=MONTHLY;COUNT=10;BYDAY=1FR" },

    /* Monthly on the first Friday until December 24, 1997 */
    { "19970905T090000",
      "FREQ=MONTHLY;UNTIL=19971224T000000Z;BYDAY=1FR" },

    /* Every other month on the first and last Sunday of the month
       for 10 occurrences */
    { "19970907T090000",
      "FREQ=MONTHLY;INTERVAL=2;COUNT=10;BYDAY=1SU,-1SU" },

    /* Monthly on the second-to-last Monday of the month for 6 months */
    { "19970922T090000",
      "FREQ=MONTHLY;COUNT=6;BYDAY=-2MO" },

    /* Monthly on the third-to-last day of the month */
    { "19970928T090000",
      "FREQ=MONTHLY;BYMONTHDAY=-3;COUNT=6" },

    /* Monthly on the 2nd and 15th of the month for 10 occurrences */
    { "19970902T090000",
      "FREQ=MONTHLY;COUNT=10;BYMONTHDAY=2,15" },

    /* Monthly on the first and last day of the month for 10 occurrences */
    { "19970930T090000",
      "FREQ=MONTHLY;COUNT=10;BYMONTHDAY=1,-1" },

    /* Every 18 months on the 10th thru 15th of the month
       for 10 occurrences */
    { "19970910T090000",
      "FREQ=MONTHLY;INTERVAL=18;COUNT=10;BYMONTHDAY=10,11,12,13,14,15" },

    /* Every Thuesday, every other month for 18 occurrences */
    { "19970902T090000",
      "FREQ=MONTHLY;INTERVAL=2;BYDAY=TU;COUNT=18" },

    /* Every Friday the 13th */
    { "19970902T090000",
      "FREQ=MONTHLY;BYDAY=FR;BYMONTHDAY=13;COUNT=5" },

    /* The first Saturday that follows the first Sunday of the month */
    { "19970913T090000",
      "FREQ=MONTHLY;BYDAY=SA;BYMONTHDAY=7,8,9,10,11,12,13;COUNT=10" },

    /* The third instance into the month of one of Tuesday, Wednesday,
       or Thursday, for the next 3 months */
    { "19970904T090000",
      "FREQ=MONTHLY;COUNT=3;BYDAY=TU,WE,TH;BYSETPOS=3" },

    /* The second-to-last weekday of the month */
    { "19970929T090000",
      "FREQ=MONTHLY;BYDAY=MO,TU,WE,TH,FR;BYSETPOS=-2;COUNT=7" },

    /* An example where an invalid date (i.e., February 30) is ignored */
    { "20070115T090000",
      "FREQ=MONTHLY;BYMONTHDAY=15,30;COUNT=5" },

    /* Another example where invalid dates are ignored */
    { "20070131T090000",
      "FREQ=MONTHLY;COUNT=12" },


    /* Weekly for 10 occurrences */
    { "19970902T090000",
      "FREQ=WEEKLY;COUNT=10" },

    /* Weekly until December 24, 1997 */
    { "19970902T090000",
      "FREQ=WEEKLY;UNTIL=19971224T000000Z" },

    /* Every other week */
    { "19970902T090000",
      "FREQ=WEEKLY;INTERVAL=2;WKST=SU;COUNT=13" },

    /* Weekly on Tuesday and Thursday for five weeks */
    { "19970902T090000",
      "FREQ=WEEKLY;UNTIL=19971007T000000Z;WKST=SU;BYDAY=TU,TH" },

    /* Weekly on Tuesday and Thursday for five weeks */
    { "19970902T090000",
      "FREQ=WEEKLY;COUNT=10;WKST=SU;BYDAY=TU,TH" },

    /* Every other week on Monday, Wednesday, and Friday 
       until December 24, 1997, starting on Monday, September 1, 1997 */
    { "19970901T090000",
      "FREQ=WEEKLY;INTERVAL=2;UNTIL=19971224T000000Z;WKST=SU;BYDAY=MO,WE,FR" },

    /* Every other week on Tuesday and Thursday, for 8 occurrences */
    { "19970902T090000",
      "FREQ=WEEKLY;INTERVAL=2;COUNT=8;WKST=SU;BYDAY=TU,TH" },

    /* Tuesday and Sunday every other week for 4 occurrences
       (week starts on Monday) */
    { "19970805T090000",
      "FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=MO" },

    /* Tuesday and Sunday every other week for 4 occurrences
       (week starts on Sunday) */
    { "19970805T090000",
      "FREQ=WEEKLY;INTERVAL=2;COUNT=4;BYDAY=TU,SU;WKST=SU" },


    /* Daily for 10 occurrences */
    { "19970902T090000",
      "FREQ=DAILY;COUNT=10" },

    /* Daily until December 24, 1997 */
    { "19970902T090000",
      "FREQ=DAILY;UNTIL=19971224T000000Z" },

    /* Every other day, 20 occurrences */
    { "19970902T090000",
      "FREQ=DAILY;INTERVAL=2;COUNT=20" },

    /* Every 10 days, 5 occurrences */
    { "19970902T090000",
      "FREQ=DAILY;INTERVAL=10;COUNT=5" },

    /* Every day in January, for 3 years */
    { "19980101T090000",
      "FREQ=DAILY;UNTIL=20000131T140000Z;BYMONTH=1" },

    /* Every 20 minutes from 9:00 AM to 4:40 PM every day */
    { "19970902T090000",
      "FREQ=DAILY;BYHOUR=9,10,11,12,13,14,15,16;BYMINUTE=0,20,40;COUNT=48" },


    /* Every 3 hours from 9:00 AM to 5:00 PM on a specific day */
    { "19970902T090000",
      "FREQ=HOURLY;INTERVAL=3;UNTIL=19970902T170000Z" },


    /* Every 15 minutes for 6 occurrences */
    { "19970902T090000",
      "FREQ=MINUTELY;INTERVAL=15;COUNT=6" },

    /* Every hour and a half for 4 occurrences */
    { "19970902T090000",
      "FREQ=MINUTELY;INTERVAL=90;COUNT=4" },

    /* Every 20 minutes from 9:00 AM to 4:40 PM every day */
    { "19970902T090000",
      "FREQ=DAILY;BYHOUR=9,10,11,12,13,14,15,16;BYMINUTE=0,20,40;COUNT=48" },

    { NULL, NULL }
};


const struct recur rscale[] = {

    /* Ethiopic last day of year */
    { "20140910",
      "RSCALE=ETHIOPIC;FREQ=YEARLY;BYMONTH=13;BYMONTHDAY=-1;COUNT=6" },

    /* Chinese New Year */
    { "20130210",
      "RSCALE=CHINESE;FREQ=YEARLY;UNTIL=20180101" },

    /* Chinese monthly */
    { "20140920",
      "RSCALE=CHINESE;FREQ=MONTHLY;COUNT=4" },

    /* Islamic monthly */
    { "20131025",
      "RSCALE=ISLAMIC-CIVIL;FREQ=MONTHLY;COUNT=4" },

    /* Islamic Ramadan */
    { "20130709",
      "RSCALE=ISLAMIC-CIVIL;FREQ=YEARLY;BYMONTH=9;COUNT=5" },

    /* Korean Buddha birthday */
    { "20131025",
      "RSCALE=DANGI;FREQ=DAILY;BYMONTHDAY=8;BYMONTH=4;UNTIL=20160101" },

    /* Chinese leap month (no leap) */
    { "20131025",
      "RSCALE=CHINESE;FREQ=DAILY;BYMONTHDAY=10;BYMONTH=9;COUNT=3" },

    /* Chinese leap month (skip yes) */
    { "20131025",
      "RSCALE=CHINESE;FREQ=DAILY;BYMONTHDAY=10;BYMONTH=9L;SKIP=YES;COUNT=2" },

    { "20131025",
      "RSCALE=CHINESE;FREQ=DAILY;BYMONTHDAY=10;BYMONTH=4L;SKIP=YES;UNTIL=21000101" },

    /* Chinese leap month (skip back) */
    { "20131025",
      "RSCALE=CHINESE;FREQ=DAILY;BYMONTHDAY=10;BYMONTH=9L;COUNT=3" },

    /* Chinese leap month (both) */
    { "20131025",
      "RSCALE=CHINESE;FREQ=DAILY;BYMONTHDAY=10;BYMONTH=9,9L;COUNT=4" },

    /* Jewish leap month (skip yes) */
    { "20140205",
      "RSCALE=HEBREW;FREQ=YEARLY;SKIP=YES;COUNT=4" },

    /* Jewish leap month (skip forward) */
    { "20140205",
      "RSCALE=HEBREW;FREQ=YEARLY;SKIP=FORWARD;COUNT=4" },

    /* Jewish leap month (skip forward by month) */
    { "20140208",
      "RSCALE=HEBREW;FREQ=YEARLY;BYMONTH=5L;BYMONTHDAY=8;SKIP=FORWARD;COUNT=5" },

    /* Gregorian (skip back) */
    { "20140131",
      "RSCALE=GREGORIAN;FREQ=MONTHLY;COUNT=4" },

    /* Gregorian (skip forward) */
    { "20140131",
      "RSCALE=GREGORIAN;FREQ=MONTHLY;SKIP=FORWARD;COUNT=4" },

    /* Gregorian (skip forward both) */
    { "20150201",
      "RSCALE=GREGORIAN;FREQ=YEARLY;BYMONTH=2;BYMONTHDAY=28,29;SKIP=FORWARD;COUNT=5" },

    /* Gregorian (skip forward interval) */
    { "20140131",
      "RSCALE=GREGORIAN;FREQ=MONTHLY;INTERVAL=3;SKIP=FORWARD;COUNT=4" },

    { NULL, NULL }
};



int main(int argc, char *argv[])
{
    /* Default to RFC 5545 tests */
    const struct recur *r = rfc5545;
    FILE *fp = fopen("test.out","w");
    if(fp == NULL) {
	fprintf(stderr, "unable to open the output file test.out\n");
	return(1);
    }

    int opt;
    while ((opt = getopt(argc, argv, "r")) != EOF) {
	switch (opt) {
#if defined(HAVE_LIBICU)
	case 'r': /* Do RSCALE tests */
	    if (!icalrecurrencetype_rscale_is_supported()) {
		fprintf(stderr, "error: RSCALE not supported\n");
		return(1);
	    }
	    r = rscale;
	    break;
#endif

	default:
	    fprintf(stderr, "usage: %s [-r]\n", argv[0]);
	    return(1);
	    break;
	}
    }

    for (; r->dtstart; r++) {
	struct icalrecurrencetype rrule;
	struct icaltimetype dtstart, next;
	icalrecur_iterator *ritr;
	const char *sep = "";

	fprintf(fp,"\nRRULE:%s\n", r->rrule);
	fprintf(fp,"DTSTART:%s\n", r->dtstart);
	fprintf(fp,"INSTANCES:");

	dtstart = icaltime_from_string(r->dtstart);
	rrule = icalrecurrencetype_from_string(r->rrule);
	ritr = icalrecur_iterator_new(rrule, dtstart);

	for (next = icalrecur_iterator_next(ritr);
	     !icaltime_is_null_time(next);
	     next = icalrecur_iterator_next(ritr)) {

	    fprintf(fp,"%s%s", sep, icaltime_as_ical_string(next));
	    sep = ",";
	}
	fprintf(fp,"\n");

	icalrecur_iterator_free(ritr);
    }
    fclose(fp);

    return(0);
}
