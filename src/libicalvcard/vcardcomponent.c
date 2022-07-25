/*======================================================================
 FILE: vcardcomponent.c

======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalerror.h"
#include "icalmemory.h"
#include "vcardcomponent.h"
#include "vcardparser.h"
#include "vcardproperty_p.h"
//#include "vcardrestriction.h"

#include <assert.h>
#include <stdlib.h>
#include <limits.h>

struct vcardcomponent_impl
{
    char id[5];
    vcardcomponent_kind kind;
    vcardproperty_version version;
    char *x_name;
    pvl_list properties;
    pvl_elem property_iterator;
    pvl_list components;
    pvl_elem component_iterator;
    struct vcardcomponent_impl *parent;
};

static void vcardcomponent_add_children(vcardcomponent *impl, va_list args)
{
    void *vp;

    while ((vp = va_arg(args, void *)) != 0)
    {
        icalassert(vcardproperty_isa_property(vp) != 0);

        vcardcomponent_add_property(impl, (vcardproperty *) vp);
    }
}

static vcardcomponent *vcardcomponent_new_impl(vcardcomponent_kind kind)
{
    vcardcomponent *comp;

    if (kind != VCARD_VCARD_COMPONENT) {
        return 0;
    }

    if ((comp = (vcardcomponent *) icalmemory_new_buffer(sizeof(vcardcomponent))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(comp, 0, sizeof(vcardcomponent));

    strcpy(comp->id, "comp");

    comp->kind = kind;
    comp->properties = pvl_newlist();
    comp->components = pvl_newlist();

    return comp;
}

vcardcomponent *vcardcomponent_new(vcardcomponent_kind kind)
{
    return vcardcomponent_new_impl(kind);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvarargs"
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
#pragma clang diagnostic pop

vcardcomponent *vcardcomponent_new_from_string(const char *str)
{
//    return vcardparser_parse_string(str);
return (void*) str;
}

vcardcomponent *vcardcomponent_clone(const vcardcomponent *old)
{
    vcardcomponent *new;
    vcardproperty *p;
    pvl_elem itr;

    icalerror_check_arg_rz((old != 0), "component");

    new = vcardcomponent_new_impl(old->kind);

    if (new == 0) {
        return 0;
    }

    for (itr = pvl_head(old->properties); itr != 0; itr = pvl_next(itr)) {
        p = (vcardproperty *) pvl_data(itr);
        vcardcomponent_add_property(new, vcardproperty_clone(p));
    }

    return new;
}

void vcardcomponent_free(vcardcomponent *c)
{
    vcardproperty *prop;
    vcardcomponent *comp;

    icalerror_check_arg_rv((c != 0), "component");

    if (c != 0) {
        if (c->parent != 0) {
            return;
        }

        if (c->properties != 0) {
            while ((prop = pvl_pop(c->properties)) != 0) {
                vcardproperty_set_parent(prop, 0);
                vcardproperty_free(prop);
            }
            pvl_free(c->properties);
        }

        while ((comp = pvl_data(pvl_head(c->components))) != 0) {
            vcardcomponent_remove_component(c, comp);
            vcardcomponent_free(comp);
        }

        pvl_free(c->components);

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
}

char *vcardcomponent_as_vcard_string(vcardcomponent *impl)
{
    char *buf;

    buf = vcardcomponent_as_vcard_string_r(impl);
    if (buf) {
        icalmemory_add_tmp_buffer(buf);
    }
    return buf;
}

char *vcardcomponent_as_vcard_string_r(vcardcomponent *impl)
{
    char *buf;
    char *tmp_buf;
    size_t buf_size = 1024;
    char *buf_ptr = 0;
    pvl_elem itr;

    /* RFC6350 explicitly says that the newline is *ALWAYS* a \r\n (CRLF)!!!! */
    const char newline[] = "\r\n";

    vcardproperty *p;

    icalerror_check_arg_rz((impl != 0), "component");

    buf = icalmemory_new_buffer(buf_size);
    if (buf == NULL)
        return NULL;

    buf_ptr = buf;

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "BEGIN:VCARD");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);

    for (itr = pvl_head(impl->properties); itr != 0; itr = pvl_next(itr)) {
        p = (vcardproperty *) pvl_data(itr);

        icalerror_assert((p != 0), "Got a null property");
        tmp_buf = vcardproperty_as_vcard_string_r(p);

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, tmp_buf);
        icalmemory_free_buffer(tmp_buf);
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "END:VCARD");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);

    return buf;
}

int vcardcomponent_is_valid(vcardcomponent *component)
{
    if ((strcmp(component->id, "comp") == 0) && component->kind != VCARD_NO_COMPONENT) {
        return 1;
    } else {
        return 0;
    }
}

vcardcomponent_kind vcardcomponent_isa(const vcardcomponent *component)
{
    icalerror_check_arg_rx((component != 0), "component", VCARD_NO_COMPONENT);

    return component->kind;
}

int vcardcomponent_isa_component(void *component)
{
    vcardcomponent *impl = component;

    icalerror_check_arg_rz((component != 0), "component");

    if (strcmp(impl->id, "comp") == 0) {
        return 1;
    } else {
        return 0;
    }
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

    pvl_push(comp->properties, property);
}

void vcardcomponent_remove_property(vcardcomponent *comp, vcardproperty *property)
{
    pvl_elem itr, next_itr;

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

    for (itr = pvl_head(comp->properties); itr != 0; itr = next_itr) {
        next_itr = pvl_next(itr);

        if (pvl_data(itr) == (void *)property) {

            if (comp->property_iterator == itr) {
                comp->property_iterator = pvl_next(itr);
            }

            (void)pvl_remove(comp->properties, itr);
            vcardproperty_set_parent(property, 0);
        }
    }
}

int vcardcomponent_count_properties(vcardcomponent *comp, vcardproperty_kind kind)
{
    int count = 0;
    pvl_elem itr;

    icalerror_check_arg_rz((comp != 0), "component");

    for (itr = pvl_head(comp->properties); itr != 0; itr = pvl_next(itr)) {
        if (kind == vcardproperty_isa((vcardproperty *) pvl_data(itr)) || kind == VCARD_ANY_PROPERTY) {
            count++;
        }
    }

    return count;
}

vcardproperty *vcardcomponent_get_current_property(vcardcomponent *comp)
{
    icalerror_check_arg_rz((comp != 0), "card");

    if (comp->property_iterator == 0) {
        return 0;
    }

    return (vcardproperty *) pvl_data(comp->property_iterator);
}

vcardproperty *vcardcomponent_get_first_property(vcardcomponent *c, vcardproperty_kind kind)
{
    icalerror_check_arg_rz((c != 0), "card");

    for (c->property_iterator = pvl_head(c->properties);
         c->property_iterator != 0;
         c->property_iterator = pvl_next(c->property_iterator)) {

        vcardproperty *p = (vcardproperty *) pvl_data(c->property_iterator);

        if (vcardproperty_isa(p) == kind || kind == VCARD_ANY_PROPERTY) {

            return p;
        }
    }
    return 0;
}

vcardproperty *vcardcomponent_get_next_property(vcardcomponent *c, vcardproperty_kind kind)
{
    icalerror_check_arg_rz((c != 0), "card");

    if (c->property_iterator == 0) {
        return 0;
    }

    for (c->property_iterator = pvl_next(c->property_iterator);
         c->property_iterator != 0;
         c->property_iterator = pvl_next(c->property_iterator)) {

        vcardproperty *p = (vcardproperty *) pvl_data(c->property_iterator);

        if (vcardproperty_isa(p) == kind || kind == VCARD_ANY_PROPERTY) {

            return p;
        }
    }

    return 0;
}

vcardproperty **vcardcomponent_get_properties(vcardcomponent *comp, vcardproperty_kind kind);

void vcardcomponent_add_component(vcardcomponent *parent, vcardcomponent *child)
{
    icalerror_check_arg_rv((parent != 0), "parent");
    icalerror_check_arg_rv((child != 0), "child");

    if (child->parent != 0) {
        icalerror_set_errno(ICAL_USAGE_ERROR);
    }

    child->parent = parent;

    pvl_push(parent->components, child);
}

void vcardcomponent_remove_component(vcardcomponent *parent,
                                     vcardcomponent *child)
{
    pvl_elem itr, next_itr;

    icalerror_check_arg_rv((parent != 0), "parent");
    icalerror_check_arg_rv((child != 0), "child");

    for (itr = pvl_head(parent->components); itr != 0; itr = next_itr) {
        next_itr = pvl_next(itr);

        if (pvl_data(itr) == (void *)child) {

            if (parent->component_iterator == itr) {
                /* Don't let the current iterator become invalid */

                /* HACK. The semantics for this are troubling. */
                parent->component_iterator = pvl_next(parent->component_iterator);
            }
            (void)pvl_remove(parent->components, itr);
            child->parent = 0;
            break;
        }
    }
}

int vcardcomponent_count_components(vcardcomponent *component,
                                    vcardcomponent_kind kind)
{
    int count = 0;
    pvl_elem itr;

    icalerror_check_arg_rz((component != 0), "component");

    for (itr = pvl_head(component->components); itr != 0; itr = pvl_next(itr)) {
        if (kind == vcardcomponent_isa((vcardcomponent *) pvl_data(itr)) ||
            kind == VCARD_ANY_COMPONENT) {
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

    return (vcardcomponent *) pvl_data(component->component_iterator);
}

vcardcomponent *vcardcomponent_get_first_component(vcardcomponent *c,
                                                   vcardcomponent_kind kind)
{
    icalerror_check_arg_rz((c != 0), "component");

    for (c->component_iterator = pvl_head(c->components);
         c->component_iterator != 0;
         c->component_iterator = pvl_next(c->component_iterator)) {

        vcardcomponent *p = (vcardcomponent *) pvl_data(c->component_iterator);

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

    for (c->component_iterator = pvl_next(c->component_iterator);
         c->component_iterator != 0;
         c->component_iterator = pvl_next(c->component_iterator)) {

        vcardcomponent *p = (vcardcomponent *) pvl_data(c->component_iterator);

        if (vcardcomponent_isa(p) == kind || kind == VCARD_ANY_COMPONENT) {

            return p;
        }
    }

    return 0;
}

int vcardcomponent_check_restrictions(vcardcomponent *comp)
{
    icalerror_check_arg_rz(comp != 0, "comp");
//    return vcardrestriction_check(comp);
    return 0;
}

int vcardcomponent_count_errors(vcardcomponent *comp)
{
    int errors = 0;
    vcardproperty *p;
    pvl_elem itr;

    icalerror_check_arg_rz((comp != 0), "card");

    for (itr = pvl_head(comp->properties); itr != 0; itr = pvl_next(itr)) {
        p = (vcardproperty *) pvl_data(itr);

        if (vcardproperty_isa(p) == VCARD_XLICERROR_PROPERTY) {
            errors++;
        }
    }

    return errors;
}

void vcardcomponent_strip_errors(vcardcomponent *comp)
{
    vcardproperty *p;
    pvl_elem itr, next_itr;

    icalerror_check_arg_rv((comp != 0), "comp");

    for (itr = pvl_head(comp->properties); itr != 0; itr = next_itr) {
        p = (vcardproperty *) pvl_data(itr);
        next_itr = pvl_next(itr);

        if (vcardproperty_isa(p) == VCARD_XLICERROR_PROPERTY) {
            vcardcomponent_remove_property(comp, p);
            vcardproperty_free(p);
            p = NULL;
        }
    }
}
#if 0
/* Hack. This will change the state of the iterators */
void vcardcomponent_convert_errors(vcardcomponent *comp)
{
    vcardproperty *p, *next_p;
    vcardcomponent *c;

    for (p = vcardcomponent_get_first_property(card, VCARD_ANY_PROPERTY); p != 0; p = next_p) {

        next_p = vcardcomponent_get_next_property(card, VCARD_ANY_PROPERTY);

        if (vcardproperty_isa(p) == VCARD_XLICERROR_PROPERTY) {
            struct icalreqstattype rst;
            icalparameter *param =
                vcardproperty_get_first_parameter(p, VCARD_XLICERRORTYPE_PARAMETER);

            rst.code = ICAL_UNKNOWN_STATUS;
            rst.desc = 0;

            switch (vcardparameter_get_xlicerrortype(param)) {

            case ICAL_XLICERRORTYPE_PARAMETERNAMEPARSEERROR:{
                    rst.code = ICAL_3_2_INVPARAM_STATUS;
                    break;
                }
            case ICAL_XLICERRORTYPE_PARAMETERVALUEPARSEERROR:{
                    rst.code = ICAL_3_3_INVPARAMVAL_STATUS;
                    break;
                }
            case ICAL_XLICERRORTYPE_PROPERTYPARSEERROR:{
                    rst.code = ICAL_3_0_INVPROPNAME_STATUS;
                    break;
                }
            case ICAL_XLICERRORTYPE_VALUEPARSEERROR:{
                    rst.code = ICAL_3_1_INVPROPVAL_STATUS;
                    break;
                }
            case ICAL_XLICERRORTYPE_CARDPARSEERROR:{
                    rst.code = ICAL_3_4_INVCOMP_STATUS;
                    break;
                }

            default:{
                    break;
                }
            }
            if (rst.code != ICAL_UNKNOWN_STATUS) {

                rst.debug = vcardproperty_get_xlicerror(p);
                vcardcomponent_add_property(card, vcardproperty_new_requeststatus(rst));

                vcardcomponent_remove_property(card, p);
                vcardproperty_free(p);
                p = NULL;
            }
        }
    }

    for (c = vcardcomponent_get_first_card(card, ICAL_ANY_CARD);
         c != 0; c = vcardcomponent_get_next_card(card, ICAL_ANY_CARD)) {

        vcardcomponent_convert_errors(c);
    }
}

static int strcmpsafe(const char *a, const char *b)
{
    return strcmp((a == NULL ? "" : a),
                  (b == NULL ? "" : b));
}

static int prop_compare(void *a, void *b)
{
    vcardproperty *p1 = (vcardproperty*) a;
    vcardproperty *p2 = (vcardproperty*) b;
    vcardproperty_kind k1 = vcardproperty_isa(p1);
    vcardproperty_kind k2 = vcardproperty_isa(p2);
    int r = k1 - k2;

    if (r == 0) {
        if (k1 == ICAL_X_PROPERTY) {
            r = strcmp(vcardproperty_get_x_name(p1),
                       vcardproperty_get_x_name(p2));
        }

        if (r == 0) {
            r = strcmpsafe(vcardproperty_get_value_as_string(p1),
                           vcardproperty_get_value_as_string(p2));
        }
    }

    return r;
}

static int comp_compare(void *a, void *b)
{
    vcardcomponent *c1 = (vcard*) a;
    vcardcomponent *c2 = (vcard*) b;
    vcardcomponent_kind k1 = vcardcomponent_isa(c1);
    vcardcomponent_kind k2 = vcardcomponent_isa(c2);
    int r = k1 - k2;

    if (r == 0) {
        if (k1 == ICAL_X_CARD) {
            r = strcmp(c1->x_name, c2->x_name);
        }

        if (r == 0) {
            const char *u1 = vcardcomponent_get_uid(c1);
            const char *u2 = vcardcomponent_get_uid(c2);

            if (u1 && u2) {
                r = strcmp(u1, u2);

                if (r == 0) {
                    r = icaltime_compare(vcardcomponent_get_recurrenceid(c1),
                                         vcardcomponent_get_recurrenceid(c2));
                }
            } else {
                vcardproperty *p1, *p2;

                switch (k1) {
                case ICAL_VALARM_CARD:
                    p1 = vcardcomponent_get_first_property(c1,
                                                          ICAL_TRIGGER_PROPERTY);
                    p2 = vcardcomponent_get_first_property(c2,
                                                          ICAL_TRIGGER_PROPERTY);
                    r = strcmp(vcardproperty_get_value_as_string(p1),
                               vcardproperty_get_value_as_string(p2));

                    if (r == 0) {
                        p1 = vcardcomponent_get_first_property(c1,
                                                              ICAL_ACTION_PROPERTY);
                        p2 = vcardcomponent_get_first_property(c2,
                                                              ICAL_ACTION_PROPERTY);
                        r = strcmp(vcardproperty_get_value_as_string(p1),
                                   vcardproperty_get_value_as_string(p2));
                    }
                    break;

                case ICAL_VTIMEZONE_CARD:
                    p1 = vcardcomponent_get_first_property(c1,
                                                          ICAL_TZID_PROPERTY);
                    p2 = vcardcomponent_get_first_property(c2,
                                                          ICAL_TZID_PROPERTY);
                    r = strcmp(vcardproperty_get_value_as_string(p1),
                               vcardproperty_get_value_as_string(p2));
                    break;

                case ICAL_XSTANDARD_CARD:
                case ICAL_XDAYLIGHT_CARD:
                    p1 = vcardcomponent_get_first_property(c1,
                                                          ICAL_DTSTART_PROPERTY);
                    p2 = vcardcomponent_get_first_property(c2,
                                                          ICAL_DTSTART_PROPERTY);
                    r = strcmp(vcardproperty_get_value_as_string(p1),
                               vcardproperty_get_value_as_string(p2));
                    break;

                case ICAL_VVOTER_CARD:
                    p1 = vcardcomponent_get_first_property(c1,
                                                          ICAL_VOTER_PROPERTY);
                    p2 = vcardcomponent_get_first_property(c2,
                                                          ICAL_VOTER_PROPERTY);
                    r = strcmp(vcardproperty_get_value_as_string(p1),
                               vcardproperty_get_value_as_string(p2));
                    break;

                case ICAL_XVOTE_CARD:
                    p1 = vcardcomponent_get_first_property(c1,
                                                          ICAL_POLLITEMID_PROPERTY);
                    p2 = vcardcomponent_get_first_property(c2,
                                                          ICAL_POLLITEMID_PROPERTY);
                    r = strcmp(vcardproperty_get_value_as_string(p1),
                               vcardproperty_get_value_as_string(p2));
                    break;

                default:
                    /* XXX  Anything better? */
                    r = icaltime_compare(vcardcomponent_get_dtstamp(c1),
                                         vcardcomponent_get_dtstamp(c2));
                    break;
                }
            }
        }
    /* Always sort VTIMEZONEs first */
    } else if (k1 == ICAL_VTIMEZONE_CARD) {
        return -1;
    } else if (k2 == ICAL_VTIMEZONE_CARD) {
        return 1;
    }

    return r;
}

void vcardcomponent_normalize(vcardcomponent *comp)
{
    pvl_list sorted_props = pvl_newlist();
    pvl_list sorted_comps = pvl_newlist();
    vcardproperty *prop;
    vcardcomponent *sub;

    /* Normalize properties into sorted list */
    while ((prop = pvl_pop(comp->properties)) != 0) {
        int nparams, remove = 0;

        vcardproperty_normalize(prop);

        nparams = vcardproperty_count_parameters(prop);

        /* Remove unparameterized properties having default values */
        if (nparams == 0) {
            switch (vcardproperty_isa(prop)) {
            case ICAL_CALSCALE_PROPERTY:
                if (strcmp("GREGORIAN", vcardproperty_get_calscale(prop)) == 0) {
                    remove = 1;
                }
                break;

            case ICAL_CLASS_PROPERTY:
                if (vcardproperty_get_class(prop) == ICAL_CLASS_PUBLIC) {
                    remove = 1;
                }
                break;

            case ICAL_PRIORITY_PROPERTY:
                if (vcardproperty_get_priority(prop) == 0) {
                    remove = 1;
                }
                break;

            case ICAL_TRANSP_PROPERTY:
                if (vcardproperty_get_transp(prop) == ICAL_TRANSP_OPAQUE) {
                    remove = 1;
                }
                break;

            case ICAL_REPEAT_PROPERTY:
                if (vcardproperty_get_repeat(prop) == 0) {
                    remove = 1;
                }
                break;

            case ICAL_SEQUENCE_PROPERTY:
                if (vcardproperty_get_sequence(prop) == 0) {
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
            pvl_insert_ordered(sorted_props, prop_compare, prop);
        }
    }

    pvl_free(comp->properties);
    comp->properties = sorted_props;

    /* Normalize sub-cards into sorted list */
    while ((sub = pvl_pop(comp->cards)) != 0) {
        vcardcomponent_normalize(sub);
        pvl_insert_ordered(sorted_comps, comp_compare, sub);
    }

    pvl_free(comp->cards);
    comp->cards = sorted_comps;
}
#endif
