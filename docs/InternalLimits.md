# Internal Limits

In order to keep processing times from getting out of hand for huge (or bad) input data
libical enforces these internal limits.  However, these limits can be changed at runtime
using the `icallimit_set()` function. Similarly, the limits can be queried using `icallimit_get()`.

The internal limits and their default values are shown here:

* Maximum number of parse failures allowed in a calendar file before processing halts

      Kind = ICAL_LIMIT_PARSE_FAILURES
      Default = 1000 (one thousand)

* Maximum number of parse characters to search in a calendar component for the next parameter or property

      Kind = ICAL_LIMIT_PARSE_SEARCH
      Default = 100000 (one hundred thousand)

* Maximum number of parse failure messages inserted into the output (by `insert_error()`)

      Kind = ICAL_LIMIT_PARSE_FAILURE_ERROR_MESSAGES
      Default = 100 (one hundred)

* Maximum number of properties allowed in a calendar component

      Kind = ICAL_LIMIT_PROPERTIES
      Default = 10000 (ten thousand)

* Maximum number of parameters allowed for a property

      Kind = ICAL_LIMIT_PARAMETERS
      Default = 100 (one hundred)

* Maximum number of characters for a value

      Kind = ICAL_LIMIT_VALUE_CHARS
      Default = 10485760 (10 * 1024 * 1024 ie. 10 mega chars)

* Maximum number of values allowed for multi-valued properties

      Kind = ICAL_LIMIT_PROPERTY_VALUES
      Default = 500

* Maximum number of times to search for the next recurrence before giving up

      Kind = ICAL_LIMIT_RECURRENCE_SEARCH
      Default = 10000000 (ten million)

* Maximum number of times to search for the next recurrence before time is considered standing still

      Kind = ICAL_LIMIT_RECURRENCE_TIME_STANDING_STILL
      Default = 50 (fifty)

* Maximum number of times to search vtimezone rrules for an occurrence before the specified end year

      Kind = ICAL_LIMIT_RRULE_SEARCH
      Default = 100 (one hundred)
