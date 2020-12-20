/**
 * @file     icalspanlist_cxx.h
 * @author   Critical Path
 * @brief    C++ class wrapping the icalspanlist data structure
 *
 * (C) COPYRIGHT 2001, Critical Path
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of either:
 *
 *   The LGPL as published by the Free Software Foundation, version
 *   2.1, available at: https://www.gnu.org/licenses/lgpl-2.1.html
 *
 * Or:
 *
 *   The Mozilla Public License Version 2.0. You may obtain a copy of
 *   the License at https://www.mozilla.org/MPL/
 */

#ifndef ICALSPANLIST_CXX_H
#define ICALSPANLIST_CXX_H

#include "libical_icalss_export.h"

extern "C"
{
#include "icalcomponent.h"
#include "icalspanlist.h"
#include "icaltime.h"
}

#include <vector>

namespace LibICal
{

class VComponent;

/**
 * This class wraps the icalspanlist routines in libicalss
 *
 * Errors within libicalss are propagated via exceptions of type
 * icalerrorenum.  See icalerror.h for the complete list of exceptions
 * that might be thrown.
 */
class LIBICAL_ICALSS_EXPORT ICalSpanList
{
public:
    ICalSpanList();
    ICalSpanList(const ICalSpanList &v);

    /** Constructs an ICalSpanList from an icalset */
    ICalSpanList(icalset *set, icaltimetype start, icaltimetype end);

    /** Constructs an ICalSpanList from the VFREEBUSY chunk of a icalcomponent */
    explicit ICalSpanList(icalcomponent *comp);

    /** Constructs an ICalSpanList from the VFREEBUSY chunk of a vcomponent */
    explicit ICalSpanList(VComponent &comp);

    /** Destructor */
    ~ICalSpanList();

    /** Returns a VFREEBUSY icalcomponent */
    VComponent *get_vfreebusy(const char *organizer, const char *attendee);

    ICalSpanList &operator=(const ICalSpanList &);

    /** Returns the base data when casting */
    operator  icalspanlist *()
    {
        return data;
    }

    /** Returns a vector of the number of events over delta t */
    std::vector < int >as_vector(int delta_t);

    /** Dumps the spanlist to STDOUT */
    void dump();

private:
    icalspanlist *data;
};

}       // namespace LibICal;

#endif
