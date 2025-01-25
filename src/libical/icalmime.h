/*======================================================================
 FILE: icalmime.h
 CREATOR: eric 26 July 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/

#ifndef ICALMIME_H
#define ICALMIME_H

#include "libical_ical_export.h"
#include "icalcomponent.h"

LIBICAL_ICAL_EXPORT icalcomponent *icalmime_parse(char *(*line_gen_func)(char *s, size_t size, void *d), void *data);

#endif /* !ICALMIME_H */
