gtk-doc comment:
```regex
(<comment [^/]*>[\w .]+[^.])(</comment>)
```

Doxygen
```regex
(/\*\* .*[a-z]) (\*/)
```

Doxygen over multiple lines

Doxygen case correction
```regex
([ \r\n/\*]\* +)([a-z])
```
## TODO
@brief
(done) it's
(done) Create -> creates
(done) Return -> returns
(done) Set -> sets
(done) Get -> gets
(done) Free -> frees
Period at end of params? Briefs? Returns?
Replace %NULL in gtk-doc
icalparameter.c Weird format
an [not vowel]
a [vowel]

## DONE
- Java
    - icalparameter.java
    - ICalDurationType.java
    - ICalProperty.java
    - ICalTimeType.java
    - ICalTriggerType.java
    - ICalValue.java
    - VComponent.java
    - ICalRecurrenceType.java
- C
    - astime.h
    - caldate.c
    - icalarray.h
    - icalattach.h
    - icalcomponent.c (period at end of brief?)
    - icalcomponent.h (same)
    - icalduration.h
    - icalderivedvalue.h.in
    - icalenums.c
    - icalerror.c
    - icalerror.h
    - icallangbind.c/h
    - icalmemory.c/h
    - icalmime.c/h
    - icalparameter_cxx
    - icalparameter.c (mostly)
    - icalparameter.h
    - icalparameterimpl.h
    - icalparser.c/h
    - icalperiod.c/h
    - icalproperty.c/h
    - icalrecur.c

## Inconsistencies
- icalparser:get_line