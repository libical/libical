/* -*- Mode: C -*-
  ======================================================================
  FILE: icaltime.c
  CREATOR: eric 02 June 2000
  
  $Id: icaltime.c,v 1.1.1.1 2001-01-02 07:33:02 ebusboom Exp $
  $Locker:  $
    
 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom


 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icaltime.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef ICAL_NO_LIBICAL
#define icalerror_set_errno(x)
#define  icalerror_check_arg_rv(x,y)
#define  icalerror_check_arg_re(x,y,z)
#else
#include "icalerror.h"
#endif

struct icaltimetype 
icaltime_from_timet(time_t tm, int is_date)
{
    struct icaltimetype tt = icaltime_null_time();
    struct tm t;

    t = *(gmtime(&tm));
     
    if(is_date == 0){ 
	tt.second = t.tm_sec;
	tt.minute = t.tm_min;
	tt.hour = t.tm_hour;
    } else {
	tt.second = tt.minute =tt.hour = 0 ;
    }

    tt.day = t.tm_mday;
    tt.month = t.tm_mon + 1;
    tt.year = t.tm_year+ 1900;
    
    tt.is_utc = 1;
    tt.is_date = is_date; 

    return tt;
}

char* set_tz(const char* tzid)
{
    char *tzstr = 0;
    char *tmp;

   /* Put the new time zone into the environment */
    if(getenv("TZ") != 0){
	tzstr = (char*)strdup(getenv("TZ"));

	if(tzstr == 0){
	    icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	    return 0;
	}
    }

    tmp = (char*)malloc(1024);

    if(tmp == 0){
	icalerror_set_errno(ICAL_NEWFAILED_ERROR);
	return 0;
    }

    snprintf(tmp,1024,"TZ=%s",tzid);

    /* HACK. In some libc versions, putenv gives the string to the
       system and in some it gives a copy, so the following might be a
       memory leak. THe linux man page says that glibc2.1.2 take
       ownership ( no leak) while BSD4.4 uses a copy ( A leak ) */
    putenv(tmp); 

    return tzstr; /* This will be zero if the TZ env var was not set */
}

void unset_tz(char* tzstr)
{
    /* restore the original environment */

    if(tzstr!=0){
	putenv(tzstr);
	free(tzstr);
    } else {
	putenv("TZ"); /* Delete from environment */
    } 
}

time_t icaltime_as_timet(struct icaltimetype tt)
{
    struct tm stm;


    memset(&stm,0,sizeof( struct tm));

    stm.tm_sec = tt.second;
    stm.tm_min = tt.minute;
    stm.tm_hour = tt.hour;
    stm.tm_mday = tt.day;
    stm.tm_mon = tt.month-1;
    stm.tm_year = tt.year-1900;
    stm.tm_isdst = 0;

    if(tt.is_utc == 1 || tt.is_date == 1){
	/* It would be nice to use set_tz("UTC") instead of this code
           to make mktime operate in UTC, but that would conflict with
           the use of set_tz in icaltime_utc_offset */
        stm.tm_sec -= icaltime_local_utc_offset();
    }

    return mktime(&stm);
}



/* convert tt, of timezone tzid, into a utc time */
struct icaltimetype icaltime_as_utc(struct icaltimetype tt,const char* tzid)
{
    int tzid_offset;

    if(tt.is_utc == 1 || tt.is_date == 1){
	return tt;
    }

    tzid_offset = icaltime_utc_offset(tt,tzid);

    tt.second += tzid_offset;

    tt.is_utc = 1;

    return icaltime_normalize(tt);
}

/* convert tt, a time in UTC, into a time in timezone tzid */
struct icaltimetype icaltime_as_zone(struct icaltimetype tt,const char* tzid)
{
    int tzid_offset;

    tzid_offset = icaltime_utc_offset(tt,tzid);

    tt.second -= tzid_offset;

    tt.is_utc = 0;

    return icaltime_normalize(tt);

}

/* Return the daylight savings offset for this zone for the given time */
int icaltime_daylight_offset(struct icaltimetype tt, const char* tzid)
{
    time_t t = icaltime_as_timet(tt);
    time_t t_l, offset;
 
    struct tm stm;

    char* old_tz = set_tz(tzid);

    stm = *(localtime(&t)); /* This sets 'timezone' */
    offset = timezone;

    stm = *(gmtime(&t)); 

    stm.tm_sec -= offset; /* Convert to local time */

    stm.tm_isdst = -1; /* Don't adjust time for daylight savings */

    t_l = mktime(&stm);


    unset_tz(old_tz);
    
    return (t - t_l);

}

int icaltime_local_daylight_offset()
{

    time_t t = time(0);
    time_t t_l, offset;

    struct tm stm;

    stm = *(localtime(&t)); /* This sets 'timezone' */
    offset = timezone;

    stm = *(gmtime(&t)); 

    stm.tm_sec -= offset; /* Convert to local time */

    stm.tm_isdst = -1; /* Don't adjust time for daylight savings */

    t_l = mktime(&stm);

    return (t - t_l);
}


/* Return the offset of the named zone as seconds. tt is a time
   indicating the date for which you want the offset */
int icaltime_utc_offset(struct icaltimetype tt, const char* tzid)
{
#ifdef HAVE_TIMEZONE
    extern long int timezone;
#endif
    time_t now;
    struct tm *stm;

    char *tzstr = 0;

    tzstr = set_tz(tzid);
 
    /* Get the offset */

    now = icaltime_as_timet(tt);

    stm = localtime(&now); /* This sets 'timezone'*/

    unset_tz(tzstr);

#ifdef HAVE_TIMEZONE
    return timezone+ (- icaltime_daylight_offset(tt,tzid));;
#else
    return -stm->tm_gmtoff+ (- icaltime_daylight_offset(tt,tzid));;
#endif
}

int icaltime_local_utc_offset()
{
    time_t now;
    struct tm *stm;

    now = time(0);
    stm = localtime(&now); /* This sets 'timezone'*/


#ifdef HAVE_TIMEZONE
    return timezone + (-icaltime_local_daylight_offset());;
#else
    return -stm->tm_gmtoff + (-icaltime_local_daylight_offset());;
#endif
}


struct icaltimetype icaltime_as_local(struct icaltimetype tt)
{
    if(icaltime_is_null_time(tt) || tt.is_utc == 0){
	return tt;
    }

    tt.second -= icaltime_local_utc_offset();

    tt.is_utc = 0;

    return icaltime_normalize(tt);
}

/* Normalize by converting from localtime to utc and back to local
   time. This uses localtime because localtime and mktime are inverses
   of each other */

struct icaltimetype icaltime_normalize(struct icaltimetype tt)
{
    struct tm stm;
    time_t tut;

    memset(&stm,0,sizeof( struct tm));

    stm.tm_sec = tt.second;
    stm.tm_min = tt.minute;
    stm.tm_hour = tt.hour;
    stm.tm_mday = tt.day;
    stm.tm_mon = tt.month-1;
    stm.tm_year = tt.year-1900;
    stm.tm_isdst = -1; /* prevents mktime from changing hour based on
			  daylight savings */

    tut = mktime(&stm);

    stm = *(localtime(&tut));

    tt.second = stm.tm_sec;
    tt.minute = stm.tm_min;
    tt.hour = stm.tm_hour;
    tt.day = stm.tm_mday;
    tt.month = stm.tm_mon +1;
    tt.year = stm.tm_year+1900;

    return tt;
}


#ifndef ICAL_NO_LIBICAL
#include "icalvalue.h"

struct icaltimetype icaltime_from_string(const char* str)
{
    struct icaltimetype tt = icaltime_null_time();
    int size;

    icalerror_check_arg_re(str!=0,"str",icaltime_null_time());

    size = strlen(str);
    
    if(size == 15) { /* Local time */
	tt.is_utc = 0;
	tt.is_date = 0;
    } else if (size == 16) { /* UTC time, ends in 'Z'*/
	tt.is_utc = 1;
	tt.is_date = 0;

	if(str[15] != 'Z'){
	    return icaltime_null_time();
	}
	    
    } else if (size == 8) { /* A DATE */
	tt.is_utc = 0;
	tt.is_date = 1;
    } else { /* error */
	return icaltime_null_time();
    }

    if(tt.is_date == 1){
	sscanf(str,"%04d%02d%02d",&tt.year,&tt.month,&tt.day);
    } else {
	char tsep;
	sscanf(str,"%04d%02d%02d%c%02d%02d%02d",&tt.year,&tt.month,&tt.day,
	       &tsep,&tt.hour,&tt.minute,&tt.second);

	if(tsep != 'T'){
	    return icaltime_null_time();
	}

    }

    return tt;    
}
#endif

char ctime_str[20];
char* icaltime_as_ctime(struct icaltimetype t)
{
    time_t tt = icaltime_as_timet(t);

    sprintf(ctime_str,"%s",ctime(&tt));

    ctime_str[strlen(ctime_str)-1] = 0;

    return ctime_str;
}


short days_in_month[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

short icaltime_days_in_month(short month,short year)
{

    int is_leap =0;
    int days = days_in_month[month];

    assert(month > 0);
    assert(month <= 12);

    if( (year % 4 == 0 && year % 100 != 0) ||
	year % 400 == 0){
	is_leap =1;
    }

    if( month == 2){
	days += is_leap;
    }

    return days;
}

/* 1-> Sunday, 7->Saturday */
short icaltime_day_of_week(struct icaltimetype t){

    time_t tt = icaltime_as_timet(t);
    struct tm *tm;

    tm = gmtime(&tt);

    return tm->tm_wday+1;
}

short icaltime_start_doy_of_week(struct icaltimetype t){
    time_t tt = icaltime_as_timet(t);
    time_t start_tt;
    struct tm *stm;
    int syear;

    stm = gmtime(&tt);
    syear = stm->tm_year;

    start_tt = tt - stm->tm_wday*(60*60*24);

    stm = gmtime(&start_tt);
    
    if(syear == stm->tm_year){
	return stm->tm_yday+1;
    } else {
	/* return negative to indicate that start of week is in
           previous year. */
	int is_leap = 0;
	int year = stm->tm_year;

	if( (year % 4 == 0 && year % 100 != 0) ||
	    year % 400 == 0){
	    is_leap =1;
	}

	return (stm->tm_yday+1)-(365+is_leap);
    }
    
}



short icaltime_day_of_year(struct icaltimetype t){
    time_t tt = icaltime_as_timet(t);
    struct tm *stm;

    stm = gmtime(&tt);

    return stm->tm_yday+1;
    
}

/* Jan 1 is day #1, not 0 */
struct icaltimetype icaltime_from_day_of_year(short doy,  short year)
{
    struct tm stm; 
    time_t tt;

    /* Get the time of january 1 of this year*/
    memset(&stm,0,sizeof(struct tm)); 
    stm.tm_year = year-1900;
    stm.tm_mday = 1;

    tt = mktime(&stm);

    /* Now add in the days */

    doy--;
    tt += doy *60*60*24;

    return icaltime_from_timet(tt, 1);
}

struct icaltimetype icaltime_null_time()
{
    struct icaltimetype t;
    memset(&t,0,sizeof(struct icaltimetype));

    return t;
}
int icaltime_is_null_time(struct icaltimetype t)
{
    if (t.second +t.minute+t.hour+t.day+t.month+t.year == 0){
	return 1;
    }

    return 0;

}

int icaltime_compare(struct icaltimetype a,struct icaltimetype b)
{
    time_t t1 = icaltime_as_timet(a);
    time_t t2 = icaltime_as_timet(b);

    if (t1 > t2) { 
	return 1; 
    } else if (t1 < t2) { 
	return -1;
    } else { 
	return 0; 
    }

}

int
icaltime_compare_date_only (struct icaltimetype a, struct icaltimetype b)
{
    time_t t1;
    time_t t2;

    if (a.year == b.year && a.month == b.month && a.day == b.day)
        return 0;

    t1 = icaltime_as_timet (a);
    t2 = icaltime_as_timet (b);

    if (t1 > t2) 
	return 1; 
    else if (t1 < t2)
	return -1;
    else {
	/* not reached */
	assert (0);
	return 0;
    }
}


time_t
icalperiodtype_duration (struct icalperiodtype period);


time_t
icalperiodtype_end (struct icalperiodtype period);


/* From Russel Steinthal */
time_t icaldurationtype_as_timet(struct icaldurationtype dur)
{
        return (time_t) (dur.seconds +
                         (60 * dur.minutes) +
                         (60 * 60 * dur.hours) +
                         (60 * 60 * 24 * dur.days) +
                         (60 * 60 * 24 * 7 * dur.weeks));
} 

/* From Seth Alves,  <alves@hungry.com>   */
struct icaldurationtype icaldurationtype_from_timet(time_t t)
{
        struct icaldurationtype dur;
        time_t used = 0;
 
        dur.weeks = (t - used) / (60 * 60 * 24 * 7);
        used += dur.weeks * (60 * 60 * 24 * 7);
        dur.days = (t - used) / (60 * 60 * 24);
        used += dur.days * (60 * 60 * 24);
        dur.hours = (t - used) / (60 * 60);
        used += dur.hours * (60 * 60);
        dur.minutes = (t - used) / (60);
        used += dur.minutes * (60);
        dur.seconds = (t - used);
 
        return dur;
}

#ifndef ICAL_NO_LIBICAL
#include "icalvalue.h"
struct icaldurationtype icaldurationtype_from_string(const char* str)
{

    icalvalue *v = icalvalue_new_from_string(ICAL_DURATION_VALUE,str);

    if( v !=0){
	return icalvalue_get_duration(v);
    } else {
        struct icaldurationtype dur;
	memset(&dur,0,sizeof(struct icaldurationtype));
	return dur;
    }
 
}

#endif

struct icaltimetype  icaltime_add(struct icaltimetype t,
				  struct icaldurationtype  d)
{
    time_t dt = icaldurationtype_as_timet(d);

    t.second += dt;

    t = icaltime_normalize(t);

    return t;
}

struct icaldurationtype  icaltime_subtract(struct icaltimetype t1,
					   struct icaltimetype t2)
{

    time_t t1t = icaltime_as_timet(t1);
    time_t t2t = icaltime_as_timet(t2);

    return icaldurationtype_from_timet(t1t-t2t);


}

