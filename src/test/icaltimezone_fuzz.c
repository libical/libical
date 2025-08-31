/*======================================================================*
 * SPDX-FileCopyrightText: 2024 Contributors to the libical project <git@github.com:libical/libical>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * Based on the original code by Zijie Zhao in
 * https://github.com/libical/libical/issues/895
 * ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(NDEBUG)
#undef NDEBUG
#endif

#include <stdlib.h>
#include <stdint.h>

#include "libical/ical.h"

/* cppcheck-suppress constParameter */
int main(int argc, char *argv[])
{
    FILE *fp;
    int fd, r;
    const char *fname;
#if defined(HAVE__FSTAT64)
    struct _stat64 sbuf;
#else
    struct stat sbuf;
#endif
    size_t filesize;
    void *data = NULL;

    if (argc != 2) {
        fname = TEST_DATADIR "/timezonefuzz895";
    } else {
        fname = argv[1];
    }

    fp = fopen(fname, "rb");
    if (fp == (FILE *)NULL) {
        fprintf(stderr, "Error: unable to open %s\n", fname);
        assert(0);
    }

    fd = fileno(fp);
    if (fstat(fd, &sbuf) != 0) {
        fprintf(stderr, "Error: unable to fstat %s\n", fname);
        fclose(fp);
        assert(0);
    }
    filesize = (size_t)sbuf.st_size;
    /* cppcheck-suppress nullPointerRedundantCheck */
    data = malloc(filesize + 1);
    if (!data) {
        fprintf(stderr, "Error: unable to allocate memory\n");
        free(data);
        assert(0);
    }
    /* cppcheck-suppress nullPointerRedundantCheck */
    memset(data, 0, filesize + 1);
    r = read(fd, data, filesize);
    /* cppcheck-suppress doubleFree */
    fclose(fp);

    if (r < 0) {
        fprintf(stderr, "Error: Failed to read data\n");
        /* cppcheck-suppress doubleFree */
        free(data);
        assert(0);
    }
#define LOOKBACK_CHARS 40
    if (r <= LOOKBACK_CHARS) {
        fprintf(stderr, "Error: Failed to read enough data -- more than %d chars is required for this test", LOOKBACK_CHARS);
        /* cppcheck-suppress doubleFree */
        free(data);
        assert(0);
    }
    size_t ics_len = filesize - LOOKBACK_CHARS;
    char *ics_str = (char *)malloc(ics_len + 1);
    if (!ics_str) {
        return 0;
    }
    memset(ics_str, '\0', ics_len + 1);
    /* cppcheck-suppress deallocuse */
    memcpy(ics_str, data, ics_len);
    icalcomponent *vtz = icalcomponent_new_from_string(ics_str);
    free(ics_str);
    if (!vtz) {
        return 0;
    }
    if (!icalcomponent_is_valid(vtz)) {
        return 0;
    }

    char *p = (char *)data;
    p = p + ics_len;

    // Construct a plausible start time.
    int start_year = (p[0] % 201) + 1900; // Range: 1900-2100.
    int start_month = (p[1] % 12) + 1;    // 1-12
    int start_day = (p[2] % 28) + 1;      // 1-28
    int start_hour = p[3] % 24;
    int start_minute = p[4] % 60;
    int start_second = p[5] % 60;

    // Construct a plausible end time.
    int end_year = (p[6] % 201) + 1900;
    int end_month = (p[7] % 12) + 1;
    int end_day = (p[8] % 28) + 1;
    int end_hour = p[9] % 24;
    int end_minute = p[10] % 60;
    int end_second = p[11] % 60;

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif
    // Format the times into strings using the standard iCalendar datetime format:
    // "YYYYMMDDThhmmss".
    char start_time_str[17] = {0};
    char end_time_str[17] = {0};
    snprintf(start_time_str, sizeof(start_time_str), "%04d%02d%02dT%02d%02d%02d",
             start_year, start_month, start_day, start_hour, start_minute,
             start_second);
    snprintf(end_time_str, sizeof(end_time_str), "%04d%02d%02dT%02d%02d%02d",
             end_year, end_month, end_day, end_hour, end_minute, end_second);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

    icaltimetype start = icaltime_from_string(start_time_str);
    if (!icaltime_is_valid_time(start)) {
        return 0;
    }
    icaltimetype end = icaltime_from_string(end_time_str);
    if (!icaltime_is_valid_time(end)) {
        return 0;
    }
    int ms_compatible = p[12] % 2;

    icaltimezone_truncate_vtimezone(vtz, start, end, ms_compatible);
    icalcomponent_free(vtz);

    free(data);
    return 0;
}
