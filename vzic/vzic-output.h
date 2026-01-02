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

#ifndef _VZIC_OUTPUT_H_
#define _VZIC_OUTPUT_H_

#include <glib.h>

void		output_vtimezone_files		(char		*directory,
						 GArray		*zone_data,
						 GHashTable	*rule_data,
						 GHashTable	*link_data,
						 int		 max_until_year);

void		ensure_directory_exists		(char		*directory);

#endif /* _VZIC_OUTPUT_H_ */
