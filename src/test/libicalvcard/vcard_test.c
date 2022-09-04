#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "vcard.h"

void strip_errors(vcardcomponent *comp)
{
    vcardproperty *prop, *next;

    for (prop = vcardcomponent_get_first_property(comp, VCARD_XLICERROR_PROPERTY);
         prop; prop = next) {
        next = vcardcomponent_get_next_property(comp, VCARD_XLICERROR_PROPERTY);
        vcardcomponent_remove_property(comp, prop);
        vcardproperty_free(prop);
    }
}

int main(int argc, const char **argv)
{
    const char *fname = argv[1];
    struct stat sbuf;
    int fd = open(fname, O_RDONLY);
    char *data = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s fname\n", argv[0]);
        exit(1);
    }

    fstat(fd, &sbuf);
    data = malloc(sbuf.st_size+1);

    read(fd, data, sbuf.st_size);
    data[sbuf.st_size] = '\0';

    vcardcomponent *card = vcardparser_parse_string(data);
    free(data);

    if (card == NULL) {
        fprintf(stderr, "Failed to parse vCard\n");
        return -1;
    }

    vcardrestriction_check(card);
    vcardcomponent_normalize(card);
    printf("%s\n", vcardcomponent_as_vcard_string(card));
    vcardcomponent_free(card);

    card = vcardcomponent_vanew(VCARD_VCARD_COMPONENT,
                                vcardproperty_new_version(VCARD_VERSION_40),
                                vcardproperty_new_kind(VCARD_KIND_INDIVIDUAL),
                                0);

    if (card == NULL) {
        fprintf(stderr, "Failed to create vCard\n");
        return -1;
    }

    vcardrestriction_check(card);
    printf("\n%s\n", vcardcomponent_as_vcard_string(card));
    strip_errors(card);

    /* Create and add NOTE property */
    vcardstrarray *sa = vcardstrarray_new(10);
    vcardstrarray_append(sa, "1");
    vcardstrarray_append(sa, "2");
    vcardstrarray_append(sa, "3");
    vcardstrarray_add(sa, "3");
    vcardstrarray_remove_element_at(sa, 1);

    vcardproperty *prop =
        vcardproperty_vanew_note("Test vCard",
                                 vcardparameter_new_language("en"),
                                 vcardparameter_new_pid(sa),
                                 0);
    vcardcomponent_add_property(card, prop);
    vcardproperty_set_group(prop, "group1");

    vcardparameter *param = vcardparameter_new(VCARD_TYPE_PARAMETER);
    vcardenumarray_element e = { VCARD_TYPE_WORK, NULL };
    vcardparameter_add_type(param, &e);
    vcardproperty_add_parameter(prop, param);

    param = vcardparameter_new(VCARD_SORTAS_PARAMETER);
    vcardparameter_add_sortas(param, "foo");
    vcardparameter_add_sortas(param, "bar");
    vcardproperty_add_parameter(prop, param);

    /* Create and add FN property */
    prop = vcardproperty_new_fn("Mickey Mouse");
    vcardcomponent_add_property(card, prop);

    /* Create and add CATEGORIES property */
    sa = vcardstrarray_new(2);
    vcardstrarray_append(sa, "zzz");
    vcardstrarray_append(sa, "aaa");
    prop = vcardproperty_new_categories(sa);
    vcardcomponent_add_property(card, prop);

    vcardrestriction_check(card);
    printf("\n%s\n", vcardcomponent_as_vcard_string(card));

    /* Change VERSION from 4.0 to 3.0 */
    prop = vcardcomponent_get_first_property(card, VCARD_VERSION_PROPERTY);
    vcardproperty_set_version(prop, VCARD_VERSION_30);

    vcardrestriction_check(card);
    printf("\n%s\n", vcardcomponent_as_vcard_string(card));

    vcardcomponent_free(card);

    return 0;
}
