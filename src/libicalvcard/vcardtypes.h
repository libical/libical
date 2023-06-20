#ifndef VCARDTYPES_H
#define VCARDTYPES_H

#include "libical_vcard_export.h"

typedef struct vcardgeotype {
    const char *uri;
    struct {
        //        double lat;
        //        double lon;
        const char *lat;
        const char *lon;
    } coords;
} vcardgeotype;

typedef struct vcardtztype {
    const char *tzid;
    const char *uri;
    int utcoffset;
} vcardtztype;

#endif /* !VCARDTYPES_H */
