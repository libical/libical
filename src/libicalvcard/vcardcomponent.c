/*======================================================================
 FILE: vcardcomponent.c
 CREATOR: Ken Murchison 24 Aug 2022 <murch@fastmailteam.com>

 SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vcardcomponent.h"
#include "icalpvl.h"
#include "vcardparser.h"
#include "vcardproperty_p.h"
#include "vcardrestriction.h"
#include "vcardvalue.h"
#include "icalerror.h"
#include "icalmemory.h"

#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

struct vcardcomponent_impl {
    char id[5];
    vcardcomponent_kind kind;
    vcardproperty *versionp;
    char *x_name;
    icalpvl_list properties;
    icalpvl_elem property_iterator;
    icalpvl_list components;
    icalpvl_elem component_iterator;
    struct vcardcomponent_impl *parent;
};

static void vcardcomponent_add_children(vcardcomponent *impl, va_list args)
{
    void *vp;

    while ((vp = va_arg(args, void *)) != 0) {
        icalassert(vcardproperty_isa_property(vp) != 0);

        vcardcomponent_add_property(impl, (vcardproperty *)vp);
    }
}

static vcardcomponent *vcardcomponent_new_impl(vcardcomponent_kind kind)
{
    vcardcomponent *comp;

    if (!vcardcomponent_kind_is_valid(kind)) {
        return NULL;
    }

    if ((comp = (vcardcomponent *)icalmemory_new_buffer(sizeof(vcardcomponent))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(comp, 0, sizeof(vcardcomponent));

    strcpy(comp->id, "comp");

    comp->kind = kind;
    comp->properties = icalpvl_newlist();
    comp->components = icalpvl_newlist();

    return comp;
}

vcardcomponent *vcardcomponent_new(vcardcomponent_kind kind)
{
    return vcardcomponent_new_impl(kind);
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvarargs"
#endif
vcardcomponent *vcardcomponent_vanew(vcardcomponent_kind kind, ...)
{
    va_list args;

    vcardcomponent *impl = vcardcomponent_new_impl(kind);

    if (impl == 0) {
        return 0;
    }

    va_start(args, kind);
    vcardcomponent_add_children(impl, args);
    va_end(args);

    return impl;
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

vcardcomponent *vcardcomponent_new_from_string(const char *str)
{
    return vcardparser_parse_string(str);
}

vcardcomponent *vcardcomponent_clone(const vcardcomponent *old)
{
    vcardcomponent *clone;
    const vcardproperty *p;
    icalpvl_elem itr;

    icalerror_check_arg_rz((old != 0), "component");

    clone = vcardcomponent_new_impl(old->kind);

    if (clone == 0) {
        return 0;
    }

    for (itr = icalpvl_head(old->properties); itr != 0; itr = icalpvl_next(itr)) {
        p = (vcardproperty *)icalpvl_data(itr);
        vcardcomponent_add_property(clone, vcardproperty_clone(p));
    }

    return clone;
}

void vcardcomponent_free(vcardcomponent *c)
{
    vcardcomponent *comp;

    icalerror_check_arg_rv((c != 0), "component");

    if (c->parent != 0) {
        return;
    }

    if (c->properties != 0) {
        vcardproperty *prop;
        while ((prop = icalpvl_pop(c->properties)) != 0) {
            vcardproperty_set_parent(prop, 0);
            vcardproperty_free(prop);
        }
        icalpvl_free(c->properties);
    }

    while ((comp = icalpvl_data(icalpvl_head(c->components))) != 0) {
        vcardcomponent_remove_component(c, comp);
        vcardcomponent_free(comp);
    }

    icalpvl_free(c->components);

    if (c->x_name != 0) {
        icalmemory_free_buffer(c->x_name);
    }

    c->kind = VCARD_NO_COMPONENT;
    c->properties = 0;
    c->property_iterator = 0;
    c->components = 0;
    c->component_iterator = 0;
    c->x_name = 0;
    c->id[0] = 'X';

    icalmemory_free_buffer(c);
}

char *vcardcomponent_as_vcard_string(vcardcomponent *comp)
{
    char *buf;

    buf = vcardcomponent_as_vcard_string_r(comp);
    if (buf) {
        icalmemory_add_tmp_buffer(buf);
    }
    return buf;
}

char *vcardcomponent_as_vcard_string_r(vcardcomponent *comp)
{
    char *buf;
    char *tmp_buf;
    size_t buf_size = 1024;
    char *buf_ptr = 0;
    icalpvl_elem itr;

    /* RFC6350 explicitly says that the newline is *ALWAYS* a \r\n (CRLF)!!!! */
    const char newline[] = "\r\n";

    vcardcomponent *c;
    vcardproperty *p;
    vcardcomponent_kind kind = vcardcomponent_isa(comp);

    const char *kind_string = NULL;

    icalerror_check_arg_rz((comp != 0), "component");
    icalerror_check_arg_rz((kind != VCARD_NO_COMPONENT),
                           "component kind is VCARD_NO_COMPONENT");

    if (kind != VCARD_X_COMPONENT) {
        kind_string = vcardcomponent_kind_to_string(kind);
    } else {
        kind_string = comp->x_name;
    }

    icalerror_check_arg_rz((kind_string != 0), "Unknown kind of component");

    buf = icalmemory_new_buffer(buf_size);
    if (buf == NULL) {
        return NULL;
    }

    buf_ptr = buf;

    if (kind != VCARD_XROOT_COMPONENT) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, "BEGIN:");
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);

        for (itr = icalpvl_head(comp->properties); itr != 0; itr = icalpvl_next(itr)) {
            p = (vcardproperty *)icalpvl_data(itr);

            icalerror_assert((p != 0), "Got a null property");
            tmp_buf = vcardproperty_as_vcard_string_r(p);

            icalmemory_append_string(&buf, &buf_ptr, &buf_size, tmp_buf);
            icalmemory_free_buffer(tmp_buf);
        }
    }

    for (itr = icalpvl_head(comp->components); itr != 0; itr = icalpvl_next(itr)) {
        c = (vcardcomponent *)icalpvl_data(itr);

        tmp_buf = vcardcomponent_as_vcard_string_r(c);
        if (tmp_buf != NULL) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, tmp_buf);
            icalmemory_free_buffer(tmp_buf);
        }
    }

    if (kind != VCARD_XROOT_COMPONENT) {
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, "END:");
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);
    }

    return buf;
}

bool vcardcomponent_is_valid(const vcardcomponent *component)
{
    return ((strcmp(component->id, "comp") == 0) &&
            (component->kind != VCARD_NO_COMPONENT));
}

vcardcomponent_kind vcardcomponent_isa(const vcardcomponent *component)
{
    icalerror_check_arg_rx((component != 0), "component", VCARD_NO_COMPONENT);

    return component->kind;
}

bool vcardcomponent_isa_component(void *component)
{
    const vcardcomponent *impl = component;

    icalerror_check_arg_rz((component != 0), "component");

    return (strcmp(impl->id, "comp") == 0);
}

void vcardcomponent_add_property(vcardcomponent *comp, vcardproperty *property)
{
    icalerror_check_arg_rv((comp != 0), "component");
    icalerror_check_arg_rv((property != 0), "property");

    icalerror_assert((!vcardproperty_get_parent(property)),
                     "The property has already been added to a vcard. "
                     "Remove the property with vcardcomponent_remove_property "
                     "before calling vcardcomponent_add_property");

    vcardproperty_set_parent(property, comp);

    icalpvl_push(comp->properties, property);

    if (vcardproperty_isa(property) == VCARD_VERSION_PROPERTY) {
        comp->versionp = property;
    }
}

void vcardcomponent_remove_property(vcardcomponent *comp, vcardproperty *property)
{
    icalpvl_elem itr, next_itr;

    icalerror_check_arg_rv((comp != 0), "component");
    icalerror_check_arg_rv((property != 0), "property");

#if defined(ICAL_REMOVE_NONMEMBER_CARD_IS_ERROR)
    icalerror_assert((vcardproperty_get_parent(property)),
                     "The property is not a member of a card");
#else
    if (vcardproperty_get_parent(property) == 0) {
        return;
    }
#endif

    if (vcardproperty_isa(property) == VCARD_VERSION_PROPERTY) {
        comp->versionp = 0;
    }

    for (itr = icalpvl_head(comp->properties); itr != 0; itr = next_itr) {
        next_itr = icalpvl_next(itr);

        if (icalpvl_data(itr) == (void *)property) {
            if (comp->property_iterator == itr) {
                comp->property_iterator = icalpvl_next(itr);
            }

            (void)icalpvl_remove(comp->properties, itr);
            vcardproperty_set_parent(property, 0);
        }
    }
}

int vcardcomponent_count_properties(vcardcomponent *comp,
                                    vcardproperty_kind kind,
                                    int ignore_alts)
{
    int count = 0;
    icalpvl_elem itr;
    vcardstrarray *altids = NULL;

    icalerror_check_arg_rz((comp != 0), "component");

    if (ignore_alts) {
        altids = vcardstrarray_new(2);
    }

    for (itr = icalpvl_head(comp->properties); itr != 0; itr = icalpvl_next(itr)) {
        vcardproperty *prop = (vcardproperty *)icalpvl_data(itr);

        if (kind == VCARD_ANY_PROPERTY || kind == vcardproperty_isa(prop)) {
            if (ignore_alts) {
                /* Like-properties having the same ALTID only get counted once */
                vcardparameter *param =
                    vcardproperty_get_first_parameter(prop,
                                                      VCARD_ALTID_PARAMETER);
                if (param) {
                    const char *altid = vcardparameter_get_altid(param);

                    if (vcardstrarray_find(altids, altid) >= vcardstrarray_size(altids)) {
                        continue;
                    }

                    vcardstrarray_append(altids, altid);
                }
            }
            count++;
        }
    }

    if (ignore_alts) {
        vcardstrarray_free(altids);
    }

    return count;
}

vcardproperty *vcardcomponent_get_current_property(vcardcomponent *comp)
{
    icalerror_check_arg_rz((comp != 0), "card");

    if (comp->property_iterator == 0) {
        return 0;
    }

    return (vcardproperty *)icalpvl_data(comp->property_iterator);
}

vcardproperty *vcardcomponent_get_first_property(vcardcomponent *c,
                                                 vcardproperty_kind kind)
{
    icalerror_check_arg_rz((c != 0), "card");

    for (c->property_iterator = icalpvl_head(c->properties);
         c->property_iterator != 0;
         c->property_iterator = icalpvl_next(c->property_iterator)) {
        vcardproperty *p = (vcardproperty *)icalpvl_data(c->property_iterator);

        if (vcardproperty_isa(p) == kind || kind == VCARD_ANY_PROPERTY) {
            return p;
        }
    }
    return 0;
}

vcardproperty *vcardcomponent_get_next_property(vcardcomponent *c,
                                                vcardproperty_kind kind)
{
    icalerror_check_arg_rz((c != 0), "card");

    if (c->property_iterator == 0) {
        return 0;
    }

    for (c->property_iterator = icalpvl_next(c->property_iterator);
         c->property_iterator != 0;
         c->property_iterator = icalpvl_next(c->property_iterator)) {
        vcardproperty *p = (vcardproperty *)icalpvl_data(c->property_iterator);

        if (vcardproperty_isa(p) == kind || kind == VCARD_ANY_PROPERTY) {
            return p;
        }
    }

    return 0;
}

vcardproperty **vcardcomponent_get_properties(vcardcomponent *comp,
                                              vcardproperty_kind kind);

void vcardcomponent_add_component(vcardcomponent *parent, vcardcomponent *child)
{
    icalerror_check_arg_rv((parent != 0), "parent");
    icalerror_check_arg_rv((child != 0), "child");

    if (child->parent != 0) {
        icalerror_set_errno(ICAL_USAGE_ERROR);
    }

    child->parent = parent;

    icalpvl_push(parent->components, child);
}

void vcardcomponent_remove_component(vcardcomponent *parent,
                                     vcardcomponent *child)
{
    icalpvl_elem itr, next_itr;

    icalerror_check_arg_rv((parent != 0), "parent");
    icalerror_check_arg_rv((child != 0), "child");

    for (itr = icalpvl_head(parent->components); itr != 0; itr = next_itr) {
        next_itr = icalpvl_next(itr);

        if (icalpvl_data(itr) == (void *)child) {
            if (parent->component_iterator == itr) {
                /* Don't let the current iterator become invalid */

                /* HACK. The semantics for this are troubling. */
                parent->component_iterator = icalpvl_next(parent->component_iterator);
            }
            (void)icalpvl_remove(parent->components, itr);
            child->parent = 0;
            break;
        }
    }
}

int vcardcomponent_count_components(vcardcomponent *component,
                                    vcardcomponent_kind kind)
{
    int count = 0;
    icalpvl_elem itr;

    icalerror_check_arg_rz((component != 0), "component");

    for (itr = icalpvl_head(component->components); itr != 0; itr = icalpvl_next(itr)) {
        if (kind == VCARD_ANY_COMPONENT ||
            kind == vcardcomponent_isa((vcardcomponent *)icalpvl_data(itr))) {
            count++;
        }
    }

    return count;
}

vcardcomponent *vcardcomponent_get_current_component(vcardcomponent *component)
{
    icalerror_check_arg_rz((component != 0), "component");

    if (component->component_iterator == 0) {
        return 0;
    }

    return (vcardcomponent *)icalpvl_data(component->component_iterator);
}

vcardcomponent *vcardcomponent_get_first_component(vcardcomponent *c,
                                                   vcardcomponent_kind kind)
{
    icalerror_check_arg_rz((c != 0), "component");

    for (c->component_iterator = icalpvl_head(c->components);
         c->component_iterator != 0;
         c->component_iterator = icalpvl_next(c->component_iterator)) {
        vcardcomponent *p = (vcardcomponent *)icalpvl_data(c->component_iterator);

        if (vcardcomponent_isa(p) == kind || kind == VCARD_ANY_COMPONENT) {
            return p;
        }
    }

    return 0;
}

vcardcomponent *vcardcomponent_get_next_component(vcardcomponent *c,
                                                  vcardcomponent_kind kind)
{
    icalerror_check_arg_rz((c != 0), "component");

    if (c->component_iterator == 0) {
        return 0;
    }

    for (c->component_iterator = icalpvl_next(c->component_iterator);
         c->component_iterator != 0;
         c->component_iterator = icalpvl_next(c->component_iterator)) {
        vcardcomponent *p = (vcardcomponent *)icalpvl_data(c->component_iterator);

        if (vcardcomponent_isa(p) == kind || kind == VCARD_ANY_COMPONENT) {
            return p;
        }
    }

    return 0;
}

int vcardcomponent_check_restrictions(vcardcomponent *comp)
{
    icalerror_check_arg_rz(comp != 0, "comp");
    return vcardrestriction_check(comp);
}

int vcardcomponent_count_errors(vcardcomponent *comp)
{
    int errors = 0;
    icalpvl_elem itr;

    icalerror_check_arg_rz((comp != 0), "card");

    for (itr = icalpvl_head(comp->properties); itr != 0; itr = icalpvl_next(itr)) {
        vcardproperty *p = (vcardproperty *)icalpvl_data(itr);
        if (vcardproperty_isa(p) == VCARD_XLICERROR_PROPERTY) {
            errors++;
        }
    }

    return errors;
}

void vcardcomponent_strip_errors(vcardcomponent *comp)
{
    icalpvl_elem itr, next_itr;

    icalerror_check_arg_rv((comp != 0), "comp");

    for (itr = icalpvl_head(comp->properties); itr != 0; itr = next_itr) {
        vcardproperty *p = (vcardproperty *)icalpvl_data(itr);
        next_itr = icalpvl_next(itr);

        if (vcardproperty_isa(p) == VCARD_XLICERROR_PROPERTY) {
            vcardcomponent_remove_property(comp, p);
            vcardproperty_free(p);
        }
    }
}

struct vcardcomponent_kind_map {
    vcardcomponent_kind kind;
    char name[20];
};

static const struct vcardcomponent_kind_map component_map[] = {
    {VCARD_XROOT_COMPONENT, "XROOT"},
    {VCARD_VCARD_COMPONENT, "VCARD"},

    /* End of list */
    {VCARD_NO_COMPONENT, ""},
};

bool vcardcomponent_kind_is_valid(const vcardcomponent_kind kind)
{
    int i = 0;

    do {
        if (component_map[i].kind == kind) {
            return true;
        }
    } while (component_map[i++].kind != VCARD_NO_COMPONENT);

    return false;
}

const char *vcardcomponent_kind_to_string(vcardcomponent_kind kind)
{
    int i;

    for (i = 0; component_map[i].kind != VCARD_NO_COMPONENT; i++) {
        if (component_map[i].kind == kind) {
            return component_map[i].name;
        }
    }

    return 0;
}

vcardcomponent_kind vcardcomponent_string_to_kind(const char *string)
{
    int i;

    if (string == 0) {
        return VCARD_NO_COMPONENT;
    }

    for (i = 0; component_map[i].kind != VCARD_NO_COMPONENT; i++) {
        if (strncasecmp(string, component_map[i].name, strlen(component_map[i].name)) == 0) {
            return component_map[i].kind;
        }
    }

    return VCARD_NO_COMPONENT;
}

static int strcmpsafe(const char *a, const char *b)
{
    return strcmp((a == NULL ? "" : a),
                  (b == NULL ? "" : b));
}

static int prop_compare(void *a, void *b)
{
    vcardproperty *p1 = (vcardproperty *)a;
    vcardproperty *p2 = (vcardproperty *)b;
    vcardproperty_kind k1 = vcardproperty_isa(p1);
    vcardproperty_kind k2 = vcardproperty_isa(p2);
    int r = (int)(k1 - k2);

    if (r == 0) {
        if (k1 == VCARD_X_PROPERTY) {
            r = strcmp(vcardproperty_get_x_name(p1),
                       vcardproperty_get_x_name(p2));
        }

        if (r == 0) {
            r = strcmpsafe(vcardproperty_get_value_as_string(p1),
                           vcardproperty_get_value_as_string(p2));
        }
    }
    /* Always sort VERSION first */
    else if (k1 == VCARD_VERSION_PROPERTY) {
        r = -1;
    } else if (k2 == VCARD_VERSION_PROPERTY) {
        r = 1;
    }

    return r;
}

static int prop_kind_compare(vcardproperty_kind kind,
                             vcardcomponent *c1, vcardcomponent *c2)
{
    const vcardproperty *p1 = vcardcomponent_get_first_property(c1, kind);
    const vcardproperty *p2 = vcardcomponent_get_first_property(c2, kind);

    if (p1 && p2) {
        return strcmpsafe(vcardproperty_get_value_as_string(p1),
                          vcardproperty_get_value_as_string(p2));
    } else if (p1) {
        return -1;
    } else if (p2) {
        return 1;
    }

    return 0;
}

static int comp_compare(void *a, void *b)
{
    vcardcomponent *c1 = (vcardcomponent *)a;
    vcardcomponent *c2 = (vcardcomponent *)b;
    vcardcomponent_kind k1 = vcardcomponent_isa(c1);
    vcardcomponent_kind k2 = vcardcomponent_isa(c2);
    int r = (int)(k1 - k2);

    if (r == 0) {
        if (k1 == VCARD_VCARD_COMPONENT) {
            vcardproperty_kind prop_kinds[] = {
                VCARD_VERSION_PROPERTY,
                VCARD_N_PROPERTY,
                VCARD_FN_PROPERTY,
                VCARD_NICKNAME_PROPERTY,
                VCARD_ORG_PROPERTY,
                /* XXX  What else should we compare? */
                VCARD_NO_PROPERTY};

            for (int i = 0; r == 0 && prop_kinds[i] != VCARD_NO_PROPERTY; i++) {
                r = prop_kind_compare(prop_kinds[i], c1, c2);
            }
        } else {
            r = strcmp(c1->x_name, c2->x_name);
        }

        if (r == 0) {
            r = prop_kind_compare(VCARD_UID_PROPERTY, c1, c2);

            if (r == 0) {
                /* XXX  Anything else to compare? */
            }
        }
    }

    return r;
}

void vcardcomponent_normalize(vcardcomponent *comp)
{
    icalpvl_list sorted_props = icalpvl_newlist();
    icalpvl_list sorted_comps = icalpvl_newlist();
    vcardproperty *prop;
    vcardcomponent *sub;

    /* Normalize properties into sorted list */
    while ((prop = icalpvl_pop(comp->properties)) != 0) {
        int nparams, remove = 0;

        vcardproperty_normalize(prop);

        nparams = vcardproperty_count_parameters(prop);

        /* Remove unparameterized properties having default values */
        if (nparams == 0) {
            switch (vcardproperty_isa(prop)) {
            case VCARD_KIND_PROPERTY:
                if (vcardproperty_get_kind(prop) == VCARD_KIND_INDIVIDUAL) {
                    remove = 1;
                }
                break;

            default:
                break;
            }
        }

        if (remove) {
            vcardproperty_set_parent(prop, 0); // MUST NOT have a parent to free
            vcardproperty_free(prop);
        } else {
            icalpvl_insert_ordered(sorted_props, prop_compare, prop);
        }
    }

    icalpvl_free(comp->properties);
    comp->properties = sorted_props;

    /* Normalize sub-components into sorted list */
    while ((sub = icalpvl_pop(comp->components)) != 0) {
        vcardcomponent_normalize(sub);
        icalpvl_insert_ordered(sorted_comps, comp_compare, sub);
    }

    icalpvl_free(comp->components);
    comp->components = sorted_comps;
}

static void comp_to_v4(vcardcomponent *impl)
{
    icalpvl_elem itr, next;

    for (itr = icalpvl_head(impl->properties); itr != 0; itr = next) {
        vcardproperty *prop = (vcardproperty *)icalpvl_data(itr);
        vcardproperty_kind pkind = vcardproperty_isa(prop);
        vcardvalue *value = vcardproperty_get_value(prop);
        vcardvalue_kind vkind = vcardvalue_isa(value);
        vcardparameter *param;
        vcardenumarray *types = NULL;
        char *mediatype = NULL, *buf = NULL, *buf_ptr;
        const char *data;
        size_t size = 0;

        next = icalpvl_next(itr);

        /* Remove TYPE=PREF and replace with PREF=1 (if no existing (PREF=) */
        param = vcardproperty_get_first_parameter(prop, VCARD_TYPE_PARAMETER);
        if (param) {
            vcardenumarray_element pref = {VCARD_TYPE_PREF, NULL};
            size_t i;

            types = vcardparameter_get_type(param);
            i = vcardenumarray_find(types, &pref);
            if (i < vcardenumarray_size(types)) {
                vcardenumarray_remove_element_at(types, i);
                if (!vcardenumarray_size(types)) {
                    vcardproperty_remove_parameter_by_ref(prop, param);
                    types = NULL;
                }

                param = vcardproperty_get_first_parameter(prop,
                                                          VCARD_PREF_PARAMETER);
                if (!param) {
                    param = vcardparameter_new_pref(1);
                    vcardproperty_add_parameter(prop, param);
                }
            }
        }

        switch (pkind) {
        case VCARD_VERSION_PROPERTY:
            vcardproperty_set_version(prop, VCARD_VERSION_40);
            break;

        case VCARD_BDAY_PROPERTY:
        case VCARD_DEATHDATE_PROPERTY:
        case VCARD_ANNIVERSARY_PROPERTY:
            for (param = vcardproperty_get_first_parameter(prop,
                                                           VCARD_X_PARAMETER);
                 param;
                 param = vcardproperty_get_next_parameter(prop,
                                                          VCARD_X_PARAMETER)) {
                const char *name = vcardparameter_get_xname(param);

                /* This appears in the wild for v3 date with missing year */
                if (name && !strcasecmp(name, "X-APPLE-OMIT-YEAR")) {
                    vcardtimetype dt = vcardproperty_get_bday(prop);

                    dt.year = -1;
                    vcardproperty_set_bday(prop, dt);
                    vcardproperty_remove_parameter_by_ref(prop, param);
                    break;
                }
            }
            break;

        case VCARD_GEO_PROPERTY:
            if (vkind != VCARD_X_VALUE) {
                vcardgeotype geo = vcardvalue_get_geo(value);

                if (!geo.uri) {
                    /* Convert STRUCTURED value kind to geo: URI */
                    int n = snprintf(buf, size, "geo:%s,%s",
                                     geo.coords.lat, geo.coords.lon);

                    size = (size_t)n + 1;
                    buf = icalmemory_new_buffer(size);
                    snprintf(buf, size, "geo:%s,%s",
                             geo.coords.lat, geo.coords.lon);

                    geo.uri = buf;
                    geo.coords.lat = geo.coords.lon = NULL;
                    vcardvalue_set_geo(value, geo);
                }
            }
            break;

        case VCARD_KEY_PROPERTY:
        case VCARD_LOGO_PROPERTY:
        case VCARD_PHOTO_PROPERTY:
        case VCARD_SOUND_PROPERTY: {
            if (types) {
                /* Replace TYPE=subtype with MEDIATYPE or data:<mediatype>
                 *
                 * XXX  We assume that the first VCARD_TYPE_X is the subtype
                 */
                for (size_t i = 0; i < vcardenumarray_size(types); i++) {
                    const vcardenumarray_element *type =
                        vcardenumarray_element_at(types, i);

                    if (type->xvalue) {
                        /* Copy and lowercase the mediatype */
                        char *c;

                        switch (pkind) {
                        case VCARD_LOGO_PROPERTY:
                        case VCARD_PHOTO_PROPERTY:
                            mediatype = icalmemory_strdup("image/");
                            break;
                        case VCARD_SOUND_PROPERTY:
                            mediatype = icalmemory_strdup("audio/");
                            break;
                        default:
                            mediatype = icalmemory_strdup("application/");
                            break;
                        }

                        size = strlen(mediatype);
                        buf_ptr = mediatype + size;
                        icalmemory_append_string(&mediatype, &buf_ptr,
                                                 &size, type->xvalue);
                        for (c = mediatype; *c; c++) {
                            *c = (char)tolower((int)*c);
                        }

                        /* Remove this TYPE */
                        vcardenumarray_remove_element_at(types, i);
                        if (!vcardenumarray_size(types)) {
                            vcardproperty_remove_parameter_by_ref(prop, param);
                        }
                        break;
                    }
                }
            }

            data = vcardvalue_get_uri(value);
            if (data && !strchr(data, ':')) {
                /* Convert base64-encoded TEXT value kind to data: URI */
                size += strlen(data) + 7; // "data:," + NUL

                param = vcardproperty_get_first_parameter(prop,
                                                          VCARD_ENCODING_PARAMETER);
                if (param) {
                    size += 7; // ";base64
                }

                buf = icalmemory_new_buffer(size);
                buf_ptr = buf;

                icalmemory_append_string(&buf, &buf_ptr, &size, "data:");
                if (mediatype) {
                    icalmemory_append_string(&buf, &buf_ptr, &size, mediatype);
                    icalmemory_free_buffer(mediatype);
                }
                if (param) {
                    icalmemory_append_string(&buf, &buf_ptr, &size, ";base64");
                    vcardproperty_remove_parameter_by_ref(prop, param);
                }
                icalmemory_append_char(&buf, &buf_ptr, &size, ',');
                icalmemory_append_string(&buf, &buf_ptr, &size, data);

                value->kind = VCARD_URI_VALUE;
                vcardvalue_set_uri(value, buf);
            } else if (mediatype) {
                param = vcardparameter_new_mediatype(mediatype);
                vcardproperty_add_parameter(prop, param);

                icalmemory_free_buffer(mediatype);
            }
            break;
        }

        case VCARD_UID_PROPERTY:
            if (vkind == VCARD_TEXT_VALUE) {
                unsigned t_low, t_mid, t_high, clock, node;

                /* Does it look like a RFC 4122 UUID? */
                data = vcardvalue_get_text(value);
                if (data && strlen(data) == 36 &&
                    sscanf(data, "%8X-%4X-%4X-%4X-%12X",
                           &t_low, &t_mid, &t_high, &clock, &node) == 5) {
                    /* Convert TEXT value kind to urn:uuid: URI */
                    size = strlen(data) + 10; // "urn:uuid:" + NUL
                    buf = icalmemory_new_buffer(size);
                    buf_ptr = buf;

                    icalmemory_append_string(&buf, &buf_ptr, &size, "urn:uuid:");
                    icalmemory_append_string(&buf, &buf_ptr, &size, data);

                    value->kind = VCARD_URI_VALUE;
                    vcardvalue_set_uri(value, buf);
                }
            }
            break;

        case VCARD_X_PROPERTY: {
            /* Rename X-ADDRESSBOOKSERVER-KIND, X-ADDRESSBOOKSERVER-MEMBER */
            const char *xprop = vcardproperty_as_vcard_string(prop);
            if (!strncasecmp(xprop, "X-ADDRESSBOOKSERVER-", 20) &&
                (!strncasecmp(xprop + 20, "KIND:", 5) ||
                 !strncasecmp(xprop + 20, "MEMBER:", 7))) {
                vcardproperty *new = vcardproperty_new_from_string(xprop + 20);
                vcardcomponent_add_property(impl, new);
                vcardcomponent_remove_property(impl, prop);
            }
            break;
        }

        default:
            break;
        }

        if (buf) {
            icalmemory_free_buffer(buf);
        }
    }
}

struct pref_prop {
    vcardproperty *prop;
    int level;
};

static void comp_to_v3(vcardcomponent *impl)
{
    struct pref_prop *pref_props[VCARD_NO_PROPERTY] = {0};
    vcardenumarray_element type;
    vcardproperty_kind pkind;
    icalpvl_elem itr, next;

    for (itr = icalpvl_head(impl->properties); itr != 0; itr = next) {
        vcardproperty *prop = (vcardproperty *)icalpvl_data(itr);
        vcardproperty *xprop;
        vcardparameter *val_param =
            vcardproperty_get_first_parameter(prop, VCARD_VALUE_PARAMETER);
        vcardvalue *value = vcardproperty_get_value(prop);
        vcardvalue_kind vkind = vcardvalue_isa(value);
        vcardparameter *param;
        char *subtype = NULL, *buf = NULL;
        const char *mediatype, *uri, *xname = NULL;

        next = icalpvl_next(itr);

        /* Find prop with lowest PREF= for each set of like properties */
        pkind = vcardproperty_isa(prop);
        param = vcardproperty_get_first_parameter(prop, VCARD_PREF_PARAMETER);
        if (param && pkind != VCARD_X_PROPERTY) {
            int level = vcardparameter_get_pref(param);
            struct pref_prop *pp = pref_props[pkind];

            if (!pp) {
                pp = icalmemory_new_buffer(sizeof(struct pref_prop));
                pp->prop = prop;
                pp->level = level;
                pref_props[pkind] = pp;
            } else if (level < pp->level) {
                pp->prop = prop;
                pp->level = level;
            }
        }

        /* Replace MEDIATYPE with TYPE=<subtype> */
        param = vcardproperty_get_first_parameter(prop,
                                                  VCARD_MEDIATYPE_PARAMETER);
        if (param) {
            mediatype = vcardparameter_get_mediatype(param);
            subtype = strchr(mediatype, '/');
            if (subtype) {
                /* Copy and uppercase the subtype */
                char *c;

                subtype = icalmemory_strdup(subtype + 1);
                for (c = subtype; *c; c++) {
                    *c = (char)toupper((int)*c);
                }

                /* Add TYPE parameter */
                type.val = VCARD_TYPE_NONE;
                type.xvalue = subtype;
                vcardproperty_add_type_parameter(prop, &type);

                icalmemory_free_buffer(subtype);
            }

            vcardproperty_remove_parameter_by_ref(prop, param);
        }

        switch (pkind) {
        case VCARD_VERSION_PROPERTY:
            vcardproperty_set_version(prop, VCARD_VERSION_30);
            break;

        case VCARD_BDAY_PROPERTY:
        case VCARD_DEATHDATE_PROPERTY:
        case VCARD_ANNIVERSARY_PROPERTY: {
            vcardtimetype dt = vcardproperty_get_bday(prop);

            if (dt.year == -1) {
                /* This appears in the wild for v3 date with missing year */
                dt.year = 1604;
                vcardproperty_set_parameter_from_string(prop,
                                                        "X-APPLE-OMIT-YEAR",
                                                        "1604");
            }
            if (dt.hour != -1) {
                if (dt.second == -1) {
                    dt.second = 0;
                    if (dt.minute == -1) {
                        dt.minute = 0;
                    }
                }
            }

            vcardproperty_set_bday(prop, dt);
            break;
        }

        case VCARD_GEO_PROPERTY:
            if (vkind != VCARD_X_VALUE) {
                vcardgeotype geo = vcardvalue_get_geo(value);

                if (geo.uri && !strncmp(geo.uri, "geo:", 4)) {
                    /* Convert geo: URI to STRUCTURED value kind */
                    char *lon;

                    buf = icalmemory_strdup(geo.uri);
                    geo.uri = NULL;
                    geo.coords.lat = buf + 4;
                    lon = strchr(buf + 4, ',');
                    if (lon) {
                        *lon++ = '\0';
                        geo.coords.lon = lon;
                    } else {
                        geo.coords.lon = "";
                    }

                    vcardvalue_set_geo(value, geo);
                }
            }
            break;

        case VCARD_KEY_PROPERTY:
        case VCARD_LOGO_PROPERTY:
        case VCARD_PHOTO_PROPERTY:
        case VCARD_SOUND_PROPERTY:
            uri = vcardvalue_get_uri(value);
            if (uri && !strncmp("data:", uri, 5)) {
                /* Convert data: URI to base64-encoded TEXT value kind */
                char *base64, *data = NULL;

                buf = icalmemory_strdup(uri);
                mediatype = buf + 5;
                base64 = strstr(mediatype, ";base64,");

                if (base64) {
                    param = vcardparameter_new_encoding(VCARD_ENCODING_B);
                    vcardproperty_add_parameter(prop, param);

                    *base64 = '\0';
                    data = base64 + 8;
                } else {
                    data = strchr(mediatype, ',');
                    if (data) {
                        *data++ = '\0';
                    }
                }

                subtype = strchr(mediatype, '/');
                if (subtype) {
                    /* Copy and uppercase the subtype */
                    char *c;

                    for (c = ++subtype; *c; c++) {
                        *c = (char)toupper((int)*c);
                    }

                    /* Add TYPE parameter */
                    type.val = VCARD_VERSION_NONE;
                    type.xvalue = subtype;
                    vcardproperty_add_type_parameter(prop, &type);
                }

                value->kind = VCARD_TEXT_VALUE;
                vcardvalue_set_text(value, data ? data : "");
            }
            break;

        case VCARD_KIND_PROPERTY:
            /* Rename KIND, MEMBER */
            xname = "X-ADDRESSBOOKSERVER-KIND";

            _fallthrough();

        case VCARD_MEMBER_PROPERTY:
            if (!xname) {
                xname = "X-ADDRESSBOOKSERVER-MEMBER";
            }
            xprop = vcardproperty_new_x(vcardvalue_as_vcard_string(value));
            vcardproperty_set_x_name(xprop, xname);
            vcardcomponent_add_property(impl, xprop);
            vcardcomponent_remove_property(impl, prop);
            break;

        case VCARD_TEL_PROPERTY:
            uri = vcardvalue_get_uri(value);
            if (uri && !strncmp(uri, "tel:", 4)) {
                /* Convert tel: URI to TEXT value kind */
                buf = icalmemory_strdup(uri + 4);

                value->kind = VCARD_TEXT_VALUE;
                vcardvalue_set_text(value, buf);

                if (val_param) {
                    vcardproperty_remove_parameter_by_ref(prop, val_param);
                }
            }
            break;

        case VCARD_UID_PROPERTY:
            uri = vcardvalue_get_uri(value);
            if (uri && !strncmp(uri, "urn:uuid:", 9)) {
                /* Convert urn:uuid: URI to TEXT value kind */
                buf = icalmemory_strdup(uri + 9);

                value->kind = VCARD_TEXT_VALUE;
                vcardvalue_set_text(value, buf);

                if (val_param) {
                    vcardproperty_remove_parameter_by_ref(prop, val_param);
                }
            }
            break;

        default:
            break;
        }

        if (buf) {
            icalmemory_free_buffer(buf);
        }
    }

    /* Add TYPE=PREF for each most preferred property */
    for (pkind = 0; pkind < VCARD_NO_PROPERTY; ++pkind) {
        struct pref_prop *pp = pref_props[pkind];

        if (pp) {
            type.val = VCARD_TYPE_PREF;
            type.xvalue = NULL;

            vcardproperty_add_type_parameter(pp->prop, &type);
            icalmemory_free_buffer(pp);
        }
    }
}

void vcardcomponent_transform(vcardcomponent *impl,
                              vcardproperty_version version)
{
    icalpvl_elem itr;
    vcardcomponent_kind kind = vcardcomponent_isa(impl);

    icalerror_check_arg_rv((impl != 0), "component");
    icalerror_check_arg_rv((kind != VCARD_NO_COMPONENT),
                           "component kind is VCARD_NO_COMPONENT");

    if (kind == VCARD_VCARD_COMPONENT) {
        if (version == VCARD_VERSION_40) {
            comp_to_v4(impl);
        } else {
            comp_to_v3(impl);
        }
    }

    for (itr = icalpvl_head(impl->components); itr != 0; itr = icalpvl_next(itr)) {
        vcardcomponent *c = (vcardcomponent *)icalpvl_data(itr);
        vcardcomponent_transform(c, version);
    }
}

/******************** Convenience routines **********************/

enum vcardproperty_version vcardcomponent_get_version(vcardcomponent *comp)
{
    icalerror_check_arg_rx(comp != 0, "comp", VCARD_VERSION_NONE);

    if (comp->versionp == 0) {
        comp->versionp =
            vcardcomponent_get_first_property(comp, VCARD_VERSION_PROPERTY);

        if (comp->versionp == 0) {
            return VCARD_VERSION_NONE;
        }
    }

    return vcardproperty_get_version(comp->versionp);
}

const char *vcardcomponent_get_uid(vcardcomponent *comp)
{
    vcardproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    prop = vcardcomponent_get_first_property(comp, VCARD_UID_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return vcardproperty_get_uid(prop);
}

const char *vcardcomponent_get_fn(vcardcomponent *comp)
{
    vcardproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    prop = vcardcomponent_get_first_property(comp, VCARD_FN_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return vcardproperty_get_fn(prop);
}
