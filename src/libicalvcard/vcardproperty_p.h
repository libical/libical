/*======================================================================
 FILE: vcardproperty_p.c

 CREATOR: Ken Murchison 24 Aug 2022

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

 ======================================================================*/

#ifndef VCARDPROPERTY_P_H
#define VCARDPROPERTY_P_H

#include "vcardproperty.h"

/* Check validity and attributes of vcardproperty_kind and vcardvalue_kind pair */
LIBICAL_VCARD_NO_EXPORT bool vcardproperty_value_kind_is_valid(vcardproperty_kind pkind,
                                                               vcardvalue_kind vkind);
LIBICAL_VCARD_NO_EXPORT bool vcardproperty_value_kind_is_multivalued(vcardproperty_kind pkind,
                                                                     vcardvalue_kind *vkind);
LIBICAL_VCARD_NO_EXPORT bool vcardproperty_value_kind_is_default(vcardproperty_kind pkind,
                                                                 vcardvalue_kind vkind);

#endif /* VCARDPROPERTY_P_H */
