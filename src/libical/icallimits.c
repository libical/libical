/**
 *  SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
 *  SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "icallimits.h"
#include "icalerror.h"

static ICAL_GLOBAL_VAR size_t _MAX_PARSE_FAILURES = 1000;
static ICAL_GLOBAL_VAR size_t _MAX_PARSE_SEARCH = 100000;
static ICAL_GLOBAL_VAR size_t _MAX_PARSE_FAILURE_ERROR_MESSAGES = 100;
static ICAL_GLOBAL_VAR size_t _MAX_PROPERTIES = 10000;
static ICAL_GLOBAL_VAR size_t _MAX_PARAMETERS = 100;
static ICAL_GLOBAL_VAR size_t _MAX_VALUE_CHARS = 10485760;
static ICAL_GLOBAL_VAR size_t _MAX_PROPERTY_VALUES = 500;
static ICAL_GLOBAL_VAR size_t _MAX_RECURRENCE_SEARCH = 10000000;
static ICAL_GLOBAL_VAR size_t _MAX_RECURRENCE_TIME_STANDING_STILL = 50;
static ICAL_GLOBAL_VAR size_t _MAX_RRULE_SEARCH = 100;

size_t icallimit_get(icallimits_kind kind)
{
    switch (kind) {
    case ICAL_LIMIT_PARSE_FAILURES:
        return _MAX_PARSE_FAILURES;
    case ICAL_LIMIT_PARSE_SEARCH:
        return _MAX_PARSE_SEARCH;
    case ICAL_LIMIT_PARSE_FAILURE_ERROR_MESSAGES:
        return _MAX_PARSE_FAILURE_ERROR_MESSAGES;
    case ICAL_LIMIT_PROPERTIES:
        return _MAX_PROPERTIES;
    case ICAL_LIMIT_PARAMETERS:
        return _MAX_PARAMETERS;
    case ICAL_LIMIT_VALUE_CHARS:
        return _MAX_VALUE_CHARS;
    case ICAL_LIMIT_PROPERTY_VALUES:
        return _MAX_PROPERTY_VALUES;
    case ICAL_LIMIT_RECURRENCE_SEARCH:
        return _MAX_RECURRENCE_SEARCH;
    case ICAL_LIMIT_RECURRENCE_TIME_STANDING_STILL:
        return _MAX_RECURRENCE_TIME_STANDING_STILL;
    case ICAL_LIMIT_RRULE_SEARCH:
        return _MAX_RRULE_SEARCH;
    default:
        icalassert(0);
        return 0;
    }
}

void icallimit_set(icallimits_kind kind, size_t limit)
{
    switch (kind) {
    case ICAL_LIMIT_PARSE_FAILURES:
        _MAX_PARSE_FAILURES = limit;
        break;
    case ICAL_LIMIT_PARSE_SEARCH:
        _MAX_PARSE_SEARCH = limit;
        break;
    case ICAL_LIMIT_PARSE_FAILURE_ERROR_MESSAGES:
        _MAX_PARSE_FAILURE_ERROR_MESSAGES = limit;
        break;
    case ICAL_LIMIT_PROPERTIES:
        _MAX_PROPERTIES = limit;
        break;
    case ICAL_LIMIT_PARAMETERS:
        _MAX_PARAMETERS = limit;
        break;
    case ICAL_LIMIT_VALUE_CHARS:
        _MAX_VALUE_CHARS = limit;
        break;
    case ICAL_LIMIT_PROPERTY_VALUES:
        _MAX_PROPERTY_VALUES = limit;
        break;
    case ICAL_LIMIT_RECURRENCE_SEARCH:
        _MAX_RECURRENCE_SEARCH = limit;
        break;
    case ICAL_LIMIT_RECURRENCE_TIME_STANDING_STILL:
        _MAX_RECURRENCE_TIME_STANDING_STILL = limit;
        break;
    case ICAL_LIMIT_RRULE_SEARCH:
        _MAX_RRULE_SEARCH = limit;
        break;
    default:
        icalassert(0);
    }
}
