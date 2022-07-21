#include <stdio.h>

#include "vcard.h"

int main()
{
    vcard *card =
        vcard_vanew(VCARD_VERSION_40,
                    vcardproperty_new_fn("Mickey Mouse"),
                    vcardproperty_new_kind(VCARD_KIND_INDIVIDUAL),
                    vcardproperty_vanew_note("Test vCard",
                                             vcardparameter_new_language("en"),
                                             0),
                    0);
    if (card == NULL) {
        fprintf(stderr, "Failed to create vCard\n");
        return -1;
    }

    printf("%s\n", vcard_as_vcard_string(card));
    vcard_free(card);
    return 0;
}
