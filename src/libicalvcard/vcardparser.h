#ifndef VCARDPARSER_H
#define VCARDPARSER_H

#include "libical_vcard_export.h"

#include "vcardcomponent.h"

LIBICAL_VCARD_EXPORT vcardcomponent *vcardparser_parse_string(const char *str);

#endif /* VCARDPARSER_H */

