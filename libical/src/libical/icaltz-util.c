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

#include "config.h"
#include <string.h>
#if defined(sun) && defined(__SVR4)
#include <sys/byteorder.h>
#else
# ifdef HAVE_BYTESWAP_H
#  include <byteswap.h>
# endif
# ifdef HAVE_SYS_ENDIAN_H
#  define bswap_32 swap32
# endif
# ifdef HAVE_ENDIAN_H
#  include <endian.h>
# endif 

#endif
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
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

static r_pos [] = {1, 2, 3, -2, -1};

static char *search_paths [] = {"/usr/share/zoneinfo","/usr/lib/zoneinfo","/etc/zoneinfo","/usr/share/lib/zoneinfo"};
static char *zdir = NULL;

#define TZID_PREFIX		"/softwarestudio.org/"

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
find_transidx (time_t *transitions, ttinfo *types, int *trans_idx, long int num_trans, int *stdidx, int *dstidx) 
{
	time_t now = time (NULL);
	int i, found = 0, idx;

	for (i = 0; i < num_trans; i++)	{
		if (now < transitions [i]) {
			found = 1;	
			break;
		}
	}

	/* If the transition time is not found, it means the timezone does not have the dst changes */
	if (!found) {
		*stdidx = i -1;
		return;
	}

	idx = trans_idx [i];
	types [idx].isdst ? (*dstidx = i) : (*stdidx = i);
	
	if (i >= num_trans - 1) 
		i--;
	else 
		i++;

	idx = trans_idx [i];
	types [idx].isdst ? (*dstidx = i) : (*stdidx = i);

	if (*dstidx != -1 && *stdidx != -1)
		return;
	else 
		icalerror_set_errno (ICAL_MALFORMEDDATA_ERROR);
}


static void
set_zone_directory (void)
{
	const char *fname = ZONES_TAB_SYSTEM_FILENAME;
	int i;	

	for (i = 0;i < NUM_SEARCH_PATHS; i++) {
		int flen = strlen (search_paths [i]) + strlen (fname) + 2;
		char file_path [flen];

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
		set_zone_directory ();

	return zdir;
}

icalcomponent*
icaltzutil_fetch_timezone (const char *location)
{
	int ret = 0;
	FILE *f;
	tzinfo type_cnts;
	unsigned int num_trans, num_types, num_chars, num_leaps, num_isstd, num_isgmt;
	time_t *transitions, trans;
	int *trans_idx = NULL, dstidx = -1, stdidx = -1, pos, sign, zidx, zp_idx, i;
	ttinfo *types;
	char *znames = NULL, *full_path, *tzid, *r_trans, *temp;
	leap *leaps = NULL;
	icalcomponent *tz_comp = NULL, *dst_comp = NULL, *std_comp = NULL;
	icalproperty *icalprop;
	icaltimetype dtstart, icaltime;
	struct icalrecurrencetype ical_recur;
	       
	if (!zdir) 
		set_zone_directory ();
	
	full_path = (char *) malloc (strlen (zdir) + strlen (location) + 2);
	sprintf (full_path,"%s/%s",zdir, location);
	
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
		c = fgetc (f);
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

	if (num_trans != 0)
		find_transidx (transitions, types, trans_idx, num_trans, &stdidx, &dstidx);
	else
		stdidx = 0;

	tz_comp = icalcomponent_new (ICAL_VTIMEZONE_COMPONENT);

	/* Add tzid property */
	tzid = (char *) malloc (strlen (TZID_PREFIX) + strlen (location) + 8);
	sprintf (tzid, "%sTzfile/%s", TZID_PREFIX, location);
	icalprop = icalproperty_new_tzid (tzid);
	icalcomponent_add_property (tz_comp, icalprop);
	free (tzid);

	icalprop = icalproperty_new_x (location);
	icalproperty_set_x_name (icalprop, "X-LIC-LOCATION");
	icalcomponent_add_property (tz_comp, icalprop);
	
	if (stdidx != -1) {
		if (stdidx != 0)
			zidx = trans_idx [stdidx];
		else 
			zidx = 0;

		std_comp = icalcomponent_new (ICAL_XSTANDARD_COMPONENT);
		icalprop = icalproperty_new_tzname (types [zidx].zname);
		icalcomponent_add_property (std_comp, icalprop);

		trans = transitions [stdidx] + types [zidx].gmtoff;
		icaltime = icaltime_from_timet (trans, 0);
		dtstart = icaltime;
		dtstart.year = 1970;
		dtstart.minute = dtstart.second = 0;
		icalprop = icalproperty_new_dtstart (dtstart);
		icalcomponent_add_property (std_comp, icalprop);

		/* If DST changes are present use RRULE */
		if (dstidx != -1) {
			zp_idx = trans_idx [stdidx-1]; 
			icalrecurrencetype_clear (&ical_recur);
			ical_recur.freq = ICAL_YEARLY_RECURRENCE;
			ical_recur.by_month [0] = icaltime.month;
			pos = r_pos [icaltime.day/7];
			pos < 0 ? (sign = -1): (sign = 1);
			ical_recur.by_day [0] = sign * ((abs (pos) * 8) + icaltime_day_of_week (icaltime));
			icalprop = icalproperty_new_rrule (ical_recur);
			icalcomponent_add_property (std_comp, icalprop);

			icalprop = icalproperty_new_tzoffsetfrom (types [zp_idx].gmtoff);
			icalcomponent_add_property (std_comp, icalprop);
		} else {
			icalprop = icalproperty_new_tzoffsetfrom (types [zidx].gmtoff);
			icalcomponent_add_property (std_comp, icalprop);
		}

		icalprop = icalproperty_new_tzoffsetto (types [zidx].gmtoff);
		icalcomponent_add_property (std_comp, icalprop);

		icalcomponent_add_component (tz_comp, std_comp);
	} else 
		icalerror_set_errno (ICAL_MALFORMEDDATA_ERROR);
	
	if (dstidx != -1) {
		zidx = trans_idx [dstidx];
	       	zp_idx = trans_idx [dstidx-1];
		dst_comp = icalcomponent_new (ICAL_XDAYLIGHT_COMPONENT);
		icalprop = icalproperty_new_tzname (types [zidx].zname);
		icalcomponent_add_property (dst_comp, icalprop);

		trans = transitions [dstidx] + types [zidx].gmtoff;
		icaltime = icaltime_from_timet (trans, 0);
		dtstart = icaltime;
		dtstart.year = 1970;
		dtstart.minute = dtstart.second = 0;
		icalprop = icalproperty_new_dtstart (dtstart);
		icalcomponent_add_property (dst_comp, icalprop);

		icalrecurrencetype_clear (&ical_recur);
		ical_recur.freq = ICAL_YEARLY_RECURRENCE;
		ical_recur.by_month [0] = icaltime.month;
		pos = r_pos [icaltime.day/7];
		pos < 0 ? (sign = -1): (sign = 1);
		ical_recur.by_day [0] = sign * ((abs (pos) * 8) + icaltime_day_of_week (icaltime));
		icalprop = icalproperty_new_rrule (ical_recur);
		icalcomponent_add_property (dst_comp, icalprop);

		icalprop = icalproperty_new_tzoffsetfrom (types [zp_idx].gmtoff);
		icalcomponent_add_property (dst_comp, icalprop);

		icalprop = icalproperty_new_tzoffsetto (types [zidx].gmtoff);
		icalcomponent_add_property (dst_comp, icalprop);

		icalcomponent_add_component (tz_comp, dst_comp);
	}

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
