#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "icalerror.h"
#include "icalmemory.h"
#include "vcardstructured.h"

vcardstructuredtype *vcardstructured_new()
{
    vcardstructuredtype *s;

    s = (vcardstructuredtype *) icalmemory_new_buffer(sizeof(vcardstructuredtype));
    if (!s) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return NULL;
    }

    memset(s, 0, sizeof(vcardstructuredtype));

    return s;
}

void vcardstructured_free(vcardstructuredtype *s)
{
    unsigned i;

    for (i = 0; i < s->num_fields; i++)
        vcardstrarray_free(s->field[i]);
    icalmemory_free_buffer((void *)s);
}
