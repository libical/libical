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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "vzic-dump.h"

static void dump_add_rule(char *name,
                          GArray *rule_array,
                          GPtrArray *name_array);
static int dump_compare_strings(const void *arg1,
                                const void *arg2);

void dump_zone_data(GArray *zone_data,
                    const char *filename)
{
    static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    FILE *fp;
    ZoneLineData *zone_line;
    gboolean output_month, output_day, output_time;

    fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Couldn't create file: %s\n", filename);
        exit(1);
    }

    for (unsigned int i = 0; i < zone_data->len; i++) {
        ZoneData *zone = &g_array_index(zone_data, ZoneData, i);

        fprintf(fp, "Zone\t%s\t", zone->zone_name);

        for (unsigned int j = 0; j < zone->zone_line_data->len; j++) {
            zone_line = &g_array_index(zone->zone_line_data, ZoneLineData, j);

            if (j != 0) {
                fprintf(fp, "\t\t\t");
            }

            fprintf(fp, "%s\t", dump_time(zone_line->stdoff_seconds, TIME_WALL, FALSE));

            if (zone_line->rules) {
                fprintf(fp, "%s\t", zone_line->rules);
            } else if (zone_line->save_seconds != 0) {
                fprintf(fp, "%s\t", dump_time(zone_line->save_seconds, TIME_WALL, FALSE));
            } else {
                fprintf(fp, "-\t");
            }

            fprintf(fp, "%s\t", zone_line->format ? zone_line->format : "-");

            if (zone_line->until_set) {
                fprintf(fp, "%s\t", dump_year(zone_line->until_year));

                output_month = output_day = output_time = FALSE;

                if (zone_line->until_time_code != TIME_WALL || zone_line->until_time_seconds != 0) {
                    output_month = output_day = output_time = TRUE;
                } else if (zone_line->until_day_code != DAY_SIMPLE || zone_line->until_day_number != 1) {
                    output_month = output_day = TRUE;
                } else if (zone_line->until_month != 0) {
                    output_month = TRUE;
                }

                if (output_month) {
                    fprintf(fp, "%s", months[zone_line->until_month]);
                }

                fprintf(fp, "\t");

                if (output_day) {
                    fprintf(fp, "%s", dump_day_coded(zone_line->until_day_code, zone_line->until_day_number, zone_line->until_day_weekday));
                }

                fprintf(fp, "\t");

                if (output_time) {
                    fprintf(fp, "%s", dump_time(zone_line->until_time_seconds, zone_line->until_time_code, FALSE));
                }

            } else {
                fprintf(fp, "\t\t\t");
            }

            fprintf(fp, "\n");
        }
    }

    fclose(fp);
}

void dump_rule_data(GHashTable *rule_data,
                    const char *filename)
{
    FILE *fp;
    GPtrArray *name_array;

    fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Couldn't create file: %s\n", filename);
        exit(1);
    }

    /* We need to sort the rules by their names, so they are in the same order
     as the Perl output. So we place all the names in a temporary GPtrArray,
     sort it, then output them. */
    name_array = g_ptr_array_new();
    g_hash_table_foreach(rule_data, (GHFunc)dump_add_rule, name_array);
    qsort((void *)name_array->pdata, name_array->len, sizeof(char *),
          dump_compare_strings);

    for (unsigned int i = 0; i < name_array->len; i++) {
        const char *name = g_ptr_array_index(name_array, i);
        const GArray *rule_array = g_hash_table_lookup(rule_data, name);
        if (!rule_array) {
            fprintf(stderr, "Couldn't access rules: %s\n", name);
            exit(1);
        }
        dump_rule_array(name, rule_array, fp);
    }

    g_ptr_array_free(name_array, TRUE);

    fclose(fp);
}

static void
dump_add_rule(char *name,
              GArray *rule_array,
              GPtrArray *name_array)
{
    (void)rule_array; /* unused */
    g_ptr_array_add(name_array, name);
}

static int
dump_compare_strings(const void *arg1,
                     const void *arg2)
{
    char **a, **b;

    a = (char **)arg1;
    b = (char **)arg2;

    return strcmp(*a, *b);
}

void dump_rule_array(const char *name,
                     const GArray *rule_array,
                     FILE *fp)
{
    static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    RuleData *rule;

#ifdef VZIC_DEBUG_PRINT
    fprintf(fp, "\n# Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER/S");
#endif

    for (unsigned int i = 0; i < rule_array->len; i++) {
        rule = &g_array_index(rule_array, RuleData, i);

        fprintf(fp, "Rule\t%s\t%s\t", name, dump_year(rule->from_year));

        if (rule->to_year == rule->from_year) {
            fprintf(fp, "only\t");
        } else {
            fprintf(fp, "%s\t", dump_year(rule->to_year));
        }

        fprintf(fp, "%s\t", rule->type ? rule->type : "-");

        fprintf(fp, "%s\t", months[rule->in_month]);

        fprintf(fp, "%s\t",
                dump_day_coded(rule->on_day_code, rule->on_day_number,
                               rule->on_day_weekday));

        fprintf(fp, "%s\t", dump_time(rule->at_time_seconds, rule->at_time_code, FALSE));

        fprintf(fp, "%s\t", dump_time(rule->save_seconds, TIME_WALL, TRUE));

        fprintf(fp, "%s", rule->letter_s ? rule->letter_s : "-");

        fprintf(fp, "\n");
    }
}

const char *
dump_time(int seconds,
          TimeCode time_code,
          gboolean use_zero)
{
    static char buffer[256];
    const char *sign;
    int hours, minutes;
    const char *code;

    if (time_code == TIME_STANDARD) {
        code = "s";
    } else if (time_code == TIME_UNIVERSAL) {
        code = "u";
    } else {
        code = "";
    }

    if (seconds < 0) {
        seconds = -seconds;
        sign = "-";
    } else {
        sign = "";
    }

    hours = seconds / 3600;
    minutes = (seconds % 3600) / 60;
    seconds = seconds % 60;

    if (use_zero && hours == 0 && minutes == 0 && seconds == 0) {
        return "0";
    } else if (seconds == 0) {
        sprintf(buffer, "%s%i:%02i%s", sign, hours, minutes, code);
    } else {
        sprintf(buffer, "%s%i:%02i:%02i%s", sign, hours, minutes, seconds, code);
    }

    return buffer;
}

char *
dump_day_coded(DayCode day_code,
               int day_number,
               int day_weekday)
{
    static char buffer[256];
    static const char *weekdays[] = {"Sun", "Mon", "Tue", "Wed",
                                     "Thu", "Fri", "Sat"};

    switch (day_code) {
    case DAY_SIMPLE:
        sprintf(buffer, "%i", day_number);
        break;
    case DAY_WEEKDAY_ON_OR_AFTER:
        sprintf(buffer, "%s>=%i", weekdays[day_weekday], day_number);
        break;
    case DAY_WEEKDAY_ON_OR_BEFORE:
        sprintf(buffer, "%s<=%i", weekdays[day_weekday], day_number);
        break;
    case DAY_LAST_WEEKDAY:
        sprintf(buffer, "last%s", weekdays[day_weekday]);
        break;
    default:
        fprintf(stderr, "Invalid day code: %i\n", day_code);
        exit(1);
    }

    return buffer;
}

const char *
dump_year(int year)
{
    static char buffer[256];

    if (year == YEAR_MINIMUM) {
        return "min";
    }
    if (year == YEAR_MAXIMUM) {
        return "max";
    }

    sprintf(buffer, "%i", year);
    return buffer;
}

void dump_time_zone_names(GList *names,
                          const char *output_dir,
                          GHashTable *zones_hash)
{
    char filename[PATHNAME_BUFFER_SIZE];
    const char *zone_name_in_hash = NULL;
    char strings_filename[PATHNAME_BUFFER_SIZE];
    FILE *fp, *strings_fp = NULL;
    GList *elem;

    sprintf(filename, "%s/zones.tab", output_dir);
    sprintf(strings_filename, "%s/zones.h", output_dir);

    fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Couldn't create file: %s\n", filename);
        exit(1);
    }

    if (VzicDumpZoneTranslatableStrings) {
        strings_fp = fopen(strings_filename, "w");
        if (!strings_fp) {
            fprintf(stderr, "Couldn't create file: %s\n", strings_filename);
            exit(1);
        }
    }

    names = g_list_sort(names, (GCompareFunc)strcmp);

    elem = names;
    while (elem) {
        char *zone_name = (char *)elem->data;
        const ZoneDescription *zone_desc = g_hash_table_lookup(zones_hash, zone_name);

        /* SPECIAL CASES: These timezones are links from other zones and are
           almost exactly the same - they are basically there so users can find
           them a bit easier. But they don't have entries in the zone.tab file,
           so we use the entry from the timezone linked from. */
        if (!zone_desc) {
            if (!strcmp(zone_name, "America/Indiana/Indianapolis")) {
                zone_name_in_hash = "America/Indianapolis";
            } else if (!strcmp(zone_name, "America/Kentucky/Louisville")) {
                zone_name_in_hash = "America/Louisville";
            } else if (!strcmp(zone_name, "Asia/Istanbul")) {
                zone_name_in_hash = "Europe/Istanbul";
            } else if (!strcmp(zone_name, "Europe/Nicosia")) {
                zone_name_in_hash = "Asia/Nicosia";
            }

            if (zone_name_in_hash) {
                zone_desc = g_hash_table_lookup(zones_hash, zone_name_in_hash);
            }
        }

        if (zone_desc) {
            fprintf(fp, "%+04i%02i%02i %+04i%02i%02i %s\n",
                    zone_desc->latitude[0], zone_desc->latitude[1],
                    zone_desc->latitude[2],
                    zone_desc->longitude[0], zone_desc->longitude[1],
                    zone_desc->longitude[2],
                    zone_name);
        } else {
            fprintf(fp, "%s\n", zone_name);
        }

        if (strings_fp) {
            fprintf(strings_fp, "N_(\"%s\");\n", zone_name);
        }

        elem = elem->next;
    }

    fclose(fp);

    if (strings_fp) {
        fclose(strings_fp);
    }
}
