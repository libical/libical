/*
 * Vzic - a program to convert Olson timezone database files into VZTIMEZONE
 * files compatible with the iCalendar specification (RFC2445).
 *
 * SPDX-FileCopyrightText: 2000-2001 Ximian, Inc.
 * SPDX-FileCopyrightText: 2003, Damon Chaplin <damon@ximian.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Author: Damon Chaplin <damon@gnome.org>
 */

#ifndef _VZIC_PARSE_H_
#define _VZIC_PARSE_H_

#include <glib.h>

void		parse_olson_file		(char		*filename,
						 GArray	       *zone_data,
						 GHashTable    *rule_data,
						 GHashTable    *link_data,
						 int		*max_until_year);

GHashTable*	parse_zone_tab			(char		*filename);

#endif /* _VZIC_PARSE_H_ */
