#include <stdio.h>

#include "icalcomponent.h"
#include "vcard.h"

int main()
{
    icalcomponent *comp = vcard_hello_world();
    if (icalcomponent_isa(comp) != ICAL_VEVENT_COMPONENT) {
        fprintf(stderr, "Unexpected component\n");
        return -1;
    }
    return 0;
}
