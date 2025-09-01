/*======================================================================
 FILE: icalclassify.h
 CREATOR: eric 21 Aug 2000

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 =========================================================================*/

#ifndef ICALCLASSIFY_H
#define ICALCLASSIFY_H

#include "libical_icalss_export.h"
#include "icalset.h"
#include "icalcomponent.h"

LIBICAL_ICALSS_EXPORT icalproperty_xlicclass icalclassify(icalcomponent *c, icalcomponent *match, const char *user);

LIBICAL_ICALSS_EXPORT icalcomponent *icalclassify_find_overlaps(icalset *set, icalcomponent *comp);

#endif /* ICALCLASSIFY_H */
