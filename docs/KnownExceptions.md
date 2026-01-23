# Known RFC Exceptions

List where libical doesn't fully follow or support all the features described in the RFCs.

Contributions to help us fix these limitations are welcome.

## RFC 5545

### Event/To-Do/Journal/Free-Busy Component section 3.6.[1-4]

libical does *not* require Event/To-Do/Journal/FreeBusy components to contain exactly one
UID and DTSTAMP property - i.e., a missing UID or DTSTAMP property will be ignored and it is
the calling application's job to deal with these missing properties as they see fit.

### Recurrence Rule section 3.3.10

* FREQ=YEARLY, BYYEARDAY can't be combined with BYWEEKNO, BYMONTH or BYMONTHDAY

     ref: <https://github.com/libical/libical/blob/cfd401b9d043214395888de1d9daf52263e3245b/src/libical/icalrecur.c#L2882>

* FREQ=YEARLY, BYWEEKNO can't be combined with BYYEARDAY, BYMONTH or BYMONTHDAY

     ref: <https://github.com/libical/libical/blob/cfd401b9d043214395888de1d9daf52263e3245b/src/libical/icalrecur.c#L2928>

### DATE-TIME section 3.3.5

The RFC states for "FORM #3: DATE WITH LOCAL TIME AND TIME ZONE REFERENCE":

> If, based on the definition of the referenced time zone, the local
> time described occurs more than once (when changing from daylight
> to standard time), the DATE-TIME value refers to the first
> occurrence of the referenced time.

This only is the case in libical for `icaltimetype` values having the`is_daylight` field set,
but this field is not set by the icalparser when parsing DATE-TIME values. As a consequence,
durations calculated from such ambiguous datetimes are incorrect.
