#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "vcard.h"

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

    printf("%s\n", vcardcomponent_as_vcard_string(card));
    vcardcomponent_free(card);

    vcardstrarray *sa = vcardstrarray_new(10);
    vcardstrarray_append(sa, "1");
    vcardstrarray_append(sa, "2");
    vcardstrarray_append(sa, "3");
    vcardstrarray_add(sa, "3");
    vcardstrarray_remove_element_at(sa, 1);

    card = vcardcomponent_vanew(VCARD_VCARD_COMPONENT,
                                vcardproperty_new_version(VCARD_VERSION_40),
                                vcardproperty_new_fn("Mickey Mouse"),
                                vcardproperty_new_kind(VCARD_KIND_INDIVIDUAL),
                                0);

    if (card == NULL) {
        fprintf(stderr, "Failed to create vCard\n");
        return -1;
    }

    vcardproperty *prop =
        vcardproperty_vanew_note("Test vCard",
                                 vcardparameter_new_language("en"),
                                 vcardparameter_new_pid(sa),
                                 0);
    vcardproperty_set_group(prop, "group1");

    vcardparameter *param = vcardparameter_new(VCARD_TYPE_PARAMETER);
    vcardenumarray_element e = { VCARD_TYPE_WORK, NULL };
    vcardparameter_add_type(param, &e);
    vcardproperty_add_parameter(prop, param);

    param = vcardparameter_new(VCARD_SORTAS_PARAMETER);
    vcardparameter_add_sortas(param, "foo");
    vcardparameter_add_sortas(param, "bar");
    vcardproperty_add_parameter(prop, param);

    vcardcomponent_add_property(card, prop);

    printf("\n%s\n", vcardcomponent_as_vcard_string(card));
    vcardcomponent_free(card);

    return 0;
}
