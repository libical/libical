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

char str[] = "BEGIN:VCALENDAR\
PRODID:\"-//RDU Software//NONSGML HandCal//EN\"\
VERSION:2.0\
BEGIN:VTIMEZONE\
TZID:US-Eastern\
BEGIN:STANDARD\
DTSTART:19990404T020000\
RDATE:19990u404xT020000\
TZOFFSETFROM:-0500\
TZOFFSETTO:-0400\
END:STANDARD\
BEGIN:DAYLIGHT\
DTSTART:19990404T020000\
RDATE:19990404T020000\
TZOFFSETFROM:-0500\
TZOFFSETTO:-0400\
TZNAME:EDT\
Dkjhgri:derhvnv;\
BEGIN:dfkjh\
END:dfdfkjh\
END:DAYLIGHT\
END:VTIMEZONE\
BEGIN:VEVENT\
GEO:Bongo\
DTSTAMP:19980309T231000Z\
UID:guid-1.host1.com\
ORGANIZER;ROLE=CHAIR:MAILTO:mrbig@host.com\
ATTENDEE;RSVP=TRUE;ROLE=REQ-PARTICIPANT;CUTYPE=GROUP\
 :MAILTO:employee-A@host.com\
DESCRIPTION:Project XYZ Review Meeting\
CATEGORIES:MEETING\
CLASS:PUBLIC\
CREATED:19980309T130000Z\
SUMMARY:XYZ Project Review\
DTSTART;TZID=US-Eastern:19980312T083000\
DTEND;TZID=US-Eastern:19980312T093000\
LOCATION:1CP Conference Room 4350\
END:VEVENT\
END:VCALENDAR\
";

extern int yydebug;

/* Have the parser fetch data from stdin */

char *read_stream(char *s, size_t size, void *d)
{
    char *c = fgets(s, (int)size, (FILE *)d);

    return c;
}

/* cppcheck-suppress constParameter */
int main(int argc, char *argv[])
{
    char *line;
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

    do {
        line = icalparser_get_line(parser, read_stream);

        icalcomponent *c = icalparser_add_line(parser, line);
        icalmemory_free_buffer(line);

        if (c != 0) {
            /*icalcomponent_convert_errors(c); */
            printf("%s", icalcomponent_as_ical_string(c));
            printf("\n---------------\n");
            icalcomponent_free(c);
        }

    } while (line != 0);

    icalparser_free(parser);
    fclose(stream);

    return 0;
}
