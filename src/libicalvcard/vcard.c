#include "vcard.h"
#include "libical_vcard_export.h"

LIBICAL_VCARD_EXPORT
icalcomponent *vcard_hello_world()
{
    return icalcomponent_new_vevent();
}
