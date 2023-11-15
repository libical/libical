#ifndef VCARDTIME_H
#define VCARDTIME_H

#include "libical_vcard_export.h"

typedef struct vcardtimetype
{
    int year;           /* 0000-9999 */
    int month;          /* 01 (Jan) to 12 (Dec). */
    int day;            /* 01-28/29/30/31 depending on month and leap year */
    int hour;           /* 00-23 */
    int minute;         /* 00-59 */
    int second;         /* 00-58/59/60 depending on leap second */
    int utcoffset;      /* -720 to +840 minutes (-12:00 to +14:00) */
} vcardtimetype;

#define vcardtime_null_date()       vcardtime_null_datetime()
#define vcardtime_null_time()       vcardtime_null_datetime()
#define vcardtime_null_timestamp()  vcardtime_null_datetime()

LIBICAL_VCARD_EXPORT vcardtimetype vcardtime_null_datetime(void);
LIBICAL_VCARD_EXPORT vcardtimetype vcardtime_current_utc_time(void);

LIBICAL_VCARD_EXPORT int vcardtime_is_null_datetime(const vcardtimetype t);
LIBICAL_VCARD_EXPORT int vcardtime_is_time(const vcardtimetype t);
LIBICAL_VCARD_EXPORT int vcardtime_is_date(const vcardtimetype t);
LIBICAL_VCARD_EXPORT int vcardtime_is_datetime(const vcardtimetype t);
LIBICAL_VCARD_EXPORT int vcardtime_is_timestamp(const vcardtimetype t);
LIBICAL_VCARD_EXPORT int vcardtime_is_utc(const vcardtimetype t);
LIBICAL_VCARD_EXPORT int vcardtime_is_leap_year(const int year);
LIBICAL_VCARD_EXPORT int vcardtime_is_valid_time(const struct vcardtimetype t);

LIBICAL_VCARD_EXPORT const char *vcardtime_as_vcard_string(const vcardtimetype t,
                                                           /* 1 for DATEANDORTIME */
                                                           int need_time_designator);
LIBICAL_VCARD_EXPORT char *vcardtime_as_vcard_string_r(const vcardtimetype t,
                                                       /* 1 for DATEANDORTIME */
                                                       int need_time_designator);

LIBICAL_VCARD_EXPORT vcardtimetype vcardtime_from_string(const char *str,
                                                         int is_bare_time);

#endif /* VCARDTIME_H */
