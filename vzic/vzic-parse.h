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

#ifndef VZIC_PARSE_H
#define VZIC_PARSE_H

#include <glib.h>

void parse_olson_file(char *filename,
                      GArray *zone_data,
                      GHashTable *rule_data,
                      GHashTable *link_data,
                      int *max_until_year);

GHashTable *parse_zone_tab(const char *filename);

#endif /* VZIC_PARSE_H */
