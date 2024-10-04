# Migrating to version 4.0

## `icalrecurrencetype` now passed by reference

The way `struct icalrecurrencetype` is passed between functions has been changed. While it was
usually passed by value in 3.0, it is now passed by reference. A reference counting mechanism is
applied that takes care of deallocating an instance as soon as the reference counter goes to 0.

### libical C library

#### Modified functions

* `icalrecurrencetype_from_string()` was replaced by `icalrecurrencetype_new_from_string()`,
   which returns a `struct icalrecurrencetype *` rather than a `struct icalrecurrencetype`.
* The following functions now take arguments of type `struct icalrecurrencetype *` rather than
  `struct icalrecurrencetype`:
  * `icalrecur_iterator_new()`
  * `icalvalue_new_recur()`
  * `icalvalue_set_recur()`

* The following functions now returns a value of type `struct icalrecurrencetype *` rather than
  `struct icalrecurrencetype`:
  * `icalvalue_get_recur()`

#### New functions

The following functions have been added:

* `icalrecurrencetype_new()`
* `icalrecurrencetype_ref()`
* `icalrecurrencetype_unref()`
* `icalrecurrencetype_clone()`

#### Removed functions

* `icalrecurrencetype_clear()` has been removed.

#### Migrating from 3.0 to 4.0

Instead of allocating `icalrecurrencetype` on the stack, use one of the constructor functions and
take care of deallocation.

Code like this in libical 3.0:

```C
    struct icalrecurrencetype recur;

    icalrecurrencetype_clear(&recur);

    // Work with the object
```

changes to this in libical 4.0:

```C
    struct icalrecurrencetype *recur;

    // allocate
    recur = icalrecurrencetype_new();
    if (recur) {

        // Work with the object

        // deallocate
        icalrecurrencetype_unref(recur);
    } else {
        // out of memory error handling
    }
```

### Working with `icalvalue` and `icalproperty`

Code like this in libical 3.0:

```C
    icalvalue *recur_value = ...;
    struct icalrecurrencetype recur = icalvalue_get_recur(recur_value);

    // Work with the object
```

changes to this in libical 4.0:

```C
    icalvalue *recur_value = ...;
    struct icalrecurrencetype *recur = icalvalue_get_recur(recur_value);

    // Work with the object
    // No need to unref
```

### C++ library

#### Modified methods

* The following methods now take arguments of type `struct icalrecurrencetype *` rather than `const
  struct icalrecurrencetype &`:
  * `ICalValue.set_recur()`
  * `ICalProperty.set_exrule()`
  * `ICalProperty.set_rrule()`

* The following methods now returns a value of type `struct icalrecurrencetype *` rather than
  `struct icalrecurrencetype`:
  * `ICalValue.get_recur()`
  * `ICalProperty.get_exrule()`
  * `ICalProperty.get_rrule()`

## `icalrecurrencetype.by_xxx` static arrays replaced by dynamically allocated ones

I.e. memory `short by_hour[ICAL_BY_DAY_SIZE]` etc. are replaced by

```c
typedef struct
{
  short *data;
  short size;
} icalrecurrence_by_data;

struct icalrecurrencetype {
  ...
  icalrecurrence_by_data by[ICAL_BY_NUM_PARTS];
}
```

Memory is allocated in the required size using the new `icalrecur_resize_by()` function. It is
automatically freed together with the containing `icalrecurrencetype`. As the size of the array is
stored explicitly, no termination of the array with special value `ICAL_RECURRENCE_ARRAY_MAX` is
required anymore.

### Migrating `icalrecurrencetype.by_xxx` static arrays usage from 3.0 to 4.0

Code like this in libical 3.0:

```C
    icalrecurrencetype recur;
    ...
    recur.by_hour[0] = 12;
    recur.by_hour[1] = ICAL_RECURRENCE_ARRAY_MAX;
```

changes to something like this in libical 4.0:

```C
    icalrecurrencetype *recur;
    ...
    if (!icalrecur_resize_by(&recur->by[ICAL_BY_HOUR], 1)) {
      // allocation failed
      // error handling
    } else {
      recur.by[ICAL_BY_HOUR].data[0] = 12;
    }
```

## GLib/Python bindings - changed `ICalGLib.Recurrence.*_by_*` methods

`i_cal_recurrence_*_by_xxx*` methods have been replaced by more generic versions that take the 'by'
type (day, month, ...) as a parameter.

### Migrating `ICalGLib.Recurrence.*_by_*` methods from 3.0 to 4.0

Code like this in libical 3.0:

```python
recurrence.set_by_second(0,
  recurrence.get_by_second(0) + 1)
```

changes to something like this in libical 4.0:

```python
recurrence.set_by(ICalGLib.RecurrenceByRule.BY_SECOND, 0,
  recurrence.get_by(ICalGLib.RecurrenceByRule.BY_SECOND, 0) + 1)
```
