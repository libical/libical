/* -*- Mode: C -*-
  ======================================================================
  FILE: icaltime.c
  CREATOR: eric 02 June 2000
  
  $Id: icaltime.c,v 1.6 2001-01-26 21:28:54 ebusboom Exp $
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
#include "icalmemory.h"
#endif




extern long int timezone;  /* Global defined by libc */
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


struct set_tz_save {char* orig_tzid; char* new_env_str;};

struct set_tz_save set_tz(const char* tzid)
{

    char *orig_tzid = 0;
    char *new_env_str;
    struct set_tz_save savetz;
    size_t tmp_sz; 

    savetz.orig_tzid = 0;
    savetz.new_env_str = 0;

    if(getenv("TZ") != 0){
	orig_tzid = (char*)strdup(getenv("TZ"));

	if(orig_tzid == 0){
	    return savetz;
	}
    }

    tmp_sz =strlen(tzid)+4; 
    new_env_str = (char*)malloc(tmp_sz);

    if(new_env_str == 0){
	return savetz;
    }

    strcpy(new_env_str,"TZ=");
    strcpy(new_env_str+3,tzid);

    putenv(new_env_str); 

    savetz.orig_tzid = orig_tzid;
    savetz.new_env_str = new_env_str;

    return savetz;
}

void unset_tz(struct set_tz_save savetz)
{
    /* restore the original environment */

    char* orig_tzid = savetz.orig_tzid;
    char* new_tzstr = getenv("TZ");

    if(orig_tzid!=0){	
	size_t tmp_sz =strlen(orig_tzid)+4; 
	char* orig_env_str = (char*)malloc(tmp_sz);

	if(orig_env_str == 0){

	}
	
	strcpy(orig_env_str,"TZ=");
	strcpy(orig_env_str+3,orig_tzid);

	putenv(orig_env_str);

	free(orig_tzid);
    } else {
	putenv("TZ"); /* Delete from environment */
    } 

    if(savetz.new_env_str != 0){
	free(savetz.new_env_str);
    }
}


time_t icaltime_as_timet(struct icaltimetype tt)
{
    struct tm stm;
    time_t t;

    memset(&stm,0,sizeof( struct tm));

    if(icaltime_is_null_time(tt)) {
	return 0;
    }

    stm.tm_sec = tt.second;
    stm.tm_min = tt.minute;
    stm.tm_hour = tt.hour;
    stm.tm_mday = tt.day;
    stm.tm_mon = tt.month-1;
    stm.tm_year = tt.year-1900;
    stm.tm_isdst = -1;

    if(tt.is_utc == 1 || tt.is_date == 1){
	struct set_tz_save old_tz = set_tz("UTC");
	t = mktime(&stm);
	unset_tz(old_tz);
    } else {
	t = mktime(&stm);
    }

    return t;

}

char* icaltime_as_ical_string(struct icaltimetype tt)
{
    size_t size = 17;
    char* buf = icalmemory_new_buffer(size);

    if(tt.is_date){
	snprintf(buf, size,"%04d%02d%02d",tt.year,tt.month,tt.day);
    } else {
	char* fmt;
	if(tt.is_utc){
	    fmt = "%04d%02d%02dT%02d%02d%02dZ";
	} else {
	    fmt = "%04d%02d%02dT%02d%02d%02d";
	}
	snprintf(buf, size,fmt,tt.year,tt.month,tt.day,
		 tt.hour,tt.minute,tt.second);
    }
    
    icalmemory_add_tmp_buffer(buf);

    return buf;

}


/* convert tt, of timezone tzid, into a utc time */
struct icaltimetype icaltime_as_utc(struct icaltimetype tt,const char* tzid)
{
    int tzid_offset;

    if(tt.is_utc == 1 || tt.is_date == 1){
	return tt;
    }

    tzid_offset = icaltime_utc_offset(tt,tzid);

    tt.second -= tzid_offset;

    tt.is_utc = 1;

    return icaltime_normalize(tt);
}

/* convert tt, a time in UTC, into a time in timezone tzid */
struct icaltimetype icaltime_as_zone(struct icaltimetype tt,const char* tzid)
{
    int tzid_offset;

    tzid_offset = icaltime_utc_offset(tt,tzid);

    tt.second += tzid_offset;

    tt.is_utc = 0;

    return icaltime_normalize(tt);

}


/* Return the offset of the named zone as seconds. tt is a time
   indicating the date for which you want the offset */
int icaltime_utc_offset(struct icaltimetype ictt, const char* tzid)
{

    time_t tt = icaltime_as_timet(ictt);
    time_t offset_tt;
    struct tm gtm;
    struct set_tz_save old_tz; 

    if(tzid != 0){
	old_tz = set_tz(tzid);
    }
 
    /* Mis-interpret a UTC broken out time as local time */
    gtm = *(gmtime(&tt));
    gtm.tm_isdst = localtime(&tt)->tm_isdst;    
    offset_tt = mktime(&gtm);
    
    if(tzid != 0){
	unset_tz(old_tz);
    }

    return tt-offset_tt;
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
    
    if(size == 15) { /* floating time */
	tt.is_utc = 0;
	tt.is_date = 0;
    } else if (size == 16) { /* UTC time, ends in 'Z'*/
	tt.is_utc = 1;
	tt.is_date = 0;

	if(str[15] != 'Z'){
	    icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
	    return icaltime_null_time();
	}
	    
    } else if (size == 8) { /* A DATE */
	tt.is_utc = 0;
	tt.is_date = 1;
    } else { /* error */
	icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
	return icaltime_null_time();
    }

    if(tt.is_date == 1){
	sscanf(str,"%04d%02d%02d",&tt.year,&tt.month,&tt.day);
    } else {
	char tsep;
	sscanf(str,"%04d%02d%02d%c%02d%02d%02d",&tt.year,&tt.month,&tt.day,
	       &tsep,&tt.hour,&tt.minute,&tt.second);

	if(tsep != 'T'){
	    icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
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

    if(t.is_utc == 1){
	tm = gmtime(&tt);
    } else {
	tm = localtime(&tt);
    }

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

    if(t.is_utc==1){
	stm = gmtime(&tt);
    } else {
	stm = localtime(&tt);
    }

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


int icaltime_is_valid_time(struct icaltimetype t){
    if(t.is_utc > 1 || t.is_utc < 0 ||
       t.year < 0 || t.year > 3000 ||
       t.is_date > 1 || t.is_date < 0){
	return 0;
    } else {
	return 1;
    }

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

struct icalperiodtype icalperiodtype_from_string (const char* str)
{
    
    struct icalperiodtype p, null_p;
    char *s = strdup(str);
    char *start, *end = s;
    int old_ieaf = icalerror_errors_are_fatal;

    p.start = p.end = icaltime_null_time();
    p.duration = icaldurationtype_from_int(0);

    null_p = p;

    if(s == 0) goto error;

    start = s;
    end = strchr(s, '/');

    if(end == 0) goto error;

    *end = 0;
    end++;

    p.start = icaltime_from_string(start);

    if (icaltime_is_null_time(p.start)) goto error;
	
    icalerror_errors_are_fatal = 0;
    p.end = icaltime_from_string(end);
    icalerror_errors_are_fatal = old_ieaf;

    if (icaltime_is_null_time(p.end)){

	p.duration = icaldurationtype_from_string(end);

	if(icaldurationtype_as_int(p.duration) == 0) goto error;
    } 

    return p;

 error:
    icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
    return null_p;
}


const char* icalperiodtype_as_ical_string(struct icalperiodtype p)
{

    const char* start;
    const char* end;

    char *buf;
    size_t buf_size = 40;
    char* buf_ptr = 0;

    buf = (char*)icalmemory_new_buffer(buf_size);
    buf_ptr = buf;
    

    start = icaltime_as_ical_string(p.start);

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, start); 

    if(!icaltime_is_null_time(p.end)){
	end = icaltime_as_ical_string(p.end);
    } else {
	end = icaldurationtype_as_ical_string(p.duration);
    }

    icalmemory_append_char(&buf, &buf_ptr, &buf_size, '/'); 

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, end); 
    

    return buf;
}


time_t
icalperiodtype_duration (struct icalperiodtype period);


time_t
icalperiodtype_end (struct icalperiodtype period);


struct icalperiodtype icalperiodtype_null_period() {
    struct icalperiodtype p;
    p.start = icaltime_null_time();
    p.end = icaltime_null_time();
    p.duration = icaldurationtype_null_duration();

    return p;
}
int icalperiodtype_is_null_period(struct icalperiodtype p){
    
    if(icaltime_is_null_time(p.start) && 
       icaltime_is_null_time(p.end) && 
       icaldurationtype_is_null_duration(p.duration)){
	return 1;
    } else {
	return 0;
    }
}

int icalperiodtype_is_valid_period(struct icalperiodtype p){
    if(icaltime_is_valid_time(p.start) && 
       (icaltime_is_valid_time(p.end) || icaltime_is_null_time(p.end)) )
	{
	    return 1;
	}

    return 0;
}

/* From Russel Steinthal */
int icaldurationtype_as_int(struct icaldurationtype dur)
{
    return (int)( (dur.seconds +
		   (60 * dur.minutes) +
		   (60 * 60 * dur.hours) +
		   (60 * 60 * 24 * dur.days) +
		   (60 * 60 * 24 * 7 * dur.weeks))
		  * (dur.is_neg==1? -1 : 1) ) ;
} 

/* From Seth Alves,  <alves@hungry.com>   */
struct icaldurationtype icaldurationtype_from_int(int t)
{
        struct icaldurationtype dur;
        int used = 0;
 
        dur.weeks = (t - used) / (60 * 60 * 24 * 7);
        used += dur.weeks * (60 * 60 * 24 * 7);
        dur.days = (t - used) / (60 * 60 * 24);
        used += dur.days * (60 * 60 * 24);
        dur.hours = (t - used) / (60 * 60);
        used += dur.hours * (60 * 60);
        dur.minutes = (t - used) / (60);
        used += dur.minutes * (60);
        dur.seconds = (t - used);
 
	dur.is_neg = t<0? 1 : 0;

        return dur;
}

#ifndef ICAL_NO_LIBICAL
#include "icalvalue.h"
struct icaldurationtype icaldurationtype_from_string(const char* str)
{

    int i;
    int begin_flag = 0;
    int time_flag = 0;
    int date_flag = 0;
    int week_flag = 0;
    int digits=-1;
    int scan_size = -1;
    int size = strlen(str);
    char p;
    struct icaldurationtype d;

    memset(&d, 0, sizeof(struct icaldurationtype));

    for(i=0;i != size;i++){
	p = str[i];
	
	switch(p) 
	    {
	    case '-': {
		if(i != 0 || begin_flag == 1) goto error;

		d.is_neg = 1;
		break;
	    }

	    case 'P': {
		if (i != 0 && i !=1 ) goto error;
		begin_flag = 1;
		break;
	    }

	    case 'T': {
		time_flag = 1;
		break;
	    }

	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		{ 
		    
		    /* HACK. Skip any more digits if the l;ast one
                       read has not been assigned */
		    if(digits != -1){
			break;
		    }

		    if (begin_flag == 0) goto error;
		    /* Get all of the digits, not one at a time */
		    scan_size = sscanf((char*)(str+i),"%d",&digits);
		    if(scan_size == 0) goto error;
		    break;
		}

	    case 'H': {	
		if (time_flag == 0||week_flag == 1||d.hours !=0||digits ==-1) 
		    goto error;
		d.hours = digits; digits = -1;
		break;
	    }
	    case 'M': {
		if (time_flag == 0||week_flag==1||d.minutes != 0||digits ==-1) 
		    goto error;
		d.minutes = digits; digits = -1;	    
		break;
	    }
	    case 'S': {
		if (time_flag == 0||week_flag==1||d.seconds!=0||digits ==-1) 
		    goto error;
		d.seconds = digits; digits = -1;	    
		break;
	    }
	    case 'W': {
		if (time_flag==1||date_flag==1||d.weeks!=0||digits ==-1) 
		    goto error;
		week_flag = 1;	
		d.weeks = digits; digits = -1;	    
		break;
	    }
	    case 'D': {
		if (time_flag==1||week_flag==1||d.days!=0||digits ==-1) 
		    goto error;
		date_flag = 1;
		d.days = digits; digits = -1;	    
		break;
	    }
	    default: {
		goto error;
	    }

	    }
    }

    return d;
	

 error:
    icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
    memset(&d, 0, sizeof(struct icaldurationtype));
    return d;

}

#define TMP_BUF_SIZE 1024
void append_duration_segment(char** buf, char** buf_ptr, size_t* buf_size, 
			     char* sep, unsigned int value) {

    char temp[TMP_BUF_SIZE];

    sprintf(temp,"%d",value);

    icalmemory_append_string(buf, buf_ptr, buf_size, temp);
    icalmemory_append_string(buf, buf_ptr, buf_size, sep);
    
}

char* icaldurationtype_as_ical_string(struct icaldurationtype d) 
{

    char *buf, *output_line;
    size_t buf_size = 256;
    char* buf_ptr = 0;
    int seconds;

    buf = (char*)icalmemory_new_buffer(buf_size);
    buf_ptr = buf;
    

    seconds = icaldurationtype_as_int(d);

    if(seconds !=0){
	
	if(d.is_neg == 1){
	    icalmemory_append_char(&buf, &buf_ptr, &buf_size, '-'); 
	}

	icalmemory_append_char(&buf, &buf_ptr, &buf_size, 'P');
    
	if (d.weeks != 0 ) {
	    append_duration_segment(&buf, &buf_ptr, &buf_size, "W", d.weeks);
	}
	
	if (d.days != 0 ) {
	    append_duration_segment(&buf, &buf_ptr, &buf_size, "D", d.days);
	}
	
	if (d.hours != 0 || d.minutes != 0 || d.seconds != 0) {
	    
	    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "T");
	    
	    if (d.hours != 0 ) {
		append_duration_segment(&buf, &buf_ptr, &buf_size, "H", d.hours);
	    }
	    if (d.minutes != 0 ) {
		append_duration_segment(&buf, &buf_ptr, &buf_size, "M", 
					d.minutes);
	    }
	    if (d.seconds != 0 ) {
		append_duration_segment(&buf, &buf_ptr, &buf_size, "S", 
					d.seconds);
	    }
	    
	}
    } else {
	icalmemory_append_string(&buf, &buf_ptr, &buf_size, "PTS0");
    }
 
    output_line = icalmemory_tmp_copy(buf);
    icalmemory_free_buffer(buf);

    return output_line;
    
}

#endif

struct icaldurationtype icaldurationtype_null_duration()
{
    struct icaldurationtype d;
    
    memset(&d,0,sizeof(struct icaldurationtype));
    
    return d;
}

int icaldurationtype_is_null_duration(struct icaldurationtype d)
{
    if(icaldurationtype_as_int(d) == 0){
	return 1;
    } else {
	return 0;
    }
}

struct icaltimetype  icaltime_add(struct icaltimetype t,
				  struct icaldurationtype  d)
{
    int dt = icaldurationtype_as_int(d);
    
    t.second += dt;
    
    t = icaltime_normalize(t);
    
    return t;
}

struct icaldurationtype  icaltime_subtract(struct icaltimetype t1,
					   struct icaltimetype t2)
{

    time_t t1t = icaltime_as_timet(t1);
    time_t t2t = icaltime_as_timet(t2);

    return icaldurationtype_from_int(t1t-t2t);


}

