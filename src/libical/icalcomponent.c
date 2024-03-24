/*======================================================================
 FILE: icalcomponent.c
 CREATOR: eric 28 April 1999

 SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>

 SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icalcomponent.h"
#include "icalerror.h"
#include "icalmemory.h"
#include "icalparser.h"
#include "icalproperty_p.h"
#include "icalrestriction.h"
#include "icaltimezone.h"

#include <assert.h>
#include <stdlib.h>
#include <limits.h>

struct icalcomponent_impl
{
    char id[5];
    icalcomponent_kind kind;
    char *x_name;
    pvl_list properties;
    pvl_elem property_iterator;
    pvl_list components;
    pvl_elem component_iterator;
    struct icalcomponent_impl *parent;

        /** An array of icaltimezone structs. We use this so we can do fast
           lookup of timezones using binary searches. timezones_sorted is
           set to 0 whenever we add a timezone, so we remember to sort the
           array before doing a binary search. */
    icalarray *timezones;
    int timezones_sorted;
};

static void icalcomponent_add_children(icalcomponent *impl, va_list args);
static icalcomponent *icalcomponent_new_impl(icalcomponent_kind kind);

static void icalcomponent_merge_vtimezone(icalcomponent *comp,
                                          icalcomponent *vtimezone, icalarray *tzids_to_rename);
static void icalcomponent_handle_conflicting_vtimezones(icalcomponent *comp,
                                                        icalcomponent *vtimezone,
                                                        icalproperty *tzid_prop,
                                                        const char *tzid,
                                                        icalarray *tzids_to_rename);
static size_t icalcomponent_get_tzid_prefix_len(const char *tzid);
static void icalcomponent_rename_tzids(icalcomponent *comp, icalarray *rename_table);
static void icalcomponent_rename_tzids_callback(icalparameter *param, void *data);
static int icalcomponent_compare_vtimezones(icalcomponent *vtimezone1, icalcomponent *vtimezone2);
static int icalcomponent_compare_timezone_fn(const void *elem1, const void *elem2);

void icalcomponent_add_children(icalcomponent *impl, va_list args)
{
    void *vp;

    while ((vp = va_arg(args, void *)) != 0)
    {
        icalassert(icalcomponent_isa_component(vp) != 0 || icalproperty_isa_property(vp) != 0);

        if (icalcomponent_isa_component(vp) != 0) {
            icalcomponent_add_component(impl, (icalcomponent *) vp);

        } else if (icalproperty_isa_property(vp) != 0) {
            icalcomponent_add_property(impl, (icalproperty *) vp);
        }
    }
}

static icalcomponent *icalcomponent_new_impl(icalcomponent_kind kind)
{
    icalcomponent *comp;

    if (!icalcomponent_kind_is_valid(kind))
        return NULL;

    if ((comp = (icalcomponent *) icalmemory_new_buffer(sizeof(icalcomponent))) == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    memset(comp, 0, sizeof(icalcomponent));

    strcpy(comp->id, "comp");

    comp->kind = kind;
    comp->properties = pvl_newlist();
    comp->components = pvl_newlist();
    comp->timezones_sorted = 1;

    return comp;
}

icalcomponent *icalcomponent_new(icalcomponent_kind kind)
{
    return icalcomponent_new_impl(kind);
}

icalcomponent *icalcomponent_vanew(icalcomponent_kind kind, ...)
{
    /* See https://github.com/libical/libical/issues/585. Caller must pass NULL as final argument */

    va_list args;

    icalcomponent *impl = icalcomponent_new_impl(kind);

    if (impl == 0) {
        return 0;
    }

    va_start(args, kind);
    icalcomponent_add_children(impl, args);
    va_end(args);

    return impl;
}

icalcomponent *icalcomponent_new_from_string(const char *str)
{
    return icalparser_parse_string(str);
}

icalcomponent *icalcomponent_clone(const icalcomponent *old)
{
    icalcomponent *new;
    icalproperty *p;
    icalcomponent *c;
    pvl_elem itr;

    icalerror_check_arg_rz((old != 0), "component");

    new = icalcomponent_new_impl(old->kind);

    if (new == 0) {
        return 0;
    }

    for (itr = pvl_head(old->properties); itr != 0; itr = pvl_next(itr)) {
        p = (icalproperty *) pvl_data(itr);
        icalcomponent_add_property(new, icalproperty_clone(p));
    }

    for (itr = pvl_head(old->components); itr != 0; itr = pvl_next(itr)) {
        c = (icalcomponent *) pvl_data(itr);
        icalcomponent_add_component(new, icalcomponent_clone(c));
    }

    return new;
}

icalcomponent *icalcomponent_new_clone(icalcomponent *old)
{
    return icalcomponent_clone(old);
}

icalcomponent *icalcomponent_new_x(const char *x_name)
{
    icalcomponent *comp = icalcomponent_new_impl(ICAL_X_COMPONENT);

    if (!comp) {
        return 0;
    }
    comp->x_name = icalmemory_strdup(x_name);
    return comp;
}

void icalcomponent_free(icalcomponent *c)
{
    icalproperty *prop;
    icalcomponent *comp;

    icalerror_check_arg_rv((c != 0), "component");

    if (c != 0) {
        if (c->parent != 0) {
            return;
        }

        if (c->properties != 0) {
            while ((prop = pvl_pop(c->properties)) != 0) {
                icalproperty_set_parent(prop, 0);
                icalproperty_free(prop);
            }
            pvl_free(c->properties);
        }

        while ((comp = pvl_data(pvl_head(c->components))) != 0) {
            icalcomponent_remove_component(c, comp);
            icalcomponent_free(comp);
        }

        pvl_free(c->components);

        if (c->x_name != 0) {
            icalmemory_free_buffer(c->x_name);
        }

        if (c->timezones) {
            icaltimezone_array_free(c->timezones);
            c->timezones = 0;
        }

        c->kind = ICAL_NO_COMPONENT;
        c->properties = 0;
        c->property_iterator = 0;
        c->components = 0;
        c->component_iterator = 0;
        c->x_name = 0;
        c->id[0] = 'X';
        c->timezones = NULL;

        icalmemory_free_buffer(c);
    }
}

char *icalcomponent_as_ical_string(icalcomponent *impl)
{
    char *buf;

    buf = icalcomponent_as_ical_string_r(impl);
    if (buf) {
        icalmemory_add_tmp_buffer(buf);
    }
    return buf;
}

char *icalcomponent_as_ical_string_r(icalcomponent *impl)
{
    char *buf;
    char *tmp_buf;
    size_t buf_size = 1024;
    char *buf_ptr = 0;
    pvl_elem itr;

    /* RFC5545 explicitly says that the newline is *ALWAYS* a \r\n (CRLF)!!!! */
    const char newline[] = "\r\n";

    icalcomponent *c;
    icalproperty *p;
    icalcomponent_kind kind = icalcomponent_isa(impl);

    const char *kind_string;

    icalerror_check_arg_rz((impl != 0), "component");
    icalerror_check_arg_rz((kind != ICAL_NO_COMPONENT), "component kind is ICAL_NO_COMPONENT");

    if (kind != ICAL_X_COMPONENT) {
        kind_string = icalcomponent_kind_to_string(kind);
    } else {
        kind_string = impl->x_name;
    }

    icalerror_check_arg_rz((kind_string != 0), "Unknown kind of component");

    buf = icalmemory_new_buffer(buf_size);
    if (buf == NULL)
        return NULL;

    buf_ptr = buf;

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "BEGIN:");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, kind_string);
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);

    for (itr = pvl_head(impl->properties); itr != 0; itr = pvl_next(itr)) {
        p = (icalproperty *) pvl_data(itr);

        icalerror_assert((p != 0), "Got a null property");
        tmp_buf = icalproperty_as_ical_string_r(p);

        icalmemory_append_string(&buf, &buf_ptr, &buf_size, tmp_buf);
        icalmemory_free_buffer(tmp_buf);
    }

    for (itr = pvl_head(impl->components); itr != 0; itr = pvl_next(itr)) {
        c = (icalcomponent *) pvl_data(itr);

        tmp_buf = icalcomponent_as_ical_string_r(c);
        if (tmp_buf != NULL) {
            icalmemory_append_string(&buf, &buf_ptr, &buf_size, tmp_buf);
            icalmemory_free_buffer(tmp_buf);
        }
    }

    icalmemory_append_string(&buf, &buf_ptr, &buf_size, "END:");
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, icalcomponent_kind_to_string(kind));
    icalmemory_append_string(&buf, &buf_ptr, &buf_size, newline);

    return buf;
}

int icalcomponent_is_valid(icalcomponent *component)
{
    if ((strcmp(component->id, "comp") == 0) && component->kind != ICAL_NO_COMPONENT) {
        return 1;
    } else {
        return 0;
    }
}

icalcomponent_kind icalcomponent_isa(const icalcomponent *component)
{
    icalerror_check_arg_rx((component != 0), "component", ICAL_NO_COMPONENT);

    return component->kind;
}

int icalcomponent_isa_component(void *component)
{
    icalcomponent *impl = component;

    icalerror_check_arg_rz((component != 0), "component");

    if (strcmp(impl->id, "comp") == 0) {
        return 1;
    } else {
        return 0;
    }
}

void icalcomponent_set_x_name(icalcomponent *comp, const char *name)
{
    icalerror_check_arg_rv((name != 0), "name");
    icalerror_check_arg_rv((comp != 0), "comp");

    if (comp->x_name != 0) {
        free(comp->x_name);
    }

    comp->x_name = icalmemory_strdup(name);

    if (comp->x_name == 0) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
    }
}

const char *icalcomponent_get_x_name(icalcomponent *comp)
{
    icalerror_check_arg_rz((comp != 0), "comp");

    return comp->x_name;
}

const char *icalcomponent_get_component_name(const icalcomponent *comp)
{
    char *buf;

    buf = icalcomponent_get_component_name_r(comp);
    icalmemory_add_tmp_buffer(buf);
    return buf;
}

char *icalcomponent_get_component_name_r(const icalcomponent *comp)
{
    const char *component_name = 0;
    size_t buf_size = 256;
    char *buf;
    char *buf_ptr;

    icalerror_check_arg_rz((comp != 0), "comp");

    buf = icalmemory_new_buffer(buf_size);
    buf_ptr = buf;

    if (comp->kind == ICAL_X_COMPONENT && comp->x_name != 0) {
        component_name = comp->x_name;
    } else {
        component_name = icalcomponent_kind_to_string(comp->kind);
    }

    if (component_name == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        icalmemory_free_buffer(buf);
        return 0;

    } else {
        /* _append_string will automatically grow the buffer if
           component_name is longer than the initial buffer size */
        icalmemory_append_string(&buf, &buf_ptr, &buf_size, component_name);
    }

    return buf;
}

void icalcomponent_add_property(icalcomponent *component, icalproperty *property)
{
    icalerror_check_arg_rv((component != 0), "component");
    icalerror_check_arg_rv((property != 0), "property");

    icalerror_assert((!icalproperty_get_parent(property)),
                     "The property has already been added to a component. "
                     "Remove the property with icalcomponent_remove_property "
                     "before calling icalcomponent_add_property");

    icalproperty_set_parent(property, component);

    pvl_push(component->properties, property);
}

void icalcomponent_remove_property(icalcomponent *component, icalproperty *property)
{
    pvl_elem itr, next_itr;

    icalerror_check_arg_rv((component != 0), "component");
    icalerror_check_arg_rv((property != 0), "property");

#if defined(ICAL_REMOVE_NONMEMBER_COMPONENT_IS_ERROR)
    icalerror_assert((icalproperty_get_parent(property)),
                     "The property is not a member of a component");
#else
    if (icalproperty_get_parent(property) == 0) {
        return;
    }
#endif

    for (itr = pvl_head(component->properties); itr != 0; itr = next_itr) {
        next_itr = pvl_next(itr);

        if (pvl_data(itr) == (void *)property) {

            if (component->property_iterator == itr) {
                component->property_iterator = pvl_next(itr);
            }

            (void)pvl_remove(component->properties, itr);
            icalproperty_set_parent(property, 0);
        }
    }
}

int icalcomponent_count_properties(icalcomponent *component, icalproperty_kind kind)
{
    int count = 0;
    pvl_elem itr;

    icalerror_check_arg_rz((component != 0), "component");

    for (itr = pvl_head(component->properties); itr != 0; itr = pvl_next(itr)) {
        if (kind == icalproperty_isa((icalproperty *) pvl_data(itr)) || kind == ICAL_ANY_PROPERTY) {
            count++;
        }
    }

    return count;
}

icalproperty *icalcomponent_get_current_property(icalcomponent *component)
{
    icalerror_check_arg_rz((component != 0), "component");

    if (component->property_iterator == 0) {
        return 0;
    }

    return (icalproperty *) pvl_data(component->property_iterator);
}

icalproperty *icalcomponent_get_first_property(icalcomponent *c, icalproperty_kind kind)
{
    icalerror_check_arg_rz((c != 0), "component");

    for (c->property_iterator = pvl_head(c->properties);
         c->property_iterator != 0; c->property_iterator = pvl_next(c->property_iterator)) {

        icalproperty *p = (icalproperty *) pvl_data(c->property_iterator);

        if (icalproperty_isa(p) == kind || kind == ICAL_ANY_PROPERTY) {

            return p;
        }
    }
    return 0;
}

icalproperty *icalcomponent_get_next_property(icalcomponent *c, icalproperty_kind kind)
{
    icalerror_check_arg_rz((c != 0), "component");

    if (c->property_iterator == 0) {
        return 0;
    }

    for (c->property_iterator = pvl_next(c->property_iterator);
         c->property_iterator != 0; c->property_iterator = pvl_next(c->property_iterator)) {

        icalproperty *p = (icalproperty *) pvl_data(c->property_iterator);

        if (icalproperty_isa(p) == kind || kind == ICAL_ANY_PROPERTY) {

            return p;
        }
    }

    return 0;
}

icalproperty **icalcomponent_get_properties(icalcomponent *component, icalproperty_kind kind);

void icalcomponent_add_component(icalcomponent *parent, icalcomponent *child)
{
    icalerror_check_arg_rv((parent != 0), "parent");
    icalerror_check_arg_rv((child != 0), "child");

    if (child->parent != 0) {
        icalerror_set_errno(ICAL_USAGE_ERROR);
    }

    child->parent = parent;

    /* Fix for Mozilla - bug 327602 */
    if (child->kind != ICAL_VTIMEZONE_COMPONENT) {
        pvl_push(parent->components, child);
    } else {
        /* VTIMEZONES should be first in the resulting VCALENDAR. */
        pvl_unshift(parent->components, child);

        /* Add the VTIMEZONE to our array. */
        /* FIXME: Currently we are also creating this array when loading in
           a builtin VTIMEZONE, when we don't need it. */
        if (!parent->timezones)
            parent->timezones = icaltimezone_array_new();

        if (parent->timezones)
            icaltimezone_array_append_from_vtimezone(parent->timezones, child);

        /* Flag that we need to sort it before doing any binary searches. */
        parent->timezones_sorted = 0;
    }
}

void icalcomponent_remove_component(icalcomponent *parent, icalcomponent *child)
{
    pvl_elem itr, next_itr;

    icalerror_check_arg_rv((parent != 0), "parent");
    icalerror_check_arg_rv((child != 0), "child");

    /* If the component is a VTIMEZONE, remove it from our array as well. */
    if (child->kind == ICAL_VTIMEZONE_COMPONENT) {
        icaltimezone *zone;
        size_t i, num_elements;

        num_elements = parent->timezones ? parent->timezones->num_elements : 0;
        for (i = 0; i < num_elements; i++) {
            zone = icalarray_element_at(parent->timezones, i);
            if (icaltimezone_get_component(zone) == child) {
                icaltimezone_free(zone, 0);
                icalarray_remove_element_at(parent->timezones, i);
                break;
            }
        }
    }

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

int icalcomponent_count_components(icalcomponent *component, icalcomponent_kind kind)
{
    int count = 0;
    pvl_elem itr;

    icalerror_check_arg_rz((component != 0), "component");

    for (itr = pvl_head(component->components); itr != 0; itr = pvl_next(itr)) {
        if (kind == icalcomponent_isa((icalcomponent *) pvl_data(itr)) ||
            kind == ICAL_ANY_COMPONENT) {
            count++;
        }
    }

    return count;
}

icalcomponent *icalcomponent_get_current_component(icalcomponent *component)
{
    icalerror_check_arg_rz((component != 0), "component");

    if (component->component_iterator == 0) {
        return 0;
    }

    return (icalcomponent *) pvl_data(component->component_iterator);
}

icalcomponent *icalcomponent_get_first_component(icalcomponent *c, icalcomponent_kind kind)
{
    icalerror_check_arg_rz((c != 0), "component");

    for (c->component_iterator = pvl_head(c->components);
         c->component_iterator != 0; c->component_iterator = pvl_next(c->component_iterator)) {

        icalcomponent *p = (icalcomponent *) pvl_data(c->component_iterator);

        if (icalcomponent_isa(p) == kind || kind == ICAL_ANY_COMPONENT) {

            return p;
        }
    }

    return 0;
}

icalcomponent *icalcomponent_get_next_component(icalcomponent *c, icalcomponent_kind kind)
{
    icalerror_check_arg_rz((c != 0), "component");

    if (c->component_iterator == 0) {
        return 0;
    }

    for (c->component_iterator = pvl_next(c->component_iterator);
         c->component_iterator != 0; c->component_iterator = pvl_next(c->component_iterator)) {

        icalcomponent *p = (icalcomponent *) pvl_data(c->component_iterator);

        if (icalcomponent_isa(p) == kind || kind == ICAL_ANY_COMPONENT) {

            return p;
        }
    }

    return 0;
}

icalcomponent *icalcomponent_get_first_real_component(icalcomponent *c)
{
    icalcomponent *comp;

    for (comp = icalcomponent_get_first_component(c, ICAL_ANY_COMPONENT);
         comp != 0; comp = icalcomponent_get_next_component(c, ICAL_ANY_COMPONENT)) {

        icalcomponent_kind kind = icalcomponent_isa(comp);

        if (kind == ICAL_VEVENT_COMPONENT ||
            kind == ICAL_VTODO_COMPONENT ||
            kind == ICAL_VJOURNAL_COMPONENT ||
            kind == ICAL_VFREEBUSY_COMPONENT ||
            kind == ICAL_VAVAILABILITY_COMPONENT ||
            kind == ICAL_VPOLL_COMPONENT ||
            kind == ICAL_VPATCH_COMPONENT ||
            kind == ICAL_VQUERY_COMPONENT || kind == ICAL_VAGENDA_COMPONENT) {
            return comp;
        }
    }
    return 0;
}

icaltime_span icalcomponent_get_span(icalcomponent *comp)
{
    icalcomponent *inner;
    icalcomponent_kind kind;
    icaltime_span span;
    struct icaltimetype start, end;

    span.start = 0;
    span.end = 0;
    span.is_busy = 1;

    /* initial Error checking */
    if (comp == NULL) {
        return span;
    }

    /* FIXME this might go away */
    kind = icalcomponent_isa(comp);
    if (kind == ICAL_VCALENDAR_COMPONENT) {
        inner = icalcomponent_get_first_real_component(comp);

        /* Maybe there is a VTIMEZONE in there */
        if (inner == 0) {
            inner = icalcomponent_get_first_component(comp, ICAL_VTIMEZONE_COMPONENT);
        }

    } else {
        inner = comp;
    }

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        /*icalerror_warn("icalcomponent_get_span: no component specified, \
or empty VCALENDAR component"); */
        return span;
    }

    kind = icalcomponent_isa(inner);

    if (!(kind == ICAL_VEVENT_COMPONENT ||
          kind == ICAL_VJOURNAL_COMPONENT ||
          kind == ICAL_VTODO_COMPONENT || kind == ICAL_VFREEBUSY_COMPONENT)) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        /*icalerror_warn("icalcomponent_get_span: no component specified, \
or empty VCALENDAR component"); */
        return span;
    }

    /* Get to work. starting with DTSTART */
    start = icalcomponent_get_dtstart(comp);
    if (icaltime_is_null_time(start)) {
        return span;
    }
    span.start = icaltime_as_timet_with_zone(start, icaltimezone_get_utc_timezone());

    /* The end time could be specified as either a DTEND, a DURATION, or be missing */
    /* icalcomponent_get_dtend takes care of these cases. */
    end = icalcomponent_get_dtend(comp);

    span.end = icaltime_as_timet_with_zone(end, icaltimezone_get_utc_timezone());
    if (icaltime_is_date(start)) {
        /* Until the end of the day */
        span.end -= 1;
    }

    return span;
}

int icalproperty_recurrence_is_excluded(icalcomponent *comp,
                                        struct icaltimetype *dtstart,
                                        struct icaltimetype *recurtime)
{
    icalproperty *exdate, *exrule;
    pvl_elem property_iterator;

    if (comp == NULL || dtstart == NULL || recurtime == NULL || icaltime_is_null_time(*recurtime)) {
        /* BAD DATA */
        return 1;
    }

    property_iterator = comp->property_iterator;

  /** first test against the exdate values **/
    for (exdate = icalcomponent_get_first_property(comp, ICAL_EXDATE_PROPERTY);
         exdate != NULL; exdate = icalcomponent_get_next_property(comp, ICAL_EXDATE_PROPERTY)) {

        struct icaltimetype exdatetime = icalproperty_get_datetime_with_component(exdate, comp);

        if ((icaltime_is_date(exdatetime) &&
             icaltime_compare_date_only(*recurtime, exdatetime) == 0) ||
            (icaltime_compare(*recurtime, exdatetime) == 0)) {
            /** MATCHED **/
            comp->property_iterator = property_iterator;
            return 1;
        }
    }

  /** Now test against the EXRULEs **/
    for (exrule = icalcomponent_get_first_property(comp, ICAL_EXRULE_PROPERTY);
         exrule != NULL; exrule = icalcomponent_get_next_property(comp, ICAL_EXRULE_PROPERTY)) {

        struct icalrecurrencetype recur = icalproperty_get_exrule(exrule);
        icalrecur_iterator *exrule_itr = icalrecur_iterator_new(recur, *dtstart);
        struct icaltimetype exrule_time;

        while (exrule_itr) {
            int result;

            exrule_time = icalrecur_iterator_next(exrule_itr);

            if (icaltime_is_null_time(exrule_time))
                break;

            result = icaltime_compare(*recurtime, exrule_time);
            if (result == 0) {
                icalrecur_iterator_free(exrule_itr);
                comp->property_iterator = property_iterator;
                return 1;
                  /** MATCH **/
            }
            if (result == 1)
                break;
                  /** exrule_time > recurtime **/
        }

        if (exrule_itr)
            icalrecur_iterator_free(exrule_itr);
    }
    comp->property_iterator = property_iterator;

    return 0; /* no matches */
}

/**
 * @brief Returns the busy status based on the TRANSP property.
 *
 * @param comp   A valid icalcomponent.
 *
 * @return 1 if the event is a busy item, 0 if it is not.
 */
static int icalcomponent_is_busy(icalcomponent *comp)
{
    icalproperty *transp;
    enum icalproperty_status status;
    int ret = 1;

  /** @todo check access control here, converting busy->free if the
     permissions do not allow access... */

    /* Is this a busy time?  Check the TRANSP property */
    transp = icalcomponent_get_first_property(comp, ICAL_TRANSP_PROPERTY);

    if (transp) {
        icalvalue *transp_val = icalproperty_get_value(transp);

        switch (icalvalue_get_transp(transp_val)) {
        case ICAL_TRANSP_OPAQUE:
        case ICAL_TRANSP_OPAQUENOCONFLICT:
        case ICAL_TRANSP_NONE:
            ret = 1;
            break;
        case ICAL_TRANSP_TRANSPARENT:
        case ICAL_TRANSP_TRANSPARENTNOCONFLICT:
            ret = 0;
            break;
        default:
            ret = 0;
            break;
        }
    }
    status = icalcomponent_get_status(comp);
    if (ret && status) {
        switch (status) {
        case ICAL_STATUS_CANCELLED:
        case ICAL_STATUS_TENTATIVE:
            ret = 0;
            break;
        default:
            break;
        }
    }
    return (ret);
}

void icalcomponent_foreach_recurrence(icalcomponent *comp,
                                      struct icaltimetype start,
                                      struct icaltimetype end,
                                      void (*callback) (icalcomponent *comp,
                                                        struct icaltime_span *span,
                                                        void *data), void *callback_data)
{
    struct icaltimetype dtstart, dtend;
    icaltime_span recurspan, basespan, limit_span;
    icaltime_t limit_start, limit_end;
    icaltime_t dtduration;
    icalproperty *rrule, *rdate;
    pvl_elem property_iterator; /* for saving the iterator */

    if (comp == NULL || callback == NULL)
        return;

    dtstart = icalcomponent_get_dtstart(comp);

    if (icaltime_is_null_time(dtstart) &&
        icalcomponent_isa(comp) == ICAL_VTODO_COMPONENT) {
        /* VTODO with no DTSTART - use DUE */
        dtstart = icalcomponent_get_due(comp);
    }
    if (icaltime_is_null_time(dtstart))
        return;

    /* The end time could be specified as either a DTEND, a DURATION or be missing */
    /* icalcomponent_get_dtend takes care of these cases. */
    dtend = icalcomponent_get_dtend(comp);

    /* Now set up the base span for this item, corresponding to the
       base DTSTART and DTEND */
    basespan = icaltime_span_new(dtstart, dtend, 1);

    basespan.is_busy = icalcomponent_is_busy(comp);

    /* Calculate the ceiling and floor values.. */
    limit_start = icaltime_as_timet_with_zone(start,
                                              icaltimezone_get_utc_timezone());
    if (!icaltime_is_null_time(end)) {
        limit_end = icaltime_as_timet_with_zone(end,
                                                icaltimezone_get_utc_timezone());
    } else {
#if (SIZEOF_ICALTIME_T > 4)
        limit_end = (icaltime_t) LONG_MAX;
#else
        limit_end = (icaltime_t) INT_MAX;
#endif
    }
    limit_span.start = limit_start;
    limit_span.end = limit_end;

    /* Do the callback for the DTSTART entry, ONLY if there is no RRULE.
       Otherwise, the initial occurrence will be handled by the RRULE. */
    rrule = icalcomponent_get_first_property(comp, ICAL_RRULE_PROPERTY);
    if ((rrule == NULL) &&
        !icalproperty_recurrence_is_excluded(comp, &dtstart, &dtstart)) {
        /* call callback action */
        if (icaltime_span_overlaps(&basespan, &limit_span))
            (*callback) (comp, &basespan, callback_data);
    }

    recurspan = basespan;
    dtduration = basespan.end - basespan.start;

    /* Now cycle through the rrule entries */
    for (; rrule != NULL;
         rrule = icalcomponent_get_next_property(comp, ICAL_RRULE_PROPERTY)) {

        struct icalrecurrencetype recur = icalproperty_get_rrule(rrule);
        icalrecur_iterator *rrule_itr = icalrecur_iterator_new(recur, dtstart);
        struct icaltimetype rrule_time;

        if (!rrule_itr) continue;

        if (recur.count == 0) {
            icaltimetype mystart = start;

            /* make sure we include any recurrence that ends in timespan */
            icaltime_adjust(&mystart, 0, 0, 0, -(int)(long)dtduration);
            icalrecur_iterator_set_start(rrule_itr, mystart);
        }

        for (rrule_time = icalrecur_iterator_next(rrule_itr);
             !icaltime_is_null_time(rrule_time);
             rrule_time = icalrecur_iterator_next(rrule_itr)) {

            /* if we have iterated past end time,
               then no need to check any further */
            if (icaltime_compare(rrule_time, end) > 0)
                break;

            recurspan.start =
                icaltime_as_timet_with_zone(rrule_time,
                                            rrule_time.zone ? rrule_time.zone :
                                            icaltimezone_get_utc_timezone());
            recurspan.end = recurspan.start + dtduration;

            /* save the iterator ICK! */
            property_iterator = comp->property_iterator;

            if (!icalproperty_recurrence_is_excluded(comp,
                                                     &dtstart, &rrule_time)) {
                /* call callback action */
                if (icaltime_span_overlaps(&recurspan, &limit_span))
                    (*callback) (comp, &recurspan, callback_data);
            }
            comp->property_iterator = property_iterator;
        }   /* end of iteration over a specific RRULE */

        icalrecur_iterator_free(rrule_itr);
    }   /* end of RRULE loop */

    /* Now process RDATE entries */
    for (rdate = icalcomponent_get_first_property(comp, ICAL_RDATE_PROPERTY);
         rdate != NULL;
         rdate = icalcomponent_get_next_property(comp, ICAL_RDATE_PROPERTY)) {

        struct icaldatetimeperiodtype rdate_period =
            icalproperty_get_rdate(rdate);

        /* RDATES can specify raw datetimes, periods, or dates.
            we only support raw datetimes for now..

            @todo Add support for other types */

        if (icaltime_is_null_time(rdate_period.time))
            continue;

        recurspan.start =
            icaltime_as_timet_with_zone(rdate_period.time,
                                        rdate_period.time.zone ?
                                        rdate_period.time.zone :
                                        icaltimezone_get_utc_timezone());
        recurspan.end = recurspan.start + dtduration;

        /* save the iterator ICK! */
        property_iterator = comp->property_iterator;

        if (!icalproperty_recurrence_is_excluded(comp,
                                                 &dtstart, &rdate_period.time)) {
            /* call callback action */
            if (icaltime_span_overlaps(&recurspan, &limit_span))
                (*callback) (comp, &recurspan, callback_data);
        }
        comp->property_iterator = property_iterator;
    }
}

int icalcomponent_check_restrictions(icalcomponent *comp)
{
    icalerror_check_arg_rz(comp != 0, "comp");
    return icalrestriction_check(comp);
}

int icalcomponent_count_errors(icalcomponent *component)
{
    int errors = 0;
    icalproperty *p;
    icalcomponent *c;
    pvl_elem itr;

    icalerror_check_arg_rz((component != 0), "component");

    for (itr = pvl_head(component->properties); itr != 0; itr = pvl_next(itr)) {
        p = (icalproperty *) pvl_data(itr);

        if (icalproperty_isa(p) == ICAL_XLICERROR_PROPERTY) {
            errors++;
        }
    }

    for (itr = pvl_head(component->components); itr != 0; itr = pvl_next(itr)) {
        c = (icalcomponent *) pvl_data(itr);

        errors += icalcomponent_count_errors(c);
    }

    return errors;
}

void icalcomponent_strip_errors(icalcomponent *component)
{
    icalproperty *p;
    icalcomponent *c;
    pvl_elem itr, next_itr;

    icalerror_check_arg_rv((component != 0), "component");

    for (itr = pvl_head(component->properties); itr != 0; itr = next_itr) {
        p = (icalproperty *) pvl_data(itr);
        next_itr = pvl_next(itr);

        if (icalproperty_isa(p) == ICAL_XLICERROR_PROPERTY) {
            icalcomponent_remove_property(component, p);
            icalproperty_free(p);
            p = NULL;
        }
    }

    for (itr = pvl_head(component->components); itr != 0; itr = pvl_next(itr)) {
        c = (icalcomponent *) pvl_data(itr);
        icalcomponent_strip_errors(c);
    }
}

/* Hack. This will change the state of the iterators */
void icalcomponent_convert_errors(icalcomponent *component)
{
    icalproperty *p, *next_p;
    icalcomponent *c;

    for (p = icalcomponent_get_first_property(component, ICAL_ANY_PROPERTY); p != 0; p = next_p) {

        next_p = icalcomponent_get_next_property(component, ICAL_ANY_PROPERTY);

        if (icalproperty_isa(p) == ICAL_XLICERROR_PROPERTY) {
            struct icalreqstattype rst;
            icalparameter *param =
                icalproperty_get_first_parameter(p, ICAL_XLICERRORTYPE_PARAMETER);

            rst.code = ICAL_UNKNOWN_STATUS;
            rst.desc = 0;

            switch (icalparameter_get_xlicerrortype(param)) {

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
            case ICAL_XLICERRORTYPE_COMPONENTPARSEERROR:{
                    rst.code = ICAL_3_4_INVCOMP_STATUS;
                    break;
                }

            default:{
                    break;
                }
            }
            if (rst.code != ICAL_UNKNOWN_STATUS) {

                rst.debug = icalproperty_get_xlicerror(p);
                icalcomponent_add_property(component, icalproperty_new_requeststatus(rst));

                icalcomponent_remove_property(component, p);
                icalproperty_free(p);
                p = NULL;
            }
        }
    }

    for (c = icalcomponent_get_first_component(component, ICAL_ANY_COMPONENT);
         c != 0; c = icalcomponent_get_next_component(component, ICAL_ANY_COMPONENT)) {

        icalcomponent_convert_errors(c);
    }
}

icalcomponent *icalcomponent_get_parent(icalcomponent *component)
{
    return component->parent;
}

void icalcomponent_set_parent(icalcomponent *component, icalcomponent *parent)
{
    component->parent = parent;
}

static const icalcompiter icalcompiter_null = { ICAL_NO_COMPONENT, 0 };

struct icalcomponent_kind_map
{
    icalcomponent_kind kind;
    char name[20];
};

static const struct icalcomponent_kind_map component_map[] = {
    {ICAL_VEVENT_COMPONENT, "VEVENT"},
    {ICAL_VTODO_COMPONENT, "VTODO"},
    {ICAL_VJOURNAL_COMPONENT, "VJOURNAL"},
    {ICAL_VCALENDAR_COMPONENT, "VCALENDAR"},
    {ICAL_VAGENDA_COMPONENT, "VAGENDA"},
    {ICAL_VFREEBUSY_COMPONENT, "VFREEBUSY"},
    {ICAL_VTIMEZONE_COMPONENT, "VTIMEZONE"},
    {ICAL_VALARM_COMPONENT, "VALARM"},
    {ICAL_XSTANDARD_COMPONENT, "STANDARD"}, /*These are part of RFC5545 */
    {ICAL_XDAYLIGHT_COMPONENT, "DAYLIGHT"}, /*but are not really components */
    {ICAL_X_COMPONENT, "X"},
    {ICAL_VSCHEDULE_COMPONENT, "SCHEDULE"},

    /* CAP components */
    {ICAL_VCAR_COMPONENT, "VCAR"},
    {ICAL_VCOMMAND_COMPONENT, "VCOMMAND"},
    {ICAL_VQUERY_COMPONENT, "VQUERY"},
    {ICAL_VREPLY_COMPONENT, "VREPLY"},

    /* libical private components */
    {ICAL_XLICINVALID_COMPONENT, "X-LIC-UNKNOWN"},
    {ICAL_XLICMIMEPART_COMPONENT, "X-LIC-MIME-PART"},
    {ICAL_ANY_COMPONENT, "ANY"},
    {ICAL_XROOT_COMPONENT, "XROOT"},

    /* Calendar Availability components */
    {ICAL_VAVAILABILITY_COMPONENT, "VAVAILABILITY"},
    {ICAL_XAVAILABLE_COMPONENT, "AVAILABLE"},

    /* Consensus Scheduling components */
    {ICAL_VPOLL_COMPONENT, "VPOLL"},
    {ICAL_VVOTER_COMPONENT, "VVOTER"},
    {ICAL_XVOTE_COMPONENT, "VOTE"},

    /* VPATCH components */
    {ICAL_VPATCH_COMPONENT, "VPATCH"},
    {ICAL_XPATCH_COMPONENT, "PATCH"},

    /* Event Publishing components */
    {ICAL_PARTICIPANT_COMPONENT, "PARTICIPANT"},
    {ICAL_VLOCATION_COMPONENT, "VLOCATION"},
    {ICAL_VRESOURCE_COMPONENT, "VRESOURCE"},

    /* End of list */
    {ICAL_NO_COMPONENT, ""},
};

int icalcomponent_kind_is_valid(const icalcomponent_kind kind)
{
    int i = 0;

    do {
        if (component_map[i].kind == kind) {
            return 1;
        }
    } while (component_map[i++].kind != ICAL_NO_COMPONENT);

    return 0;
}

const char *icalcomponent_kind_to_string(icalcomponent_kind kind)
{
    int i;

    for (i = 0; component_map[i].kind != ICAL_NO_COMPONENT; i++) {
        if (component_map[i].kind == kind) {
            return component_map[i].name;
        }
    }

    return 0;
}

icalcomponent_kind icalcomponent_string_to_kind(const char *string)
{
    int i;

    if (string == 0) {
        return ICAL_NO_COMPONENT;
    }

    for (i = 0; component_map[i].kind != ICAL_NO_COMPONENT; i++) {
        if (strncasecmp(string, component_map[i].name, strlen(component_map[i].name)) == 0) {
            return component_map[i].kind;
        }
    }

    return ICAL_NO_COMPONENT;
}

icalcompiter icalcomponent_begin_component(icalcomponent *component, icalcomponent_kind kind)
{
    icalcompiter itr;
    pvl_elem i;

    itr.kind = kind;
    itr.iter = NULL;

    icalerror_check_arg_re(component != 0, "component", icalcompiter_null);

    for (i = pvl_head(component->components); i != 0; i = pvl_next(i)) {

        icalcomponent *c = (icalcomponent *) pvl_data(i);

        if (icalcomponent_isa(c) == kind || kind == ICAL_ANY_COMPONENT) {

            itr.iter = i;

            return itr;
        }
    }

    return icalcompiter_null;
}

icalcompiter icalcomponent_end_component(icalcomponent *component, icalcomponent_kind kind)
{
    icalcompiter itr;
    pvl_elem i;

    itr.kind = kind;

    icalerror_check_arg_re(component != 0, "component", icalcompiter_null);

    for (i = pvl_tail(component->components); i != 0; i = pvl_prior(i)) {

        icalcomponent *c = (icalcomponent *) pvl_data(i);

        if (icalcomponent_isa(c) == kind || kind == ICAL_ANY_COMPONENT) {

            itr.iter = pvl_next(i);

            return itr;
        }
    }

    return icalcompiter_null;
}

icalcomponent *icalcompiter_next(icalcompiter *i)
{
    if (i->iter == 0) {
        return 0;
    }

    icalerror_check_arg_rz((i != 0), "i");

    for (i->iter = pvl_next(i->iter); i->iter != 0; i->iter = pvl_next(i->iter)) {

        icalcomponent *c = (icalcomponent *) pvl_data(i->iter);

        if (icalcomponent_isa(c) == i->kind || i->kind == ICAL_ANY_COMPONENT) {

            return icalcompiter_deref(i);
        }
    }

    return 0;
}

icalcomponent *icalcompiter_prior(icalcompiter *i)
{
    if (i->iter == 0) {
        return 0;
    }

    for (i->iter = pvl_prior(i->iter); i->iter != 0; i->iter = pvl_prior(i->iter)) {

        icalcomponent *c = (icalcomponent *) pvl_data(i->iter);

        if (icalcomponent_isa(c) == i->kind || i->kind == ICAL_ANY_COMPONENT) {

            return icalcompiter_deref(i);
        }
    }

    return 0;
}

icalcomponent *icalcompiter_deref(icalcompiter *i)
{
    if (i->iter == 0) {
        return 0;
    }

    return pvl_data(i->iter);
}

icalcomponent *icalcomponent_get_inner(icalcomponent *comp)
{
    if (icalcomponent_isa(comp) == ICAL_VCALENDAR_COMPONENT) {
        return icalcomponent_get_first_real_component(comp);
    } else {
        return comp;
    }
}

void icalcomponent_set_method(icalcomponent *comp, icalproperty_method method)
{
    icalproperty *prop = icalcomponent_get_first_property(comp, ICAL_METHOD_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_method(method);
        icalcomponent_add_property(comp, prop);
    }

    icalproperty_set_method(prop, method);
}

icalproperty_method icalcomponent_get_method(icalcomponent *comp)
{
    icalproperty *prop = icalcomponent_get_first_property(comp, ICAL_METHOD_PROPERTY);

    if (prop == 0) {
        return ICAL_METHOD_NONE;
    }

    return icalproperty_get_method(prop);
}

#define ICALSETUPSET(p_kind) \
icalcomponent *inner; \
icalproperty *prop; \
icalerror_check_arg_rv(comp != 0,"comp"); \
inner = icalcomponent_get_inner(comp); \
if (inner == 0) { \
    icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR); \
    return; \
} \
prop = icalcomponent_get_first_property(inner, p_kind);

void icalcomponent_set_dtstart(icalcomponent *comp, struct icaltimetype v)
{
    const char *tzid;

    ICALSETUPSET(ICAL_DTSTART_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_dtstart(v);
        icalcomponent_add_property(inner, prop);
    } else {
        icalproperty_remove_parameter_by_kind(prop, ICAL_TZID_PARAMETER);
    }

    icalproperty_set_dtstart(prop, v);

    if ((tzid = icaltime_get_tzid(v)) != NULL && !icaltime_is_utc(v)) {
        icalproperty_add_parameter(prop, icalparameter_new_tzid(tzid));
    }
}

struct icaltimetype icalcomponent_get_dtstart(icalcomponent *comp)
{
    icalcomponent *inner = icalcomponent_get_inner(comp);
    icalproperty *prop;

    prop = icalcomponent_get_first_property(inner, ICAL_DTSTART_PROPERTY);
    if (prop == 0) {
        return icaltime_null_time();
    }

    return icalproperty_get_datetime_with_component(prop, comp);
}

struct icaltimetype icalcomponent_get_dtend(icalcomponent *comp)
{
    icalcomponent *inner = icalcomponent_get_inner(comp);
    const icalcomponent_kind kind = icalcomponent_isa(inner);
    icalproperty *end_prop, *dur_prop;
    struct icaltimetype ret;

    switch(kind) {
    case ICAL_VAVAILABILITY_COMPONENT:
    case ICAL_VEVENT_COMPONENT:
    case ICAL_VFREEBUSY_COMPONENT:
    case ICAL_XAVAILABLE_COMPONENT:
        break;
    default:
        return icaltime_null_time();
    }

    end_prop = icalcomponent_get_first_property(inner, ICAL_DTEND_PROPERTY);
    dur_prop = icalcomponent_get_first_property(inner, ICAL_DURATION_PROPERTY);

    if (end_prop != 0 && dur_prop == 0) {
        ret = icalproperty_get_datetime_with_component(end_prop, comp);
    } else if (end_prop == 0 && dur_prop != 0) {

        struct icaltimetype start = icalcomponent_get_dtstart(inner);
        struct icaldurationtype duration;

        //extra check to prevent empty durations from crashing
        if (icalproperty_get_value(dur_prop)) {
            duration = icalproperty_get_duration(dur_prop);
        } else {
            duration = icaldurationtype_null_duration();
        }

        ret = icaltime_add(start, duration);
    } else if (end_prop == 0 && dur_prop == 0) {
        if (kind == ICAL_VEVENT_COMPONENT) {
            struct icaltimetype start = icalcomponent_get_dtstart(inner);
            if (icaltime_is_date(start)) {
                struct icaldurationtype duration = icaldurationtype_null_duration();
                duration.days = 1;
                ret = icaltime_add(start, duration);
            } else {
                ret = start;
            }
        } else {
            ret = icaltime_null_time();
        }
    } else {
        /* Error, both duration and dtend have been specified */
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        ret = icaltime_null_time();
    }

    return ret;
}

void icalcomponent_set_dtend(icalcomponent *comp, struct icaltimetype v)
{
    const char *tzid;

    ICALSETUPSET(ICAL_DTEND_PROPERTY);

    if (icalcomponent_get_first_property(inner, ICAL_DURATION_PROPERTY) != NULL) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return;
    }

    if (prop == 0) {
        prop = icalproperty_new_dtend(v);
        icalcomponent_add_property(inner, prop);
    } else {
        icalproperty_remove_parameter_by_kind(prop, ICAL_TZID_PARAMETER);
    }

    icalproperty_set_dtend(prop, v);

    if ((tzid = icaltime_get_tzid(v)) != NULL && !icaltime_is_utc(v)) {
        icalproperty_add_parameter(prop, icalparameter_new_tzid(tzid));
    }
}

void icalcomponent_set_duration(icalcomponent *comp, struct icaldurationtype v)
{
    ICALSETUPSET(ICAL_DURATION_PROPERTY);

    if (icalcomponent_get_first_property(inner, ICAL_DTEND_PROPERTY) != NULL) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return;
    }

    if (prop == 0) {
        prop = icalproperty_new_duration(v);
        icalcomponent_add_property(inner, prop);
    } else {
        icalproperty_set_duration(prop, v);
    }
}

struct icaldurationtype icalcomponent_get_duration(icalcomponent *comp)
{
    icalcomponent *inner = icalcomponent_get_inner(comp);
    const icalcomponent_kind kind = icalcomponent_isa(inner);
    icalproperty *end_prop, *dur_prop;
    struct icaltimetype end;
    struct icaldurationtype ret;

    switch(kind) {
    case ICAL_VAVAILABILITY_COMPONENT:
    case ICAL_VEVENT_COMPONENT:
    case ICAL_XAVAILABLE_COMPONENT:
        end_prop = icalcomponent_get_first_property(inner, ICAL_DTEND_PROPERTY);
        if (end_prop) {
            end = icalcomponent_get_dtend(inner);
        }
        break;
    case ICAL_VTODO_COMPONENT:
        end_prop = icalcomponent_get_first_property(inner, ICAL_DUE_PROPERTY);
        if (end_prop) {
            end = icalcomponent_get_due(inner);
        }
        break;
    default:
        /* The libical API is used incorrectly */
        return icaldurationtype_null_duration();
    }

    dur_prop = icalcomponent_get_first_property(inner, ICAL_DURATION_PROPERTY);

    if (dur_prop != 0 && end_prop == 0) {
        ret = icalproperty_get_duration(dur_prop);

    } else if (end_prop != 0 && dur_prop == 0) {
        /**
         * FIXME
         * We assume DTSTART and DTEND are not in different time zones.
         * The standard actually allows different time zones.
         */
        struct icaltimetype start = icalcomponent_get_dtstart(inner);

        ret = icaltime_subtract(end, start);
    } else if (end_prop == 0 && dur_prop == 0) {
        struct icaltimetype start = icalcomponent_get_dtstart(inner);
        ret = icaldurationtype_null_duration();
        if (kind == ICAL_VEVENT_COMPONENT && icaltime_is_date(start)) {
            ret.days = 1;
        }
    } else {
        ret = icaldurationtype_null_duration();
        /* Error, both duration and dtend have been specified */
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
    }
    return ret;
}

void icalcomponent_set_dtstamp(icalcomponent *comp, struct icaltimetype v)
{
    ICALSETUPSET(ICAL_DTSTAMP_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_dtstamp(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_dtstamp(prop, v);
}

struct icaltimetype icalcomponent_get_dtstamp(icalcomponent *comp)
{
    icalcomponent *inner = icalcomponent_get_inner(comp);
    icalproperty *prop = icalcomponent_get_first_property(inner, ICAL_DTSTAMP_PROPERTY);

    if (prop == 0) {
        return icaltime_null_time();
    }

    return icalproperty_get_dtstamp(prop);
}

void icalcomponent_set_summary(icalcomponent *comp, const char *v)
{
    ICALSETUPSET(ICAL_SUMMARY_PROPERTY)

    if (prop == 0) {
        prop = icalproperty_new_summary(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_summary(prop, v);
}

const char *icalcomponent_get_summary(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_SUMMARY_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_summary(prop);
}

void icalcomponent_set_comment(icalcomponent *comp, const char *v)
{
    ICALSETUPSET(ICAL_COMMENT_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_comment(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_comment(prop, v);
}

const char *icalcomponent_get_comment(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_COMMENT_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_comment(prop);
}

void icalcomponent_set_uid(icalcomponent *comp, const char *v)
{
    ICALSETUPSET(ICAL_UID_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_uid(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_uid(prop, v);
}

const char *icalcomponent_get_uid(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_UID_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_uid(prop);
}

void icalcomponent_set_recurrenceid(icalcomponent *comp, struct icaltimetype v)
{
    ICALSETUPSET(ICAL_RECURRENCEID_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_recurrenceid(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_recurrenceid(prop, v);
}

struct icaltimetype icalcomponent_get_recurrenceid(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    if (comp == 0) {
        icalerror_set_errno(ICAL_BADARG_ERROR);
        return icaltime_null_time();
    }

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return icaltime_null_time();
    }

    prop = icalcomponent_get_first_property(inner, ICAL_RECURRENCEID_PROPERTY);

    if (prop == 0) {
        return icaltime_null_time();
    }

    return icalproperty_get_recurrenceid(prop);
}

void icalcomponent_set_description(icalcomponent *comp, const char *v)
{
    ICALSETUPSET(ICAL_DESCRIPTION_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_description(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_description(prop, v);
}

const char *icalcomponent_get_description(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_DESCRIPTION_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_description(prop);
}

void icalcomponent_set_location(icalcomponent *comp, const char *v)
{
    ICALSETUPSET(ICAL_LOCATION_PROPERTY)

    if (prop == 0) {
        prop = icalproperty_new_location(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_location(prop, v);
}

const char *icalcomponent_get_location(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_LOCATION_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_location(prop);
}

void icalcomponent_set_sequence(icalcomponent *comp, int v)
{
    ICALSETUPSET(ICAL_SEQUENCE_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_sequence(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_sequence(prop, v);
}

int icalcomponent_get_sequence(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_SEQUENCE_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_sequence(prop);
}

void icalcomponent_set_status(icalcomponent *comp, enum icalproperty_status v)
{
    ICALSETUPSET(ICAL_STATUS_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_status(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_status(prop, v);
}

enum icalproperty_status icalcomponent_get_status(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        icalerror_set_errno(ICAL_MALFORMEDDATA_ERROR);
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_STATUS_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_status(prop);
}

icalcomponent *icalcomponent_new_vcalendar(void)
{
    return icalcomponent_new(ICAL_VCALENDAR_COMPONENT);
}

icalcomponent *icalcomponent_new_vevent(void)
{
    return icalcomponent_new(ICAL_VEVENT_COMPONENT);
}

icalcomponent *icalcomponent_new_vtodo(void)
{
    return icalcomponent_new(ICAL_VTODO_COMPONENT);
}

icalcomponent *icalcomponent_new_vjournal(void)
{
    return icalcomponent_new(ICAL_VJOURNAL_COMPONENT);
}

icalcomponent *icalcomponent_new_valarm(void)
{
    return icalcomponent_new(ICAL_VALARM_COMPONENT);
}

icalcomponent *icalcomponent_new_vfreebusy(void)
{
    return icalcomponent_new(ICAL_VFREEBUSY_COMPONENT);
}

icalcomponent *icalcomponent_new_vtimezone(void)
{
    return icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);
}

icalcomponent *icalcomponent_new_xstandard(void)
{
    return icalcomponent_new(ICAL_XSTANDARD_COMPONENT);
}

icalcomponent *icalcomponent_new_xdaylight(void)
{
    return icalcomponent_new(ICAL_XDAYLIGHT_COMPONENT);
}

icalcomponent *icalcomponent_new_vagenda(void)
{
    return icalcomponent_new(ICAL_VAGENDA_COMPONENT);
}

icalcomponent *icalcomponent_new_vquery(void)
{
    return icalcomponent_new(ICAL_VQUERY_COMPONENT);
}

icalcomponent *icalcomponent_new_vreply(void)
{
    return icalcomponent_new(ICAL_VREPLY_COMPONENT);
}

icalcomponent *icalcomponent_new_vavailability(void)
{
    return icalcomponent_new(ICAL_VAVAILABILITY_COMPONENT);
}

icalcomponent *icalcomponent_new_xavailable(void)
{
    return icalcomponent_new(ICAL_XAVAILABLE_COMPONENT);
}

icalcomponent *icalcomponent_new_vpoll(void)
{
    return icalcomponent_new(ICAL_VPOLL_COMPONENT);
}

icalcomponent *icalcomponent_new_vvoter(void)
{
    return icalcomponent_new(ICAL_VVOTER_COMPONENT);
}

icalcomponent *icalcomponent_new_xvote(void)
{
    return icalcomponent_new(ICAL_XVOTE_COMPONENT);
}

icalcomponent *icalcomponent_new_vpatch(void)
{
    return icalcomponent_new(ICAL_VPATCH_COMPONENT);
}

icalcomponent *icalcomponent_new_xpatch(void)
{
    return icalcomponent_new(ICAL_XPATCH_COMPONENT);
}

icalcomponent *icalcomponent_new_participant(void)
{
    return icalcomponent_new(ICAL_PARTICIPANT_COMPONENT);
}

icalcomponent *icalcomponent_new_vlocation(void)
{
    return icalcomponent_new(ICAL_VLOCATION_COMPONENT);
}

icalcomponent *icalcomponent_new_vresource(void)
{
    return icalcomponent_new(ICAL_VRESOURCE_COMPONENT);
}

/*
 * Timezone stuff.
 */

void icalcomponent_merge_component(icalcomponent *comp, icalcomponent *comp_to_merge)
{
    icalcomponent *subcomp, *next_subcomp;
    icalarray *tzids_to_rename;
    size_t i;

    /* Check that both components are VCALENDAR components. */
    icalassert(icalcomponent_isa(comp) == ICAL_VCALENDAR_COMPONENT);
    icalassert(icalcomponent_isa(comp_to_merge) == ICAL_VCALENDAR_COMPONENT);

    /* Step through each subcomponent of comp_to_merge, looking for VTIMEZONEs.
       For each VTIMEZONE found, check if we need to add it to comp and if we
       need to rename it and all TZID references to it. */
    tzids_to_rename = icalarray_new(sizeof(char *), 16);
    if (!tzids_to_rename)
        return;

    subcomp = icalcomponent_get_first_component(comp_to_merge, ICAL_VTIMEZONE_COMPONENT);
    while (subcomp) {
        next_subcomp = icalcomponent_get_next_component(comp_to_merge, ICAL_VTIMEZONE_COMPONENT);
        /* This will add the VTIMEZONE to comp, if necessary, and also update
           the array of TZIDs we need to rename. */
        icalcomponent_merge_vtimezone(comp, subcomp, tzids_to_rename);
        /* FIXME: Handle possible NEWFAILED error. */

        subcomp = next_subcomp;
    }

    /* If we need to do any renaming of TZIDs, do it now. */
    if (tzids_to_rename->num_elements != 0) {
        icalcomponent_rename_tzids(comp_to_merge, tzids_to_rename);

        /* Now free the tzids_to_rename array. */
        for (i = 0; i < tzids_to_rename->num_elements; i++) {
            icalmemory_free_buffer(icalarray_element_at(tzids_to_rename, i));
        }
    }
    icalarray_free(tzids_to_rename);
    tzids_to_rename = 0;
    /* Now move all the components from comp_to_merge to comp, excluding
       VTIMEZONE components. */
    subcomp = icalcomponent_get_first_component(comp_to_merge, ICAL_ANY_COMPONENT);
    while (subcomp) {
        next_subcomp = icalcomponent_get_next_component(comp_to_merge, ICAL_ANY_COMPONENT);
        if (icalcomponent_isa(subcomp) != ICAL_VTIMEZONE_COMPONENT) {
            icalcomponent_remove_component(comp_to_merge, subcomp);
            icalcomponent_add_component(comp, subcomp);
        }
        subcomp = next_subcomp;
    }

    /* Free comp_to_merge. We have moved most of the subcomponents over to
       comp now. */
    icalcomponent_free(comp_to_merge);
}

static void icalcomponent_merge_vtimezone(icalcomponent *comp,
                                          icalcomponent *vtimezone, icalarray *tzids_to_rename)
{
    icalproperty *tzid_prop;
    const char *tzid;
    char *tzid_copy;
    icaltimezone *existing_vtimezone;

    /* Get the TZID of the VTIMEZONE. */
    tzid_prop = icalcomponent_get_first_property(vtimezone, ICAL_TZID_PROPERTY);
    if (!tzid_prop)
        return;

    tzid = icalproperty_get_tzid(tzid_prop);
    if (!tzid)
        return;

    /* See if there is already a VTIMEZONE in comp with the same TZID. */
    existing_vtimezone = icalcomponent_get_timezone(comp, tzid);

    /* If there is no existing VTIMEZONE with the same TZID, we can just move
       the VTIMEZONE to comp and return. */
    if (!existing_vtimezone) {
        icalcomponent_remove_component(icalcomponent_get_parent(vtimezone), vtimezone);
        icalcomponent_add_component(comp, vtimezone);
        return;
    }

    /* If the TZID has a '/' prefix, then we don't have to worry about the
       clashing TZIDs, as they are supposed to be exactly the same VTIMEZONE. */
    if (tzid[0] == '/')
        return;

    /* Now we have two VTIMEZONEs with the same TZID (which isn't a globally
       unique one), so we compare the VTIMEZONE components to see if they are
       the same. If they are, we don't need to do anything. We make a copy of
       the tzid, since the parameter may get modified in these calls. */
    tzid_copy = icalmemory_strdup(tzid);
    if (!tzid_copy) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return;
    }

    if (!icalcomponent_compare_vtimezones(comp, vtimezone)) {
        /* FIXME: Handle possible NEWFAILED error. */

        /* Now we have two different VTIMEZONEs with the same TZID. */
        icalcomponent_handle_conflicting_vtimezones(comp, vtimezone, tzid_prop,
                                                    tzid_copy, tzids_to_rename);
    }
    icalmemory_free_buffer(tzid_copy);
}

static void icalcomponent_handle_conflicting_vtimezones(icalcomponent *comp,
                                                        icalcomponent *vtimezone,
                                                        icalproperty *tzid_prop,
                                                        const char *tzid,
                                                        icalarray *tzids_to_rename)
{
    int suffix, max_suffix = 0;
    size_t i, num_elements, tzid_len;
    char *tzid_copy, *new_tzid, suffix_buf[32];

    _unused(tzid_prop);

    /* Find the length of the TZID without any trailing digits. */
    tzid_len = icalcomponent_get_tzid_prefix_len(tzid);

    /* Step through each of the VTIMEZONEs in comp. We may already have the
       clashing VTIMEZONE in the calendar, but it may have been renamed
       (i.e. a unique number added on the end of the TZID, e.g. 'London2').
       So we compare the new VTIMEZONE with any VTIMEZONEs that have the
       same prefix (e.g. 'London'). If it matches any of those, we have to
       rename the TZIDs to that TZID, else we rename to a new TZID, using
       the biggest numeric suffix found + 1. */
    num_elements = comp->timezones ? comp->timezones->num_elements : 0;
    for (i = 0; i < num_elements; i++) {
        icaltimezone *zone;
        const char *existing_tzid;
        char *existing_tzid_copy;
        size_t existing_tzid_len;

        zone = icalarray_element_at(comp->timezones, i);
        existing_tzid = icaltimezone_get_tzid(zone);

        /* Find the length of the TZID without any trailing digits. */
        existing_tzid_len = icalcomponent_get_tzid_prefix_len(existing_tzid);

        /* Check if we have the same prefix. */
        if (tzid_len == existing_tzid_len && !strncmp(tzid, existing_tzid, tzid_len)) {
            /* Compare the VTIMEZONEs. */
            if (icalcomponent_compare_vtimezones(icaltimezone_get_component(zone), vtimezone)) {
                /* The VTIMEZONEs match, so we can use the existing VTIMEZONE. But
                   we have to rename TZIDs to this TZID. */
                tzid_copy = icalmemory_strdup(tzid);
                if (!tzid_copy) {
                    icalerror_set_errno(ICAL_NEWFAILED_ERROR);
                    return;
                }
                existing_tzid_copy = icalmemory_strdup(existing_tzid);
                if (!existing_tzid_copy) {
                    icalerror_set_errno(ICAL_NEWFAILED_ERROR);
                    icalmemory_free_buffer(tzid_copy);
                } else {
                    icalarray_append(tzids_to_rename, tzid_copy);
                    icalmemory_free_buffer(tzid_copy);
                    icalarray_append(tzids_to_rename, existing_tzid_copy);
                    icalmemory_free_buffer(existing_tzid_copy);
                }
                return;
            } else {
                /* FIXME: Handle possible NEWFAILED error. */

                /* Convert the suffix to an integer and remember the maximum numeric
                   suffix found. */
                suffix = atoi(existing_tzid + existing_tzid_len);
                if (max_suffix < suffix)
                    max_suffix = suffix;
            }
        }
    }

    /* We didn't find a VTIMEZONE that matched, so we have to rename the TZID,
       using the maximum numerical suffix found + 1. */
    tzid_copy = icalmemory_strdup(tzid);
    if (!tzid_copy) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return;
    }

    snprintf(suffix_buf, sizeof(suffix_buf), "%i", max_suffix + 1);
    new_tzid = icalmemory_new_buffer(tzid_len + strlen(suffix_buf) + 1);
    if (!new_tzid) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        icalmemory_free_buffer(tzid_copy);
        return;
    }

    strncpy(new_tzid, tzid, tzid_len);
    strcpy(new_tzid + tzid_len, suffix_buf);
    icalarray_append(tzids_to_rename, tzid_copy);
    icalarray_append(tzids_to_rename, new_tzid);
    icalmemory_free_buffer(tzid_copy);
    icalmemory_free_buffer(new_tzid);
}

/* Returns the length of the TZID, without any trailing digits. */
static size_t icalcomponent_get_tzid_prefix_len(const char *tzid)
{
    size_t len;
    const char *p;

    len = strlen(tzid);
    p = tzid + len - 1;
    while (len > 0 && *p >= '0' && *p <= '9') {
        p--;
        len--;
    }

    return len;
}

/**
 * Renames all references to the given TZIDs to a new name. rename_table
 * contains pairs of strings - a current TZID, and the new TZID to rename it
 * to.
 */
static void icalcomponent_rename_tzids(icalcomponent *comp, icalarray *rename_table)
{
    icalcomponent_foreach_tzid(comp, icalcomponent_rename_tzids_callback, rename_table);
}

static void icalcomponent_rename_tzids_callback(icalparameter *param, void *data)
{
    icalarray *rename_table = data;
    const char *tzid;
    size_t i;

    tzid = icalparameter_get_tzid(param);
    if (!tzid)
        return;

    /* Step through the rename table to see if the current TZID matches
       any of the ones we want to rename. */
    for (i = 0; i < rename_table->num_elements - 1; i += 2) {
        if (!strcmp(tzid, icalarray_element_at(rename_table, i))) {
            icalparameter_set_tzid(param, icalarray_element_at(rename_table, i + 1));
            break;
        }
    }
}

void icalcomponent_foreach_tzid(icalcomponent *comp,
                                void (*callback) (icalparameter *param, void *data),
                                void *callback_data)
{
    icalproperty *prop;
    icalproperty_kind kind;
    icalparameter *param;
    icalcomponent *subcomp;

    /* First look for any TZID parameters used in this component itself. */
    prop = icalcomponent_get_first_property(comp, ICAL_ANY_PROPERTY);
    while (prop) {
        kind = icalproperty_isa(prop);

        /* These are the only properties that can have a TZID. Note that
           COMPLETED, CREATED, DTSTAMP & LASTMODIFIED must be in UTC. */
        if (kind == ICAL_DTSTART_PROPERTY ||
            kind == ICAL_DTEND_PROPERTY ||
            kind == ICAL_DUE_PROPERTY ||
            kind == ICAL_EXDATE_PROPERTY ||
            kind == ICAL_RDATE_PROPERTY) {
            param = icalproperty_get_first_parameter(prop, ICAL_TZID_PARAMETER);
            if (param)
                (*callback) (param, callback_data);
        }

        prop = icalcomponent_get_next_property(comp, ICAL_ANY_PROPERTY);
    }

    /* Now recursively check child components. */
    subcomp = icalcomponent_get_first_component(comp, ICAL_ANY_COMPONENT);
    while (subcomp) {
        icalcomponent_foreach_tzid(subcomp, callback, callback_data);
        subcomp = icalcomponent_get_next_component(comp, ICAL_ANY_COMPONENT);
    }
}

icaltimezone *icalcomponent_get_timezone(icalcomponent *comp, const char *tzid)
{
    icaltimezone *zone;
    size_t lower, middle, upper;
    int cmp;
    const char *zone_tzid;

    if (!comp->timezones)
        return NULL;

    /* Sort the array if necessary (by the TZID string). */
    if (!comp->timezones_sorted) {
        icalarray_sort(comp->timezones, icalcomponent_compare_timezone_fn);
        comp->timezones_sorted = 1;
    }

    /* Do a simple binary search. */
    lower = 0;
    upper = comp->timezones->num_elements;

    while (lower < upper) {
        middle = (lower + upper) >> 1;
        zone = icalarray_element_at(comp->timezones, middle);
        zone_tzid = icaltimezone_get_tzid(zone);
        if (zone_tzid != NULL) {
            cmp = strcmp(tzid, zone_tzid);
            if (cmp == 0) {
                return zone;
            } else if (cmp < 0) {
                upper = middle;
            } else {
                lower = middle + 1;
            }
        }
    }

    return NULL;
}

/**
 * A function to compare 2 icaltimezone elements, used for qsort().
 */
static int icalcomponent_compare_timezone_fn(const void *elem1, const void *elem2)
{
    icaltimezone *zone1, *zone2;
    const char *zone1_tzid, *zone2_tzid;

    zone1 = (icaltimezone *) elem1;
    zone2 = (icaltimezone *) elem2;

    zone1_tzid = icaltimezone_get_tzid(zone1);
    zone2_tzid = icaltimezone_get_tzid(zone2);

    return strcmp(zone1_tzid, zone2_tzid);
}

/**
 * Compares 2 VTIMEZONE components to see if they match, ignoring their TZIDs.
 * It returns 1 if they match, 0 if they don't, or -1 on error.
 */
static int icalcomponent_compare_vtimezones(icalcomponent *vtimezone1, icalcomponent *vtimezone2)
{
    icalproperty *prop1, *prop2;
    const char *tzid1, *tzid2;
    char *tzid2_copy, *string1, *string2;
    int cmp;

    /* Get the TZID property of the first VTIMEZONE. */
    prop1 = icalcomponent_get_first_property(vtimezone1, ICAL_TZID_PROPERTY);
    if (!prop1)
        return -1;

    tzid1 = icalproperty_get_tzid(prop1);
    if (!tzid1)
        return -1;

    /* Get the TZID property of the second VTIMEZONE. */
    prop2 = icalcomponent_get_first_property(vtimezone2, ICAL_TZID_PROPERTY);
    if (!prop2)
        return -1;

    tzid2 = icalproperty_get_tzid(prop2);
    if (!tzid2)
        return -1;

    /* Copy the second TZID, and set the property to the same as the first
       TZID, since we don't care if these match of not. */
    tzid2_copy = icalmemory_strdup(tzid2);
    if (!tzid2_copy) {
        icalerror_set_errno(ICAL_NEWFAILED_ERROR);
        return 0;
    }

    icalproperty_set_tzid(prop2, tzid1);

    /* Now convert both VTIMEZONEs to strings and compare them. */
    string1 = icalcomponent_as_ical_string_r(vtimezone1);
    if (!string1) {
        icalmemory_free_buffer(tzid2_copy);
        return -1;
    }

    string2 = icalcomponent_as_ical_string_r(vtimezone2);
    if (!string2) {
        icalmemory_free_buffer(string1);
        icalmemory_free_buffer(tzid2_copy);
        return -1;
    }

    cmp = strcmp(string1, string2);

    icalmemory_free_buffer(string1);
    icalmemory_free_buffer(string2);

    /* Now reset the second TZID. */
    icalproperty_set_tzid(prop2, tzid2_copy);
    icalmemory_free_buffer(tzid2_copy);

    return (cmp == 0) ? 1 : 0;
}

/**
 * @brief Sets the RELCALID property of a component.
 *
 * @param comp    Valid calendar component.
 * @param v       Relcalid URL value
 */

void icalcomponent_set_relcalid(icalcomponent *comp, const char *v)
{
    ICALSETUPSET(ICAL_RELCALID_PROPERTY);

    if (prop == 0) {
        prop = icalproperty_new_relcalid(v);
        icalcomponent_add_property(inner, prop);
    }

    icalproperty_set_relcalid(prop, v);
}

/**
 * @brief Gets the RELCALID property of a component.
 *
 * @param comp    Valid calendar component.
 */

const char *icalcomponent_get_relcalid(icalcomponent *comp)
{
    icalcomponent *inner;
    icalproperty *prop;

    icalerror_check_arg_rz(comp != 0, "comp");

    inner = icalcomponent_get_inner(comp);

    if (inner == 0) {
        return 0;
    }

    prop = icalcomponent_get_first_property(inner, ICAL_RELCALID_PROPERTY);

    if (prop == 0) {
        return 0;
    }

    return icalproperty_get_relcalid(prop);
}

struct icaltimetype icalcomponent_get_due(icalcomponent *comp)
{
    icalcomponent *inner = icalcomponent_get_inner(comp);

    icalproperty *due_prop = icalcomponent_get_first_property(inner, ICAL_DUE_PROPERTY);

    icalproperty *dur_prop = icalcomponent_get_first_property(inner, ICAL_DURATION_PROPERTY);

    if (due_prop != 0) {
        return icalproperty_get_datetime_with_component(due_prop, comp);
    } else if (dur_prop != 0) {

        struct icaltimetype start = icalcomponent_get_dtstart(inner);
        struct icaldurationtype duration = icalproperty_get_duration(dur_prop);

        struct icaltimetype due = icaltime_add(start, duration);

        return due;
    }
    return icaltime_null_time();
}

void icalcomponent_set_due(icalcomponent *comp, struct icaltimetype v)
{
    const char *tzid;

    icalcomponent *inner = icalcomponent_get_inner(comp);

    icalproperty *due_prop = icalcomponent_get_first_property(inner, ICAL_DUE_PROPERTY);

    icalproperty *dur_prop = icalcomponent_get_first_property(inner, ICAL_DURATION_PROPERTY);

    if (due_prop == 0 && dur_prop == 0) {
        due_prop = icalproperty_new_due(v);
        icalcomponent_add_property(inner, due_prop);
    } else if (due_prop != 0) {
        icalproperty_set_due(due_prop, v);
        icalproperty_remove_parameter_by_kind(due_prop, ICAL_TZID_PARAMETER);
    } else if (dur_prop != 0) {
        struct icaltimetype start = icalcomponent_get_dtstart(inner);

        struct icaltimetype due = icalcomponent_get_due(inner);

        struct icaldurationtype dur = icaltime_subtract(due, start);

        icalproperty_set_duration(dur_prop, dur);
    }

    if (due_prop && (tzid = icaltime_get_tzid(v)) != NULL && !icaltime_is_utc(v)) {
        icalproperty_set_parameter(due_prop, icalparameter_new_tzid(tzid));
    }
}

static int strcmpsafe(const char *a, const char *b)
{
    return strcmp((a == NULL ? "" : a),
                  (b == NULL ? "" : b));
}

static int prop_compare(void *a, void *b)
{
    icalproperty *p1 = (icalproperty*) a;
    icalproperty *p2 = (icalproperty*) b;
    icalproperty_kind k1 = icalproperty_isa(p1);
    icalproperty_kind k2 = icalproperty_isa(p2);
    int r = k1 - k2;

    if (r == 0) {
        if (k1 == ICAL_X_PROPERTY) {
            r = strcmp(icalproperty_get_x_name(p1),
                       icalproperty_get_x_name(p2));
        }

        if (r == 0) {
            r = strcmpsafe(icalproperty_get_value_as_string(p1),
                           icalproperty_get_value_as_string(p2));
        }
    }

    return r;
}

static int comp_compare(void *a, void *b)
{
    icalcomponent *c1 = (icalcomponent*) a;
    icalcomponent *c2 = (icalcomponent*) b;
    icalcomponent_kind k1 = icalcomponent_isa(c1);
    icalcomponent_kind k2 = icalcomponent_isa(c2);
    int r = k1 - k2;

    if (r == 0) {
        if (k1 == ICAL_X_COMPONENT) {
            r = strcmp(c1->x_name, c2->x_name);
        }

        if (r == 0) {
            const char *u1 = icalcomponent_get_uid(c1);
            const char *u2 = icalcomponent_get_uid(c2);

            if (u1 && u2) {
                r = strcmp(u1, u2);

                if (r == 0) {
                    r = icaltime_compare(icalcomponent_get_recurrenceid(c1),
                                         icalcomponent_get_recurrenceid(c2));
                }
            } else {
                icalproperty *p1, *p2;

                switch (k1) {
                case ICAL_VALARM_COMPONENT:
                    p1 = icalcomponent_get_first_property(c1,
                                                          ICAL_TRIGGER_PROPERTY);
                    p2 = icalcomponent_get_first_property(c2,
                                                          ICAL_TRIGGER_PROPERTY);
                    r = strcmp(icalproperty_get_value_as_string(p1),
                               icalproperty_get_value_as_string(p2));

                    if (r == 0) {
                        p1 = icalcomponent_get_first_property(c1,
                                                              ICAL_ACTION_PROPERTY);
                        p2 = icalcomponent_get_first_property(c2,
                                                              ICAL_ACTION_PROPERTY);
                        r = strcmp(icalproperty_get_value_as_string(p1),
                                   icalproperty_get_value_as_string(p2));
                    }
                    break;

                case ICAL_VTIMEZONE_COMPONENT:
                    p1 = icalcomponent_get_first_property(c1,
                                                          ICAL_TZID_PROPERTY);
                    p2 = icalcomponent_get_first_property(c2,
                                                          ICAL_TZID_PROPERTY);
                    r = strcmp(icalproperty_get_value_as_string(p1),
                               icalproperty_get_value_as_string(p2));
                    break;

                case ICAL_XSTANDARD_COMPONENT:
                case ICAL_XDAYLIGHT_COMPONENT:
                    p1 = icalcomponent_get_first_property(c1,
                                                          ICAL_DTSTART_PROPERTY);
                    p2 = icalcomponent_get_first_property(c2,
                                                          ICAL_DTSTART_PROPERTY);
                    r = strcmp(icalproperty_get_value_as_string(p1),
                               icalproperty_get_value_as_string(p2));
                    break;

                case ICAL_VVOTER_COMPONENT:
                    p1 = icalcomponent_get_first_property(c1,
                                                          ICAL_VOTER_PROPERTY);
                    p2 = icalcomponent_get_first_property(c2,
                                                          ICAL_VOTER_PROPERTY);
                    r = strcmp(icalproperty_get_value_as_string(p1),
                               icalproperty_get_value_as_string(p2));
                    break;

                case ICAL_XVOTE_COMPONENT:
                    p1 = icalcomponent_get_first_property(c1,
                                                          ICAL_POLLITEMID_PROPERTY);
                    p2 = icalcomponent_get_first_property(c2,
                                                          ICAL_POLLITEMID_PROPERTY);
                    r = strcmp(icalproperty_get_value_as_string(p1),
                               icalproperty_get_value_as_string(p2));
                    break;

                default:
                    /* XXX  Anything better? */
                    r = icaltime_compare(icalcomponent_get_dtstamp(c1),
                                         icalcomponent_get_dtstamp(c2));
                    break;
                }
            }
        }
    /* Always sort VTIMEZONEs first */
    } else if (k1 == ICAL_VTIMEZONE_COMPONENT) {
        return -1;
    } else if (k2 == ICAL_VTIMEZONE_COMPONENT) {
        return 1;
    }

    return r;
}

void icalcomponent_normalize(icalcomponent *comp)
{
    pvl_list sorted_props = pvl_newlist();
    pvl_list sorted_comps = pvl_newlist();
    icalproperty *prop;
    icalcomponent *sub;

    /* Normalize properties into sorted list */
    while ((prop = pvl_pop(comp->properties)) != 0) {
        int nparams, remove = 0;

        icalproperty_normalize(prop);

        nparams = icalproperty_count_parameters(prop);

        /* Remove unparameterized properties having default values */
        if (nparams == 0) {
            switch (icalproperty_isa(prop)) {
            case ICAL_CALSCALE_PROPERTY:
                if (strcmp("GREGORIAN", icalproperty_get_calscale(prop)) == 0) {
                    remove = 1;
                }
                break;

            case ICAL_CLASS_PROPERTY:
                if (icalproperty_get_class(prop) == ICAL_CLASS_PUBLIC) {
                    remove = 1;
                }
                break;

            case ICAL_PRIORITY_PROPERTY:
                if (icalproperty_get_priority(prop) == 0) {
                    remove = 1;
                }
                break;

            case ICAL_TRANSP_PROPERTY:
                if (icalproperty_get_transp(prop) == ICAL_TRANSP_OPAQUE) {
                    remove = 1;
                }
                break;

            case ICAL_REPEAT_PROPERTY:
                if (icalproperty_get_repeat(prop) == 0) {
                    remove = 1;
                }
                break;

            case ICAL_SEQUENCE_PROPERTY:
                if (icalproperty_get_sequence(prop) == 0) {
                    remove = 1;
                }
                break;

            default:
                break;
            }
        }

        if (remove) {
            icalproperty_set_parent(prop, 0); // MUST NOT have a parent to free
            icalproperty_free(prop);
        } else {
            pvl_insert_ordered(sorted_props, prop_compare, prop);
        }
    }

    pvl_free(comp->properties);
    comp->properties = sorted_props;

    /* Normalize sub-components into sorted list */
    while ((sub = pvl_pop(comp->components)) != 0) {
        icalcomponent_normalize(sub);
        pvl_insert_ordered(sorted_comps, comp_compare, sub);
    }

    pvl_free(comp->components);
    comp->components = sorted_comps;
}
