/*======================================================================
 FILE: icalrecur_test.c
 CREATOR: Ken Murchison 26 September 2014

 SPDX-FileCopyrightText: 2000 Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
======================================================================*/

/*
 * Program for testing libical recurrence iterator on RFC 5545 examples.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libical/ical.h>
#include <stdlib.h>

struct recur {
    int line_no;
    char dtstart[100];
    char rrule[1024];
    char start_at[100];
    char instances[2000];
};

int check_and_copy_field(const char *line, const char *pref, char *field, size_t field_size)
{
    size_t l = strlen(pref);
    if (strncmp(line, pref, l) != 0) {
        return 0;
    }

    size_t data_size = strlen(line) - l;
    if (data_size >= field_size) {
        return 1;
    }

    memcpy(field, &line[l], data_size + 1);
    return 0;
}

static void print_error_hdr(const struct recur *r)
{
    fprintf(stderr, "Test case at line %d failed.\n", r->line_no);
    fprintf(stderr, "RRULE:%s\n", r->rrule);
    fprintf(stderr, "DTSTART:%s\n", r->dtstart);
    if (r->start_at[0]) {
        fprintf(stderr, "START-AT:%s\n", r->start_at);
    }
}

static void reverse_array(char *str, size_t len)
{
    for (size_t i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}

static void reverse_instances(char *str)
{
    size_t l = strlen(str);
    reverse_array(str, l);

    size_t current_instance_start = 0;

    for (size_t i = 0; i < l; i++) {
        if (str[i] == ',') {
            reverse_array(&str[current_instance_start], i - current_instance_start);
            current_instance_start = i + 1;
        }
    }

    reverse_array(&str[current_instance_start], l - current_instance_start);
}

static char *skip_first(char *instances)
{
    while (*instances && (*instances != ',')) {
        instances++;
    }

    if (*instances) {
        instances++;
    }

    return instances;
}

static char *skip_until(char *instances, icaltimetype t, int order)
{
    char *start = instances;
    while (1) {
        char *next = skip_first(start);
        if (!next) {
            return 0;
        }

        char tmp = next[-1];
        if (next[-1] == ',') {
            next[-1] = 0;
        }
        icaltimetype current = icaltime_from_string(start);
        next[-1] = tmp;

        if ((icaltime_compare(current, t) * order) >= 0) {
            return start;
        }

        start = next;
    }
}

/* cppcheck-suppress constParameter */
int main(int argc, char *argv[])
{
    /* Default to RFC 5545 tests */
    const char *file_name = "icalrecur_test.txt";
    int verbose = 0;

    /* Do not use getopt for command line parsing -- for portability on Windows */
    for (int i = 1; i < argc; ++i) {
        if ((strncmp(argv[i], "-f", 2) == 0) && (argc > i + 1)) {
            file_name = argv[++i];
            continue;
        }

        if (strncmp(argv[i], "-v", 2) == 0) { /* Verbose output to stdout */
            verbose = 1;
            continue;
        }

        fprintf(stderr, "usage: %s [-f <input file>]\n", argv[0]);
        return (1);
    }

    FILE *fp = fopen(file_name, "r");

    if (fp == NULL) {
        fprintf(stderr, "unable to open the input file '%s'\n", file_name);
        return (1);
    }

    if (verbose) {
        printf("Processing file '%s'\n", file_name);
    }

    struct recur r;
    int eof = 0;
    int nof_errors = 0;
    int nof_failed_tests = 0;
    int nof_tests = 0;
    int line_no = 0;

    memset(&r, 0, sizeof(r));

    do {
        char line[2048];
        int yield;

        if (fgets(line, sizeof(line), fp)) {
            line_no++;

            size_t l = strlen(line);
            if (l > 0 && line[l - 1] == '\n') {
                line[--l] = '\0';
            }

            if (l == 0) {
                yield = 1;
            } else if (line[0] == '#') {
                continue;
            } else {
                int parse_err = 0;

                if (r.rrule[0] == 0) {
                    r.line_no = line_no;
                }

                parse_err = parse_err || check_and_copy_field(line, "RRULE:", r.rrule, sizeof(r.rrule));
                parse_err = parse_err || check_and_copy_field(line, "DTSTART:", r.dtstart, sizeof(r.dtstart));
                parse_err = parse_err || check_and_copy_field(line, "START-AT:", r.start_at, sizeof(r.start_at));
                parse_err = parse_err || check_and_copy_field(line, "INSTANCES:", r.instances, sizeof(r.instances));

                if (parse_err) {
                    nof_errors++;
                    fprintf(stderr, "line buffer overflow: %s\n", line);
                }

                yield = 0;
            }
        } else {
            eof = 1;
            yield = 1;
        }

        yield &= (r.rrule[0] != 0);

        if (yield) {
            struct icalrecurrencetype *rrule;
            struct icaltimetype dtstart, next;
            icalrecur_iterator *ritr;
            const char *sep = "";
            char actual_instances[2048];
            int actual_instances_len = 0;
            struct icaltimetype start = icaltime_null_time();
            int test_error = 0;

            nof_tests++;

            if (verbose) {
                printf("Processing line %d: %s\n", r.line_no, r.rrule);
            }

            memset(&actual_instances[0], 0, sizeof(actual_instances));
            actual_instances_len = 0;

            dtstart = icaltime_from_string(r.dtstart);
            rrule = icalrecurrencetype_new_from_string(r.rrule);
            ritr = icalrecur_iterator_new(rrule, dtstart);

            char *instances = r.instances;

            if (!ritr) {
                snprintf(&actual_instances[actual_instances_len],
                         sizeof(actual_instances) - (size_t)actual_instances_len,
                         " *** %s", icalerror_strerror(icalerrno));
            } else {
                if (r.start_at[0]) {
                    start = icaltime_from_string(r.start_at);
                    icalrecur_iterator_set_start(ritr, start);

                    instances = skip_until(instances, start, 1);
                }

                for (next = icalrecur_iterator_next(ritr);
                     !icaltime_is_null_time(next);
                     next = icalrecur_iterator_next(ritr)) {
                    int n = snprintf(&actual_instances[actual_instances_len],
                                     sizeof(actual_instances) - (size_t)actual_instances_len,
                                     "%s%s", sep, icaltime_as_ical_string(next));
                    if (n < 0 || (size_t)n > sizeof(actual_instances) - (size_t)actual_instances_len) {
                        break;
                    }
                    actual_instances_len += n;
                    sep = ",";
                }
            }

            if (strcmp(instances, actual_instances)) {
                nof_errors++;
                test_error = 1;

                print_error_hdr(&r);
                fprintf(stderr, "Expected INSTANCES:%s\n", instances);
                fprintf(stderr, "Actual   INSTANCES:%s\n", actual_instances);
                fprintf(stderr, "\n");
            }

            if (ritr) {
                instances = r.instances;

                reverse_instances(instances);

                instances = skip_first(instances);

                if (r.start_at[0]) {
                    icalrecur_iterator_set_range(ritr, start, dtstart);
                    instances = skip_until(instances, start, -1);
                }

                sep = "";
                memset(&actual_instances[0], 0, sizeof(actual_instances));
                actual_instances_len = 0;

                for (next = icalrecur_iterator_prev(ritr);
                     !icaltime_is_null_time(next);
                     next = icalrecur_iterator_prev(ritr)) {
                    int n = snprintf(&actual_instances[actual_instances_len],
                                     sizeof(actual_instances) - (size_t)actual_instances_len,
                                     "%s%s", sep, icaltime_as_ical_string(next));
                    if (n < 0 || (size_t)n >= sizeof(actual_instances) - (size_t)actual_instances_len) {
                        break;
                    }
                    actual_instances_len += n;
                    sep = ",";
                }

                if (strcmp(instances, actual_instances)) {
                    nof_errors++;

                    if (!test_error) {
                        print_error_hdr(&r);
                    }

                    test_error = 1;

                    fprintf(stderr, "Expected PREV-INSTANCES:%s\n", instances);
                    fprintf(stderr, "Actual   PREV-INSTANCES:%s\n", actual_instances);
                    fprintf(stderr, "\n");
                }
            }

            icalrecur_iterator_free(ritr);
            icalrecurrencetype_unref(rrule);

            if (test_error) {
                nof_failed_tests++;
            }

            memset(&r, 0, sizeof(r));
        }
    } while (!eof);

    fclose(fp);

    printf("%d tests failed out of %d.\n", nof_failed_tests, nof_tests);

    return (nof_errors == 0) ? 0 : 1;
}
