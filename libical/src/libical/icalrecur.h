/* -*- Mode: C -*- */
/*======================================================================
 FILE: icalrecur.h
 CREATOR: eric 20 March 2000


 (C) COPYRIGHT 2000, Eric Busboom, http://www.softwarestudio.org

 This program is free software; you can redistribute it and/or modify
 it under the terms of either: 

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.fsf.org/copyleft/lesser.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/


======================================================================*/

#ifndef ICALRECUR_H
#define ICALRECUR_H

#include <time.h>
#include "icaltime.h"

/***********************************************************************
 * Recurrance enumerations
**********************************************************************/

typedef enum icalrecurrencetype_frequency
{
    /* These enums are used to index an array, so don't change the
       order or the integers */

    ICAL_SECONDLY_RECURRENCE=0,
    ICAL_MINUTELY_RECURRENCE=1,
    ICAL_HOURLY_RECURRENCE=2,
    ICAL_DAILY_RECURRENCE=3,
    ICAL_WEEKLY_RECURRENCE=4,
    ICAL_MONTHLY_RECURRENCE=5,
    ICAL_YEARLY_RECURRENCE=6,
    ICAL_NO_RECURRENCE=7

} icalrecurrencetype_frequency;

typedef enum icalrecurrencetype_weekday
{
    ICAL_NO_WEEKDAY,
    ICAL_SUNDAY_WEEKDAY,
    ICAL_MONDAY_WEEKDAY,
    ICAL_TUESDAY_WEEKDAY,
    ICAL_WEDNESDAY_WEEKDAY,
    ICAL_THURSDAY_WEEKDAY,
    ICAL_FRIDAY_WEEKDAY,
    ICAL_SATURDAY_WEEKDAY
} icalrecurrencetype_weekday;

enum {
    ICAL_RECURRENCE_ARRAY_MAX = 0x7f7f,
    ICAL_RECURRENCE_ARRAY_MAX_BYTE = 0x7f
};
    
const char* icalrecur_recurrence_to_string(icalrecurrencetype_frequency kind);
icalrecurrencetype_frequency icalrecur_string_to_recurrence(const char* str);
const char* icalrecur_weekday_to_string(icalrecurrencetype_weekday kind);
icalrecurrencetype_weekday icalrecur_string_to_weekday(const char* str);


/********************** Recurrence type routines **************/

/* See RFC 2445 Section 4.3.10, RECUR Value, for an explaination of
   the values and fields in struct icalrecurrencetype */

#define ICAL_BY_SECOND_SIZE 61
#define ICAL_BY_MINUTE_SIZE 61
#define ICAL_BY_HOUR_SIZE 25
#define ICAL_BY_DAY_SIZE 364 /* 7 days * 52 weeks */
#define ICAL_BY_MONTHDAY_SIZE 32
#define ICAL_BY_YEARDAY_SIZE 367
#define ICAL_BY_WEEKNO_SIZE 54
#define ICAL_BY_MONTH_SIZE 13
#define ICAL_BY_SETPOS_SIZE 367

struct icalrecurrencetype 
{
	icalrecurrencetype_frequency freq;


	/* until and count are mutually exclusive. */
       	struct icaltimetype until; 
	int count;

	short interval;
	
	icalrecurrencetype_weekday week_start;
	
	/* The BY* parameters can each take a list of values. Here I
	 * assume that the list of values will not be larger than the
	 * range of the value -- that is, the client will not name a
	 * value more than once. 
	 
	 * Each of the lists is terminated with the value
	 * ICAL_RECURRENCE_ARRAY_MAX unless the the list is full.
	 */

	short by_second[ICAL_BY_SECOND_SIZE];
	short by_minute[ICAL_BY_MINUTE_SIZE];
	short by_hour[ICAL_BY_HOUR_SIZE];
	short by_day[ICAL_BY_DAY_SIZE]; /* Encoded value, see below */
	short by_month_day[ICAL_BY_MONTHDAY_SIZE];
	short by_year_day[ ICAL_BY_YEARDAY_SIZE];
	short by_week_no[ICAL_BY_WEEKNO_SIZE];
	short by_month[ICAL_BY_MONTH_SIZE];
	short by_set_pos[ICAL_BY_SETPOS_SIZE];
};


void icalrecurrencetype_clear(struct icalrecurrencetype *r);

/* The 'day' element of the by_day array is encoded to allow
representation of both the day of the week ( Monday, Tueday), but also
the Nth day of the week ( First tuesday of the month, last thursday of
the year) These routines decode the day values */

/* 1 == Monday, etc. */
enum icalrecurrencetype_weekday icalrecurrencetype_day_day_of_week(short day);

/* 0 == any of day of week. 1 == first, 2 = second, -2 == second to last, etc */
short icalrecurrencetype_day_position(short day);

/* Return the next occurance of 'r' after the time specified by 'after' */
struct icaltimetype icalrecurrencetype_next_occurance(
    struct icalrecurrencetype *r,
    struct icaltimetype *after);



typedef void icalrecur_iterator;
void icalrecurrencetype_test();

/***********************************************************************
 * Recurrance rule parser
**********************************************************************/

struct icalrecurrencetype icalrecurrencetype_from_string(const char* str);
char* icalrecurrencetype_as_string(struct icalrecurrencetype *recur);


/********** recurrence routines ********************/

icalrecur_iterator* icalrecur_iterator_new(struct icalrecurrencetype rule, struct icaltimetype dtstart);

struct icaltimetype icalrecur_iterator_next(icalrecur_iterator*);

int icalrecur_iterator_count(icalrecur_iterator*);

void icalrecur_iterator_free(icalrecur_iterator*);

/* Fills array up with at most 'count' time_t values, each
   representing an occurrence time in seconds past the POSIX epoch */
int icalrecur_expand_recurrence(char* rule, time_t start, 
				int count, time_t* array);


#endif
