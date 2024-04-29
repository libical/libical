/*======================================================================
 FILE: icalvcal.h
 CREATOR: eric 25 May 00

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/

#ifndef ICALVCAL_H
#define ICALVCAL_H

#include "libical_vcal_export.h"
#include "vobject.h"
#include "icalcomponent.h"

/* These are used as default values if the values are missing in the vCalendar
   file. Gnome Calendar, for example, does not save the URL of the audio alarm,
   so we have to add a value here to make a valid iCalendar object. */
typedef struct _icalvcal_defaults icalvcal_defaults;
struct _icalvcal_defaults {
    char *alarm_audio_url;
    char *alarm_audio_fmttype;
    char *alarm_description;
};

/* Convert a vObject into an icalcomponent */

LIBICAL_VCAL_EXPORT icalcomponent *icalvcal_convert(VObject *object);

LIBICAL_VCAL_EXPORT icalcomponent *icalvcal_convert_with_defaults(VObject *object, icalvcal_defaults *defaults);

#endif /* !ICALVCAL_H */
