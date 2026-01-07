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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "vzic.h"
#include "vzic-parse.h"
#include "vzic-dump.h"
#include "vzic-output.h"

/*
 * Global command-line options.
 */

/* By default we output Outlook-compatible output. If --pure is used we
   output pure output, with no changes to be compatible with Outlook. */
gboolean VzicPureOutput = FALSE;
gboolean VzicWithArtifacts = FALSE;

gboolean VzicDumpOutput = FALSE;
gboolean VzicDumpChanges = FALSE;
gboolean VzicDumpZoneNamesAndCoords = TRUE;
gboolean VzicDumpZoneTranslatableStrings = TRUE;
gboolean VzicNoRRules = FALSE;
gboolean VzicNoRDates = FALSE;
const char *VzicOutputDir = "zoneinfo";
char *VzicUrlPrefix = NULL;
char *VzicOlsonDir = NULL;

GList *VzicTimeZoneNames = NULL;

#if !defined(VZIC_LIBRARY)
static void
convert_olson_files(GPtrArray *olson_filenames);

static void free_zone_data(GArray *zone_data);
static void free_rule_array(gpointer key,
                            gpointer value,
                            gpointer data);
static void free_link_data(gpointer key,
                           gpointer value,
                           gpointer data);

static void
usage(void)
{
    fprintf(stderr, "Usage: vzic [--dump] [--dump-changes] [--no-rrules] [--no-rdates] [--pure] [--gen-links] [--output-dir <directory>] [--url-prefix <url>] --olson-dir <directory>\n");

    exit(1);
}

int main(int argc,
         char *argv[])
{
    int i;
    char directory[PATHNAME_BUFFER_SIZE];
    char filename[PATHNAME_BUFFER_SIZE];
    GHashTable *zones_hash;
    GPtrArray *olson_filenames;

    /*
     * Command-Line Option Parsing.
     */
    for (i = 1; i < argc; i++) {
        /* User Options. */

        /* --pure: Output the perfect VCALENDAR data, which Outlook won't parse
         * as it has problems with certain iCalendar constructs.
         */
        if (!strcmp(argv[i], "--pure")) {
            VzicPureOutput = TRUE;
        }

        /* --with-artifacts: Include tzdata artifacts (transition time types) in the "pure" iCalendar data.
         * These artifacts can be used to construct TZif files from iCalendar.
         */
        else if (!strcmp(argv[i], "--with-artifacts")) {
            VzicWithArtifacts = TRUE;
        }

        /* --output-dir: specify where to output all the files beneath.
         * The default is the current directory.
         */
        else if (argc > i + 1 && !strcmp(argv[i], "--output-dir")) {
            VzicOutputDir = argv[++i];
        }

        /* --url-prefix: Used as the base for the TZURL property in each VTIMEZONE.
         * The default is to not output TZURL properties.
         */
        else if (argc > i + 1 && !strcmp(argv[i], "--url-prefix")) {
            int length;
            VzicUrlPrefix = argv[++i];
            /* remove the trailing '/' if there is one */
            length = strlen(VzicUrlPrefix);
            if (VzicUrlPrefix[length - 1] == '/') {
                VzicUrlPrefix[length - 1] = '\0';
            }
        }

        else if (argc > i + 1 && !strcmp(argv[i], "--olson-dir")) {
            VzicOlsonDir = argv[++i];
        }

        /* Debugging Options.*/

        /* --dump: Dump the Rule and Zone data that we parsed from the Olson timezone files.
         * This is used to test the parsing code.
         */
        else if (!strcmp(argv[i], "--dump")) {
            VzicDumpOutput = TRUE;
        }

        /* --dump-changes: Dumps a list of times when each timezone changed,
         * and the new local time offset from UTC.
         */
        else if (!strcmp(argv[i], "--dump-changes")) {
            VzicDumpChanges = TRUE;
        }

        /* --no-rrules: Don't output RRULE properties in the VTIMEZONEs.
         * Instead it will just output RDATEs for each year up to a certain year.
         */
        else if (!strcmp(argv[i], "--no-rrules")) {
            VzicNoRRules = TRUE;
        }

        /* --no-rdates: Don't output multiple RDATEs in a single VTIMEZONE component.
         * Instead they will be output separately.
         */
        else if (!strcmp(argv[i], "--no-rdates")) {
            VzicNoRDates = TRUE;
        } else {
            usage();
        }
    }

    /*
     * Verify the olson dir
     */
    if (!VzicOlsonDir) {
        fprintf(stderr, "You forgot to specify the --olson-dir\n");
        exit(1);
    }

    /*
    * Create any necessary directories.
    */
    ensure_directory_exists(VzicOutputDir);

    if (VzicDumpOutput) {
        /* Create the directories for the dump output, if they don't exist. */
        sprintf(directory, "%s/ZonesVzic", VzicOutputDir);
        ensure_directory_exists(directory);
        sprintf(directory, "%s/RulesVzic", VzicOutputDir);
        ensure_directory_exists(directory);
    }

    if (VzicDumpChanges) {
        /* Create the directory for the changes output, if it doesn't exist. */
        sprintf(directory, "%s/ChangesVzic", VzicOutputDir);
        ensure_directory_exists(directory);
    }

    /* Decide which Olson timezone files to convert. */
    olson_filenames = g_ptr_array_new();
    g_ptr_array_add(olson_filenames, (char *)"africa");
    g_ptr_array_add(olson_filenames, (char *)"antarctica");
    g_ptr_array_add(olson_filenames, (char *)"asia");
    g_ptr_array_add(olson_filenames, (char *)"australasia");
    g_ptr_array_add(olson_filenames, (char *)"europe");
    g_ptr_array_add(olson_filenames, (char *)"northamerica");
    g_ptr_array_add(olson_filenames, (char *)"southamerica");

    /* These are backwards-compatibility and weird stuff. */
    g_ptr_array_add(olson_filenames, (char *)"backward");
    g_ptr_array_add(olson_filenames, (char *)"etcetera");

    /* these top-level dirs are ignored. some have been removed over the years:
    g_ptr_array_add(olson_filenames, (char *)"leapseconds");
    g_ptr_array_add(olson_filenames, (char *)"pacificnew");
    g_ptr_array_add(olson_filenames, (char *)"solar87");
    g_ptr_array_add(olson_filenames, (char *)"solar88");
    g_ptr_array_add(olson_filenames, (char *)"solar89");
    */

    /* Convert the Olson timezone files. */
    convert_olson_files(olson_filenames);

    /* Output the timezone names and coordinates in a zone.tab file,
     * and the translatable strings to feed to gettext.
     */
    if (VzicDumpZoneNamesAndCoords) {
        sprintf(filename, "%s/zone.tab", VzicOlsonDir);
        zones_hash = parse_zone_tab(filename);

        dump_time_zone_names(VzicTimeZoneNames, VzicOutputDir, zones_hash);
        g_hash_table_destroy(zones_hash);
    }

    g_ptr_array_free(olson_filenames, TRUE);

    return 0;
}

static void
convert_olson_files(GPtrArray *olson_filenames)
{
    int max_until_year = 0;

    GArray *zone_data = g_array_new(FALSE, FALSE, sizeof(ZoneData));
    GHashTable *rule_data = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable *link_data = g_hash_table_new(g_str_hash, g_str_equal);

    for (unsigned int i = 0; i < olson_filenames->len; i++) {
        const char *olson_filename = g_ptr_array_index(olson_filenames, i);
        char input_filename[PATHNAME_BUFFER_SIZE];
        int file_max_until_year;

        sprintf(input_filename, "%s/%s", VzicOlsonDir, olson_filename);
        parse_olson_file(input_filename, zone_data, rule_data, link_data,
                         &file_max_until_year);
        if (file_max_until_year > max_until_year) {
            max_until_year = file_max_until_year;
        }

        if (VzicDumpOutput) {
            char dump_filename[PATHNAME_BUFFER_SIZE];
            sprintf(dump_filename, "%s/ZonesVzic/%s", VzicOutputDir, olson_filename);
            dump_zone_data(zone_data, dump_filename);

            sprintf(dump_filename, "%s/RulesVzic/%s", VzicOutputDir, olson_filename);
            dump_rule_data(rule_data, dump_filename);
        }
    }

    output_vtimezone_files(VzicOutputDir, zone_data, rule_data, link_data,
                           max_until_year);

    free_zone_data(zone_data);
    g_hash_table_foreach(rule_data, free_rule_array, NULL);
    g_hash_table_destroy(rule_data);
    g_hash_table_foreach(link_data, free_link_data, NULL);
    g_hash_table_destroy(link_data);
}

/*
 * Functions to free the data structures.
 */
static void
free_zone_data(GArray *zone_data)
{
    ZoneLineData *zone_line;

    for (unsigned int i = 0; i < zone_data->len; i++) {
        ZoneData *zone = &g_array_index(zone_data, ZoneData, i);

        g_free(zone->zone_name);

        for (unsigned int j = 0; j < zone->zone_line_data->len; j++) {
            zone_line = &g_array_index(zone->zone_line_data, ZoneLineData, j);

            g_free(zone_line->rules);
            g_free(zone_line->format);
        }

        g_array_free(zone->zone_line_data, TRUE);
    }

    g_array_free(zone_data, TRUE);
}

static void
free_rule_array(gpointer key,
                gpointer value,
                gpointer data)
{
    (void)data; /* unused */
    char *name = key;
    GArray *rule_array = value;

    for (unsigned int i = 0; i < rule_array->len; i++) {
        RuleData *rule = &g_array_index(rule_array, RuleData, i);

        if (!rule->is_shallow_copy) {
            g_free(rule->type);
            g_free(rule->letter_s);
        }
    }

    g_array_free(rule_array, TRUE);

    g_free(name);
}

static void
free_link_data(gpointer key,
               gpointer value,
               gpointer data)
{
    (void)value; /* unused */
    GList *link = data;

    g_free(key);

    while (link) {
        g_free(link->data);
        link = link->next;
    }

    g_list_free(data);
}
#endif
