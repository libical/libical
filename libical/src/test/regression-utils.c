#include "ical.h"

static char ictt_str[1024];

const char* ical_timet_string(const time_t t)
{
    struct tm stm = *(gmtime(&t));    

    sprintf(ictt_str,"%02d-%02d-%02d %02d:%02d:%02d Z",stm.tm_year+1900,
	    stm.tm_mon+1,stm.tm_mday,stm.tm_hour,stm.tm_min,stm.tm_sec);

    return ictt_str;
    
}

const char* ictt_as_string(struct icaltimetype t)
{
    sprintf(ictt_str,"%02d-%02d-%02d %02d:%02d:%02d%s %s",t.year,t.month,t.day,
	    t.hour,t.minute,t.second,t.is_utc?" Z":"",
	icaltimezone_get_tzid((icaltimezone *)t.zone));	/* HACK */

    return ictt_str;
}

char* icaltime_as_ctime(struct icaltimetype t)
{
    time_t tt;
 
    tt = icaltime_as_timet(t);
    sprintf(ictt_str,"%s",ctime(&tt));

    return ictt_str;
}
