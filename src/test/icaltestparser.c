/*======================================================================
 FILE: icaltestparser.c
 CREATOR: eric 20 June 1999

 SPDX-FileCopyrightText: 1999 The Software Studio <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 The original author is Eric Busboom
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libical/ical.h"

extern int yydebug;

/* Have the parser fetch data from stdin */

static char *read_stream(char *s, size_t size, void *d)
{
    char *c = fgets(s, (int)size, (FILE *)d);

    return c;
}

int main(int argc, const char *argv[])
{
    FILE *stream;
    icalparser *parser = icalparser_new();

    if (argc != 2) {
        fprintf(stderr, "Usage: parser [file.ics]\n");
        return 0;
    }
    stream = fopen(argv[1], "r");
    if (stream == (FILE *)NULL) {
        fprintf(stderr, "Cannot open file \"%s\" for reading\n", argv[1]);
        return 1;
    }

    icalparser_set_gen_data(parser, stream);

    bool cont;
    do {
        char *line = icalparser_get_line(parser, read_stream);

        icalcomponent *c = icalparser_add_line(parser, line);
        if (c != 0) {
            /*icalcomponent_convert_errors(c); */
            printf("%s", icalcomponent_as_ical_string(c));
            printf("\n---------------\n");
            icalcomponent_free(c);
        }
        cont = false;
        if (line != 0) {
            icalmemory_free_buffer(line);
            cont = true;
        }
    } while (cont);

    icalparser_free(parser);
    fclose(stream);

    return 0;
}
