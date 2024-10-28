# Known Limitations

List where libical doesn't fully support all the features described in the RFCs.

Contributions to help us fix these limitations are welcome.

## RFC 5545

### Recurrence Rule section 3.3.10

* BYSETPOS is only supported for MONTHLY and YEARLY frequencies (<https://github.com/libical/libical/issues/795>)

* When used as a limit (rather than for expansion) negative values are not supported
  for BYMONTHDAY, BYYEARDAY (<https://github.com/libical/libical/issues/791>)

* FREQ=YEARLY, BYYEARDAY can't be combined with BYWEEKNO, BYMONTH or BYMONTHDAY

     ref: <https://github.com/libical/libical/blob/cfd401b9d043214395888de1d9daf52263e3245b/src/libical/icalrecur.c#L2882>

* FREQ=YEARLY, BYWEEKNO can't be combined with BYYEARDAY, BYMONTH or BYMONTHDAY

     ref: <https://github.com/libical/libical/blob/cfd401b9d043214395888de1d9daf52263e3245b/src/libical/icalrecur.c#L2928>
