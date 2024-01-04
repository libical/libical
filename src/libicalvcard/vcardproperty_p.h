/*======================================================================
 FILE: vcardproperty_p.h

======================================================================*/

#ifndef VCARDPROPERTY_P_H
#define VCARDPROPERTY_P_H

#include "vcardproperty.h"

/* Check validity and attributes of vcardproperty_kind and vcardvalue_kind pair */
LIBICAL_VCARD_NO_EXPORT int vcardproperty_value_kind_is_valid(vcardproperty_kind pkind,
                                                              vcardvalue_kind vkind);
LIBICAL_VCARD_NO_EXPORT int vcardproperty_value_kind_is_multivalued(vcardproperty_kind pkind,
                                                                    vcardvalue_kind *vkind);
LIBICAL_VCARD_NO_EXPORT int vcardproperty_value_kind_is_default(vcardproperty_kind pkind,
                                                                vcardvalue_kind vkind);

#endif /* VCARDPROPERTY_P_H */
