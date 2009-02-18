/* -*- Mode: C -*-
  ======================================================================

  The contents of this file are subject to the Mozilla Public License
  Version 1.0 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License at
  http://www.mozilla.org/MPL/
 
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
  the License for the specific language governing rights and
  limitations under the License.

  ======================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <libical/ical.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    icalarray *timezones = icaltimezone_get_builtin_timezones();
    int i;
    int ret = 0;
    unsigned int total_failed = 0;
    unsigned int total_okay = 0;
    int verbose = 0;
    icaltimezone *utc_zone = icaltimezone_get_utc_timezone();

    /* for all known time zones... */
    for (i = 0; i < timezones->num_elements; i++) {
        icaltimezone *zone = icalarray_element_at(timezones, i);
        const char *zone_location = icaltimezone_get_location(zone);
        int day;
        time_t start_time;
        struct tm start_tm;
        time_t curr_time;
        struct tm curr_tm;
        struct icaltimetype curr_tt;
        int failed = 0;
        int curr_failed;

        /*
         * select this location for glibc: needs support for TZ=<location>
         * which is not POSIX
         */
        setenv("TZ", zone_location, 1);
        tzset();

        /*
         * determine current local time and date: always use midday in
         * the current zone and first day of first month in the year
         */
        start_time = time(NULL);
        localtime_r(&start_time, &start_tm);
        start_tm.tm_hour = 12;
        start_tm.tm_min = 0;
        start_tm.tm_sec = 0;
	start_tm.tm_mday = 1;
	start_tm.tm_mon = 0;
        start_time = mktime(&start_tm);

        /* check time conversion for the next 365 days */
        for (day = 0, curr_time = start_time;
             day < 365;
             day++, curr_time += 24 * 60 * 60) {
            /* determine date/time with glibc */
            localtime_r(&curr_time, &curr_tm);
            /* determine date/time with libical */
            curr_tt = icaltime_from_timet_with_zone(curr_time, 0, utc_zone);
            curr_tt.zone = utc_zone; /* workaround: icaltime_from_timet_with_zone() should do this, but doesn't! */
            curr_tt = icaltime_convert_to_zone(curr_tt, zone);

            /* compare... */
            curr_failed = 
                curr_tm.tm_year + 1900 != curr_tt.year ||
                curr_tm.tm_mon + 1 != curr_tt.month ||
                curr_tm.tm_mday != curr_tt.day ||
                curr_tm.tm_hour != curr_tt.hour ||
                curr_tm.tm_min != curr_tt.minute ||
                curr_tm.tm_sec != curr_tt.second;

            /* only print first failed day and first day which is okay again */
            if (verbose || curr_failed != failed) {
                printf("%s: day %03d: %s: libc %04d-%02d-%02d %02d:%02d:%02d dst %d",
                       zone_location,
                       day,
                       verbose ?
                       (curr_failed ? "failed" : "okay") :
                       (curr_failed ? "first failed" : "okay again"),

                       curr_tm.tm_year + 1900,
                       curr_tm.tm_mon + 1,
                       curr_tm.tm_mday,
                       curr_tm.tm_hour,
                       curr_tm.tm_min,
                       curr_tm.tm_sec,
		       curr_tm.tm_isdst);
                if (curr_failed) {
                    printf(" != libical %04d-%02d-%02d %02d:%02d:%02d dst %d",
                           curr_tt.year,
                           curr_tt.month,
                           curr_tt.day,
                           curr_tt.hour,
                           curr_tt.minute,
                           curr_tt.second,
			   curr_tt.is_daylight);
                    ret = 1;
                }
                printf("\n");
                failed = curr_failed;
            }

            if (curr_failed) {
                total_failed++;
            } else {
                total_okay++;
            }
        }
    }

    if (total_failed || total_okay) {
        printf(" *** Summary: %d zones tested, %u days failed, %u okay => %u%% failed ***\n",
               timezones->num_elements,
               total_failed,
               total_okay,
               total_failed * 100 / (total_failed + total_okay));
    }

    return ret;
}
