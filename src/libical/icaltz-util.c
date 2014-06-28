/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * Authors : 
 *  Chenthill Palanisamy <pchenthill@novell.com>
 *
 * Copyright 2007, Novell, Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of version 2 of the GNU Lesser General Public 
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#if defined(sun) && defined(__SVR4)
#include <sys/types.h>
#include <sys/byteorder.h>
#else
# ifdef HAVE_BYTESWAP_H
#  include <byteswap.h>
# endif
# ifdef HAVE_ENDIAN_H
#  include <endian.h>
# else
# ifdef HAVE_SYS_ENDIAN_H
#  include <sys/endian.h>
#  ifdef bswap32
#   define bswap_32 bswap32
#  else
#   define bswap_32 swap32
#  endif
# endif
# endif
#endif

#ifdef _MSC_VER
#if !defined(HAVE_BYTESWAP_H) && !defined(HAVE_SYS_ENDIAN_H) && !defined(HAVE_ENDIAN_H)
#define bswap_16(x) (((x) << 8) & 0xff00) | (((x) >> 8 ) & 0xff)
#define bswap_32(x) (((x) << 24) & 0xff000000)  \
                    | (((x) << 8) & 0xff0000)   \
                    | (((x) >> 8) & 0xff00)     \
                    | (((x) >> 24) & 0xff )
#define bswap_64(x) ((((x) & 0xff00000000000000ull) >> 56) \
                    | (((x) & 0x00ff000000000000ull) >> 40) \
                    | (((x) & 0x0000ff0000000000ull) >> 24) \
                    | (((x) & 0x000000ff00000000ull) >> 8) \
                    | (((x) & 0x00000000ff000000ull) << 8) \
                    | (((x) & 0x0000000000ff0000ull) << 24) \
                    | (((x) & 0x000000000000ff00ull) << 40) \
                    | (((x) & 0x00000000000000ffull) << 56))
#endif
#include <io.h>
#endif

#if defined(__APPLE__)
#define bswap_16(x) (((x) << 8) & 0xff00) | (((x) >> 8 ) & 0xff)
#define bswap_32 __builtin_bswap32
#define bswap_64 __builtin_bswap64
#endif

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

#ifndef F_OK
#define F_OK 0
#endif

#ifndef R_OK
#define R_OK 4
#endif

#include <limits.h>
#include <time.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <libical/icalerror.h>
#include <icaltz-util.h>

typedef struct 
{
	char	ttisgmtcnt [4];	
	char	ttisstdcnt[4];	
	char	leapcnt[4];		
	char	timecnt[4];	
	char	typecnt[4];
	char	charcnt[4];			
} tzinfo; 

static int r_pos [] = {1, 2, 3, -2, -1};

static char *search_paths [] = {"/usr/share/zoneinfo","/usr/lib/zoneinfo","/etc/zoneinfo","/usr/share/lib/zoneinfo"};
static char *zdir = NULL;

#define NUM_SEARCH_PATHS (sizeof (search_paths)/ sizeof (search_paths [0]))
#define EFREAD(buf,size,num,fs) \
	if (fread (buf, size, num, fs) == 0  && ferror (fs)) {\
		icalerror_set_errno (ICAL_FILE_ERROR);		\
		goto error;					\
	}							\

typedef struct
{
	long int gmtoff;
	unsigned char isdst;
	unsigned int abbr;	
	unsigned char isstd;
	unsigned char isgmt;
	char *zname;

} ttinfo;

typedef struct
{
	time_t transition;
	long int change;
} leap;

extern const char *ical_tzid_prefix;

static int
decode (const void *ptr)
{
#if defined(sun) && defined(__SVR4)
    if (sizeof (int) == 4)
#ifdef _BIG_ENDIAN
        return *(const int *) ptr;
#else
        return BSWAP_32 (*(const int *) ptr);
#endif
#else
    if ((BYTE_ORDER == BIG_ENDIAN) && sizeof (int) == 4)
        return *(const int *) ptr;
    else if (BYTE_ORDER == LITTLE_ENDIAN && sizeof (int) == 4)
        return bswap_32 (*(const int *) ptr);
#endif
	else
	{
		const unsigned char *p = ptr;
		int result = *p & (1 << (CHAR_BIT - 1)) ? ~0 : 0;

		result = (result << 8) | *p++;
		result = (result << 8) | *p++;
		result = (result << 8) | *p++;
		result = (result << 8) | *p++;

		return result;
	}
}

static char *
zname_from_stridx (char *str, long int idx) 
{
	int i = 0;
	char *ret;
	size_t size;

	i = idx;

	while (str [i] != '\0') 
		i++;

	size = i - idx;
	str += idx;
	ret = (char *) malloc (size + 1);
	ret = strncpy (ret, str, size);
	ret [size] = '\0';

	return ret;
}

static void
set_zonedir (void)
{
	char file_path[PATH_MAX];
	const char *fname = ZONES_TAB_SYSTEM_FILENAME;
	int i;	

	for (i = 0;i < NUM_SEARCH_PATHS; i++) {
		sprintf (file_path, "%s/%s", search_paths [i], fname);
		if (!access (file_path, F_OK|R_OK)) {
			zdir = search_paths [i];
			break;
		}
	}
}


const char *
icaltzutil_get_zone_directory (void)
{
	if (!zdir)
		set_zonedir ();

	return zdir;
}

/* Calculate the relative position of the week in a month from a date */
static int
calculate_pos (icaltimetype icaltime)
{
	int pos;

	pos = (icaltime.day -1) / 7;

	/* Check if pos 3 is the last occurence of the week day in the month */	
	if (pos == 3 && ((icaltime.day + 7) > icaltime_days_in_month (icaltime.month, icaltime.year))) 
		pos = 4;

	return r_pos [pos];
}

icalcomponent*
icaltzutil_fetch_timezone (const char *location)
{
	int ret = 0;
	FILE *f;
	tzinfo type_cnts;
	unsigned int i, num_trans, num_types, num_chars, num_leaps, num_isstd, num_isgmt;
	time_t *transitions = NULL;
	time_t trans, start, end;
	int *trans_idx = NULL, pos, sign, zidx, zp_idx, idx, prev_idx;
	ttinfo *types = NULL;
	char *znames = NULL, *full_path, *tzid, *r_trans, *temp;
	leap *leaps = NULL;
	icalcomponent *tz_comp = NULL, *comp = NULL;
	icalproperty *icalprop;
	icaltimetype dtstart, icaltime;
	struct icalrecurrencetype ical_recur;
	const char *basedir;
	       
	basedir = icaltzutil_get_zone_directory();
	if (!basedir) {
		icalerror_set_errno (ICAL_FILE_ERROR);
		return NULL;
	}

	full_path = (char *) malloc (strlen (basedir) + strlen (location) + 2);
	sprintf (full_path,"%s/%s",basedir, location);

	if ((f = fopen (full_path, "rb")) == 0) {
		icalerror_set_errno (ICAL_FILE_ERROR);
		free (full_path);
		return NULL;
	}

	if ((ret = fseek (f, 20, SEEK_SET)) != 0) {
		icalerror_set_errno (ICAL_FILE_ERROR);
		goto error;	
	}

	EFREAD(&type_cnts, 24, 1, f);

	num_isgmt = decode (type_cnts.ttisgmtcnt);
	num_leaps = decode (type_cnts.leapcnt);
	num_chars = decode (type_cnts.charcnt);
	num_trans = decode (type_cnts.timecnt);
	num_isstd = decode (type_cnts.ttisstdcnt);
	num_types = decode (type_cnts.typecnt);

	transitions = calloc (num_trans, sizeof (time_t));
	r_trans = calloc (num_trans, 4);
	EFREAD(r_trans, 4, num_trans, f);
	temp = r_trans;	

	if (num_trans) {
		trans_idx = calloc (num_trans, sizeof (int));
		for (i = 0; i < num_trans; i++) {
			trans_idx [i] = fgetc (f);
			transitions [i] = decode (r_trans);
			r_trans += 4;
		}
	}
	
	free (temp);

	types = calloc (num_types, sizeof (ttinfo));
	for (i = 0; i < num_types; i++) {
		unsigned char a [4];
		int c;

		EFREAD(a, 4, 1, f);
		c = fgetc (f);
		types [i].isdst = c;
		if((c = fgetc (f)) < 0) {
		   c = 0;
		   break;
		}
		types [i].abbr = c;
		types [i].gmtoff = decode (a);
	}

	znames = (char *) malloc (num_chars);
	EFREAD(znames, num_chars, 1, f);

	/* We got all the information which we need */

	leaps = calloc (num_leaps, sizeof (leap));
	for (i = 0; i < num_leaps; i++) {
		char c [4];

		EFREAD (c, 4, 1, f);
		leaps [i].transition = decode (c);

		EFREAD (c, 4, 1, f);
		leaps [i].change = decode (c);
	}

	for (i = 0; i < num_isstd; ++i) {
		int c = getc (f);
		types [i].isstd = c != 0;
	}

	while (i < num_types)
		types [i++].isstd = 0;

	for (i = 0; i <  num_isgmt; ++i) {
		int c = getc (f);
		types [i].isgmt = c != 0;
	}

	while (i < num_types)
		types [i++].isgmt = 0;

	/* Read all the contents now */

	for (i = 0; i < num_types; i++) 
		types [i].zname = zname_from_stridx (znames, types [i].abbr);

	tz_comp = icalcomponent_new (ICAL_VTIMEZONE_COMPONENT);

	/* Add tzid property */
	tzid = (char *) malloc (strlen (ical_tzid_prefix) + strlen (location) + 8);
	sprintf (tzid, "%sTzfile/%s", ical_tzid_prefix, location);
	icalprop = icalproperty_new_tzid (tzid);
	icalcomponent_add_property (tz_comp, icalprop);
	free (tzid);

	icalprop = icalproperty_new_x (location);
	icalproperty_set_x_name (icalprop, "X-LIC-LOCATION");
	icalcomponent_add_property (tz_comp, icalprop);

	prev_idx = 0;
	if (num_trans == 0) {
		prev_idx = idx = 0;
		
	} else {
		idx = trans_idx[0];
	}
	start = 0;
	for (i = 1; i < num_trans; i++, start = end) {
		prev_idx = idx;
		idx = trans_idx [i];
		end = transitions [i] + types [prev_idx].gmtoff;
		/* don't bother starting until the epoch */
		if (0 > end)
			continue;

		if (types [prev_idx].isdst)
			comp = icalcomponent_new (ICAL_XDAYLIGHT_COMPONENT);
		else
			comp = icalcomponent_new (ICAL_XSTANDARD_COMPONENT);
		icalprop = icalproperty_new_tzname (types [prev_idx].zname);
		icalcomponent_add_property (comp, icalprop);
		dtstart = icaltime_from_timet(start, 0);
		icalprop = icalproperty_new_dtstart (dtstart);
		icalcomponent_add_property (comp, icalprop);
		icalprop = icalproperty_new_tzoffsetfrom (types [idx].gmtoff);
		icalcomponent_add_property (comp, icalprop);
		icalprop = icalproperty_new_tzoffsetto (types [prev_idx].gmtoff);
		icalcomponent_add_property (comp, icalprop);
		icalcomponent_add_component (tz_comp, comp);
	}
	/* finally, add a last zone with no end date */
	if (types [idx].isdst)
		comp = icalcomponent_new (ICAL_XDAYLIGHT_COMPONENT);
	else
		comp = icalcomponent_new (ICAL_XSTANDARD_COMPONENT);
	icalprop = icalproperty_new_tzname (types [idx].zname);
	icalcomponent_add_property (comp, icalprop);
	dtstart = icaltime_from_timet(start, 0);
	icalprop = icalproperty_new_dtstart (dtstart);
	icalcomponent_add_property (comp, icalprop);
	icalprop = icalproperty_new_tzoffsetfrom (types [prev_idx].gmtoff);
	icalcomponent_add_property (comp, icalprop);
	icalprop = icalproperty_new_tzoffsetto (types [idx].gmtoff);
	icalcomponent_add_property (comp, icalprop);
	icalcomponent_add_component (tz_comp, comp);


error:
	if (f)
		fclose  (f);

	if (transitions)
		free (transitions);
	if (trans_idx)
		free (trans_idx);
	if (types) {
		for (i = 0; i < num_types; i++) 
			if (types [i].zname)
				free (types [i].zname);
		free (types);
	}
	if (znames)
		free (znames);
	free (full_path);
	if (leaps)
		free (leaps);

	return tz_comp;
}

/*
int 
main (int argc, char *argv [])
{
	tzutil_fetch_timezone (argv [1]);
	return 0;
}*/
