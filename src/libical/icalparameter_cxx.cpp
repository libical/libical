/**
 * @file    icalparameter_cxx.cpp
 * @author  fnguyen (12/10/01)
 * @brief   Implementation of C++ Wrapper for icalparameter.c
 *
 * SPDX-FileCopyrightText: 2001, Critical Path
 * SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#include "icalparameter_cxx.h"
using namespace LibICal;

ICalParameter::ICalParameter()
    : imp(icalparameter_new(ICAL_ANY_PARAMETER))
{
}

ICalParameter::ICalParameter(const ICalParameter &v)
    : imp(icalparameter_clone(v.imp))
{
    if (imp == NULL) {
        throw icalerrno;
    }
}

ICalParameter &ICalParameter::operator=(const ICalParameter &v)
{
    if (this == &v) {
        return *this;
    }

    if (imp != NULL) {
        icalparameter_free(imp);
        imp = icalparameter_clone(v.imp);
        if (imp == NULL) {
            throw icalerrno;
        }
    }

    return *this;
}

void ICalParameter::detach()
{
    imp = NULL;
}

ICalParameter::~ICalParameter()
{
    if (imp != NULL) {
        icalparameter_free(imp);
    }
}

ICalParameter::ICalParameter(icalparameter *v)
    : imp(v)
{
}

/// Create from string of form "PARAMNAME=VALUE"
ICalParameter::ICalParameter(const std::string &str)
    : imp(icalparameter_new_from_string(str.c_str()))
{
    if (imp == NULL) {
        throw icalerrno;
    }
}

/// Create from just the value, the part after the "="
ICalParameter::ICalParameter(const icalparameter_kind &kind, const std::string &str)
    : imp(icalparameter_new_from_value_string(kind, str.c_str()))
{
    if (imp == NULL) {
        throw icalerrno;
    }
}

ICalParameter::ICalParameter(const icalparameter_kind &kind)
    : imp(icalparameter_new(kind))
{
    if (imp == NULL) {
        throw icalerrno;
    }
}

std::string ICalParameter::as_ical_string()
{
    char *str = icalparameter_as_ical_string(imp);

    if (str == NULL) {
        throw icalerrno;
    }

    return str;
}

bool ICalParameter::is_valid()
{
    if (imp == NULL) {
        return false;
    }
    return (icalparameter_isa_parameter(static_cast<void *>(imp)) != 0);
}

icalparameter_kind ICalParameter::isa()
{
    return icalparameter_isa(imp);
}

int ICalParameter::isa_parameter(void *param)
{
    return icalparameter_isa_parameter(param);
}

/* Access the name of an X parameter */
/* cppcheck-suppress constParameterReference */
void ICalParameter::set_xname(ICalParameter &param, const std::string &v)
{
    icalparameter_set_xname(param, v.c_str());
}

/* cppcheck-suppress constParameterReference */
std::string ICalParameter::get_xname(ICalParameter &param)
{
    return static_cast<std::string>(icalparameter_get_xname(param));
}

/* cppcheck-suppress constParameterReference */
void ICalParameter::set_xvalue(ICalParameter &param, const std::string &v)
{
    icalparameter_set_xvalue(param, v.c_str());
}

/* cppcheck-suppress constParameterReference */
std::string ICalParameter::get_xvalue(ICalParameter &param)
{
    return static_cast<std::string>(icalparameter_get_xvalue(param));
}

/* Convert enumerations */
std::string ICalParameter::kind_to_string(const icalparameter_kind &kind)
{
    return static_cast<std::string>(icalparameter_kind_to_string(kind));
}

icalparameter_kind ICalParameter::string_to_kind(const std::string &str)
{
    return icalparameter_string_to_kind(str.c_str());
}

/* DELEGATED-FROM */
std::vector<std::string> ICalParameter::get_delegatedfrom() const
{
    std::vector<std::string> vals;
    icalstrarray *c_vals = icalparameter_get_delegatedfrom(imp);
    if (c_vals) {
        for (size_t i = 0; i < icalstrarray_size(c_vals); ++i) {
            vals.push_back(std::string(icalstrarray_element_at(c_vals, i)));
        }
    }
    return vals;
}

void ICalParameter::set_delegatedfrom(const std::vector<std::string> &v)
{
    icalstrarray *c_vals = icalstrarray_new(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        icalstrarray_append(c_vals, v[i].c_str());
    }
    icalparameter_set_delegatedfrom(imp, c_vals);
}

/* RELATED */
icalparameter_related ICalParameter::get_related() const
{
    return icalparameter_get_related(imp);
}

void ICalParameter::set_related(const icalparameter_related &v)
{
    icalparameter_set_related(imp, v);
}

/* SENT-BY */
std::string ICalParameter::get_sentby() const
{
    return static_cast<std::string>(icalparameter_get_sentby(imp));
}

void ICalParameter::set_sentby(const std::string &v)
{
    icalparameter_set_sentby(imp, v.c_str());
}

/* LANGUAGE */
std::string ICalParameter::get_language() const
{
    return static_cast<std::string>(icalparameter_get_language(imp));
}

void ICalParameter::set_language(const std::string &v)
{
    icalparameter_set_language(imp, v.c_str());
}

/* RELTYPE */
icalparameter_reltype ICalParameter::get_reltype() const
{
    return icalparameter_get_reltype(imp);
}

void ICalParameter::set_reltype(const icalparameter_reltype &v)
{
    icalparameter_set_reltype(imp, v);
}

/* ENCODING */
icalparameter_encoding ICalParameter::get_encoding() const
{
    return icalparameter_get_encoding(imp);
}

void ICalParameter::set_encoding(const icalparameter_encoding &v)
{
    icalparameter_set_encoding(imp, v);
}

/* ALTREP */
std::string ICalParameter::get_altrep() const
{
    return static_cast<std::string>(icalparameter_get_language(imp));
}

void ICalParameter::set_altrep(const std::string &v)
{
    icalparameter_set_altrep(imp, v.c_str());
}

/* FMTTYPE */
std::string ICalParameter::get_fmttype() const
{
    return static_cast<std::string>(icalparameter_get_fmttype(imp));
}

void ICalParameter::set_fmttype(const std::string &v)
{
    icalparameter_set_fmttype(imp, v.c_str());
}

/* FBTYPE */
icalparameter_fbtype ICalParameter::get_fbtype() const
{
    return icalparameter_get_fbtype(imp);
}

void ICalParameter::set_fbtype(const icalparameter_fbtype &v)
{
    icalparameter_set_fbtype(imp, v);
}

/* RSVP */
icalparameter_rsvp ICalParameter::get_rsvp() const
{
    return icalparameter_get_rsvp(imp);
}

void ICalParameter::set_rsvp(const icalparameter_rsvp &v)
{
    icalparameter_set_rsvp(imp, v);
}

/* RANGE */
icalparameter_range ICalParameter::get_range() const
{
    return icalparameter_get_range(imp);
}

void ICalParameter::set_range(const icalparameter_range &v)
{
    icalparameter_set_range(imp, v);
}

/* DELEGATED-TO */
std::vector<std::string> ICalParameter::get_delegatedto() const
{
    std::vector<std::string> vals;
    icalstrarray *c_vals = icalparameter_get_delegatedto(imp);
    if (c_vals) {
        for (size_t i = 0; i < icalstrarray_size(c_vals); ++i) {
            vals.push_back(std::string(icalstrarray_element_at(c_vals, i)));
        }
    }
    return vals;
}

void ICalParameter::set_delegatedto(const std::vector<std::string> &v)
{
    icalstrarray *c_vals = icalstrarray_new(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        icalstrarray_append(c_vals, v[i].c_str());
    }
    icalparameter_set_delegatedto(imp, c_vals);
}

/* CN */
std::string ICalParameter::get_cn() const
{
    return static_cast<std::string>(icalparameter_get_cn(imp));
}

void ICalParameter::set_cn(const std::string &v)
{
    icalparameter_set_cn(imp, v.c_str());
}

/* ROLE */
icalparameter_role ICalParameter::get_role() const
{
    return icalparameter_get_role(imp);
}

void ICalParameter::set_role(const icalparameter_role &v)
{
    icalparameter_set_role(imp, v);
}

/* X-LIC-COMPARETYPE */
icalparameter_xliccomparetype ICalParameter::get_xliccomparetype() const
{
    return icalparameter_get_xliccomparetype(imp);
}

void ICalParameter::set_xliccomparetype(const icalparameter_xliccomparetype &v)
{
    icalparameter_set_xliccomparetype(imp, v);
}

/* PARTSTAT */
icalparameter_partstat ICalParameter::get_partstat() const
{
    return icalparameter_get_partstat(imp);
}

void ICalParameter::set_partstat(const icalparameter_partstat &v)
{
    icalparameter_set_partstat(imp, v);
}

/* X-LIC-ERRORTYPE */
icalparameter_xlicerrortype ICalParameter::get_xlicerrortype() const
{
    return icalparameter_get_xlicerrortype(imp);
}

void ICalParameter::set_xlicerrortype(const icalparameter_xlicerrortype &v)
{
    icalparameter_set_xlicerrortype(imp, v);
}

/* MEMBER */
std::vector<std::string> ICalParameter::get_member() const
{
    std::vector<std::string> vals;
    icalstrarray *c_vals = icalparameter_get_member(imp);
    if (c_vals) {
        for (size_t i = 0; i < icalstrarray_size(c_vals); ++i) {
            vals.push_back(std::string(icalstrarray_element_at(c_vals, i)));
        }
    }
    return vals;
}

void ICalParameter::set_member(const std::vector<std::string> &v)
{
    icalstrarray *c_vals = icalstrarray_new(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        icalstrarray_append(c_vals, v[i].c_str());
    }
    icalparameter_set_member(imp, c_vals);
}

/* X */
std::string ICalParameter::get_x() const
{
    return static_cast<std::string>(icalparameter_get_x(imp));
}

void ICalParameter::set_x(const std::string &v)
{
    icalparameter_set_x(imp, v.c_str());
}

/* CUTYPE */
icalparameter_cutype ICalParameter::get_cutype() const
{
    return icalparameter_get_cutype(imp);
}

void ICalParameter::set_cutype(const icalparameter_cutype &v)
{
    icalparameter_set_cutype(imp, v);
}

/* TZID */
std::string ICalParameter::get_tzid() const
{
    return static_cast<std::string>(icalparameter_get_tzid(imp));
}

void ICalParameter::set_tzid(const std::string &v)
{
    icalparameter_set_tzid(imp, v.c_str());
}

/* VALUE */
icalparameter_value ICalParameter::get_value() const
{
    return icalparameter_get_value(imp);
}

void ICalParameter::set_value(const icalparameter_value &v)
{
    icalparameter_set_value(imp, v);
}

/* DIR */
std::string ICalParameter::get_dir() const
{
    return static_cast<std::string>(icalparameter_get_dir(imp));
}

void ICalParameter::set_dir(const std::string &v)
{
    icalparameter_set_dir(imp, v.c_str());
}
