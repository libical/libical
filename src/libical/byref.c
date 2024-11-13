#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalcomponent.h"
#include "icaltime.h"
#include "astime.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icaltimezone.h"

#include <ctype.h>
#include <stdlib.h>

LIBICAL_ICAL_EXPORT void icaltime_current_time_with_zone_ex(const icaltimezone *zone, struct icaltimetype* ret) {
    *ret = icaltime_current_time_with_zone(zone);
}