/*======================================================================
 *
 * SPDX-FileCopyrightText: 2024 Contributors to the libical project <git@github.com:libical/libical>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * Based on the original code by Gabe Sherman in
 * https://github.com/libical/libical/issues/678
 *
 * ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(NDEBUG)
#undef NDEBUG
#endif

#include <stdlib.h>

#include "libical/ical.h"

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
    icalcomponent *comp;

    if (argc != 2) {
        fname = TEST_DATADIR "/poc-01";
    } else {
        fname = argv[1];
    }

    fp = fopen(fname, "rb");
    if (fp == (FILE *)NULL) {
        fprintf(stderr, "Error: unable to open %s\n", fname);
        assert(0);
    }

    fd = fileno(fp);
    fstat(fd, &sbuf);
    filesize = sbuf.st_size; //to make fortify compile happy
    data = malloc(filesize + 1);
    memset(data, 0, filesize + 1);

    r = read(fd, data, filesize);
    fclose(fp);

    if (r < 0) {
        fprintf(stderr, "Failed to read data\n");
        free(data);
        assert(0);
    }

    comp = icalcomponent_new_from_string(data);
    free(data);

    icalcomponent_normalize(comp);
    icalcomponent_free(comp);

    return 0;
}
