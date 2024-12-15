# Known Limitations

List where libical doesn't fully support all the features described in the RFCs.

Contributions to help us fix these limitations are welcome.

## RFC 5545

### Recurrence Rule section 3.3.10

* BYSETPOS is only supported for MONTHLY and YEARLY frequencies (<https://github.com/libical/libical/issues/795>)

* FREQ=YEARLY, BYYEARDAY can't be combined with BYWEEKNO, BYMONTH or BYMONTHDAY

     ref: <https://github.com/libical/libical/blob/cfd401b9d043214395888de1d9daf52263e3245b/src/libical/icalrecur.c#L2882>

* FREQ=YEARLY, BYWEEKNO can't be combined with BYYEARDAY, BYMONTH or BYMONTHDAY

     ref: <https://github.com/libical/libical/blob/cfd401b9d043214395888de1d9daf52263e3245b/src/libical/icalrecur.c#L2928>

### Recurrence Rule sections 3.8.5.3, DURATION 3.3.6

* The lib (e.g. `icalcomponent_foreach_recurrence()`, `icalcomponent_get_dtend()`) does not
  differentiate between nominal and exact durations. According to the RFC, when a component's
  duration is specified using the `DURATION` property rather than `DTEND`, the nominal duration
  must be used to calculate each recurrence's duration. However, the current implementation always
  uses the exact duration, which can cause discrepancies if a recurrence spans a daylight saving
  time transition.

     ref: <https://github.com/libical/libical/issues/630>
