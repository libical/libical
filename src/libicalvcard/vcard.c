#include "vcard.h"
#include "libicalvcard_export.h"

LIBICALVCARD_EXPORT
icalcomponent *vcard_hello_world()
{
    return icalcomponent_new_vevent();
}
