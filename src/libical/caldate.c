/*
 * This work is based on work from Hiram Clawson and has been modified to the
 * needs of the libical project. The original copyright notice is as follows:
 */
/*
 *	Copyright (c) 1986-2000, Hiram Clawson
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or
 *	without modification, are permitted provided that the following
 *	conditions are met:
 *
 *		Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the
 *		following disclaimer.
 *
 *		Redistributions in binary form must reproduce the
 *		above copyright notice, this list of conditions and
 *		the following disclaimer in the documentation and/or
 *		other materials provided with the distribution.
 *
 *		Neither name of The Museum of Hiram nor the names of
 *		its contributors may be used to endorse or promote products
 *		derived from this software without specific prior
 *		written permission.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *	CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *	IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *	OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *	STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *	IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *	THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * The modifications made are licensed as follows (to distinguish between
 * the original code and the modifications made, refer to the source code
 * history):
 */
/*======================================================================

   SPDX-FileCopyrightText: 2018, Markus Minichmayr
       https://tapkey.com

   SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
  ========================================================================*/

#include "astime.h"	/*	time structures	*/

void caldat_int( struct ut_instant_int * date )
{
	long jd;
	long ka;
	long kb;
	long kc;
	long kd;
	long ke;
	long ialp;

	jd = (long) (date->j_date0 + 1L);	/* integer julian date */
	ka = (long) jd;
	if ( jd >= 2299161L )
	{
            ialp = (long) (((jd * 100LL) - 186721625LL) / 3652425LL);
            ka = jd + 1L + ialp - ( ialp >> 2 );
	}
	kb = ka + 1524L;
	kc = (long) ( ((kb * 100LL) - 12210LL) / 36525LL );
	kd = (long) ( ( kc * 36525LL ) / 100LL );
	ke = (long) ( (kb - kd) * 10000LL / 306001LL );
	date->day = kb - kd - ((long) ( (ke * 306001LL) / 10000LL ));
	if ( ke > 13L )
		date->month = ke - 13L;
	else
		date->month = ke - 1L;
	if ( (date->month == 2) && (date->day > 28) )
		date->day = 29;
	if ( (date->month == 2) && (date->day == 29) && (ke == 3L) )
		date->year = kc - 4716L;
	else if ( date->month > 2 )
		date->year = kc - 4716L;
	else
		date->year = kc - 4715L;
	date->weekday = (jd + 1L) % 7L;	/* day of week */
	// if ( date->year == ((date->year >> 2) << 2) ) // ubsan "runtime error: left shift of negative value -1179"
	if ( date->year == ((date->year >> 2) * 4) )
		date->day_of_year =
			( ( 275 * date->month ) / 9)
			- ((date->month + 9) / 12)
			+ date->day - 30;
	else
		date->day_of_year =
			( ( 275 * date->month ) / 9)
			- (((date->month + 9) / 12) << 1)
			+ date->day - 30;
}

void juldat_int( struct ut_instant_int * date )
{
	long iy0, im0;
	long ia, ib;
	long jd;

	/* conversion factors */
	if ( date->month <= 2 )
	{
		iy0 = date->year - 1L;
		im0 = date->month + 12;
	}
	else
	{
		iy0 = date->year;
		im0 = date->month;
	}
	ia = iy0 / 100L;
	ib = 2L - ia + (ia >> 2);
	/* calculate julian date	*/
	if ( date->year < 0L )
		jd = (long) (((36525LL * iy0) - 75) / 100)
			+ (long) ((306001LL * (im0 + 1L)) / 10000)
			+ (long) date->day + 1720994L;
	else
		jd = (long) ((36525LL * iy0) / 100)
			+ (long) ((306001LL * (im0 + 1L)) / 10000)
			+ (long) date->day + 1720994L;

	/* on or after 15 October 1582	*/
	if ((date->year > 1582)
			|| ((date->year == 1582) && (((date->month * 100) + date->day) >= 1015))
		) {
		jd += ib;
	}
	date->j_date0 = jd;
	date->weekday = (jd + 2L) % 7L;
}	/*	end of	void juldat( date )	*/
