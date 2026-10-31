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

/*
 * These functions are for dumping all the parsed Zones and Rules to
 * files, to be compared with the output of vzic-dump.pl to check our parsing
 * code is OK. Some of the functions are also used for producing debugging
 * output.
 */

#ifndef VZIC_DUMP_H
#define VZIC_DUMP_H

#include "vzic.h"

#include <stdio.h>
#include <glib.h>

void dump_zone_data(GArray *zone_data,
                    const char *filename);
void dump_rule_data(GHashTable *rule_data,
                    const char *filename);

void dump_rule_array(const char *name,
                     const GArray *rule_array,
                     FILE *fp);

const char *dump_year(int year);
char *dump_day_coded(DayCode day_code,
                     int day_number,
                     int day_weekday);
const char *dump_time(int seconds,
                      TimeCode time_code,
                      gboolean use_zero);

void dump_time_zone_names(GList *names,
                          const char *output_dir,
                          GHashTable *zones_hash);

#endif /* VZIC_DUMP_H */
