/*======================================================================
 FILE: icalproperty_p.h

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/

#ifndef ICALPROPERTY_P_H
#define ICALPROPERTY_P_H

#include "icalproperty.h"
#include "icalcomponent.h"

/* Check validity and attributes of icalproperty_kind and icalvalue_kind pair */
LIBICAL_ICAL_NO_EXPORT bool icalproperty_value_kind_is_valid(icalproperty_kind pkind,
                                                             icalvalue_kind vkind);
LIBICAL_ICAL_NO_EXPORT bool icalproperty_value_kind_is_multivalued(icalproperty_kind pkind,
                                                                   icalvalue_kind *vkind);
LIBICAL_ICAL_NO_EXPORT bool icalproperty_value_kind_is_default(icalproperty_kind pkind,
                                                               icalvalue_kind vkind);

#endif /* ICALPROPERTY_P_H */
