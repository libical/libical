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

#ifndef VZIC_OUTPUT_H
#define VZIC_OUTPUT_H

#include <glib.h>

void output_vtimezone_files(const char *directory,
                            GArray *zone_data,
                            GHashTable *rule_data,
                            GHashTable *link_data,
                            int max_until_year);

void ensure_directory_exists(const char *directory);

#endif /* VZIC_OUTPUT_H */
