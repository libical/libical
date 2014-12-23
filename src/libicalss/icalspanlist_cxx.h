/**
 * @file     icalspanlist_cxx.h
 * @author   Critical Path
 * @brief    C++ class wrapping the icalspanlist data structure
 *

 (C) COPYRIGHT 2001, Critical Path

 This program is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

  Or:

    The Mozilla Public License Version 1.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 */

#ifndef ICALSPANLIST_CXX_H
#define ICALSPANLIST_CXX_H

extern "C" {
#include <libical/ical.h>
#include <libicalss/icalset.h>
#include <libicalss/icalspanlist.h>
}
#include "vcomponent.h"
#include <vector>       /* For as_matrix.. */

/**
 * This class wraps the icalspanlist routines in libicalss
 *
 * Errors within libicalss are propagated via exceptions of type
 * icalerrorenum.  See icalerror.h for the complete list of exceptions
 * that might be thrown.
 */
class ICalSpanList
{
public:
    /** Construct an ICalSpanList from an icalset */
    ICalSpanList(icalset *set, icaltimetype start, icaltimetype end) throw(icalerrorenum);

    /** Construct an ICalSpanList from the VFREEBUSY chunk of a icalcomponent */
    explicit ICalSpanList(icalcomponent *comp) throw(icalerrorenum);

    /** Construct an ICalSpanList from the VFREEBUSY chunk of a vcomponent */
    explicit ICalSpanList(VComponent &comp) throw(icalerrorenum);

    /** Destructor */
    ~ICalSpanList();

    /** Return a VFREEBUSY icalcomponent */
    VComponent *get_vfreebusy(const char *organizer, const char *attendee) throw(icalerrorenum);

    /** Return the base data when casting */
    operator icalspanlist *()
    {
        return data;
    }

    /** Return a vector of the number of events over delta t */
    std::vector<int> as_vector(int delta_t) throw(icalerrorenum);

    /** Dump the spanlist to stdout */
    void dump();

private:
    icalspanlist *data;
};

#endif
