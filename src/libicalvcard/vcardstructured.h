#ifndef VCARDSTRUCTURED_H
#define VCARDSTRUCTURED_H

#include "libical_vcard_export.h"

#include "vcardstrarray.h"

#define VCARD_MAX_STRUCTURED_FIELDS 20  // Extended ADR currently uses 18

typedef struct vcardstructuredtype {
    unsigned num_fields;
    vcardstrarray *field[VCARD_MAX_STRUCTURED_FIELDS];
} vcardstructuredtype;

LIBICAL_VCARD_EXPORT vcardstructuredtype *vcardstructured_new(void);

LIBICAL_VCARD_EXPORT char *vcardstructured_as_vcard_string_r(const vcardstructuredtype *s,
                                                             int is_param);

LIBICAL_VCARD_EXPORT vcardstructuredtype *vcardstructured_from_string(const char *s);

LIBICAL_VCARD_EXPORT void vcardstructured_free(vcardstructuredtype *s);

#endif /* VCARDSTRUCTURED_H */
