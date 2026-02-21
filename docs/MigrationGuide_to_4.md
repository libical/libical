# Migrating to version 4

A guide to help developers port their code from libical v3.x to libical 4.0.

## CMake options

Some CMake option names have been removed or renamed (deprecated) to the LIBICAL namespace.

Please change your build scripts to use the new names before the next major release.

User-specific:

| Old Name                     | New Name                             |
|------------------------------|--------------------------------------|
| ICAL_ALLOW_EMPTY_PROPERTIES  | removed                              |
| ICAL_BUILD_DOCS              | LIBICAL_BUILD_DOCS                   |
| ICAL_ERRORS_ARE_FATAL        | removed                              |
| ICAL_GLIB                    | LIBICAL_GLIB                         |
| ICAL_GLIB_VAPI               | LIBICAL_GLIB_VAPI                    |
| ICAL_GLIB_BUILD_DOCS         | LIBICAL_GLIB_BUILD_DOCS              |
| USE_BUILTIN_TZDATA           | LIBICAL_ENABLE_BUILTIN_TZDATA        |
| USE_32BIT_TIME_T             | LIBICAL_ENABLE_MSVC_32BIT_TIME_T     |
| GOBJECT_INTROSPECTION        | LIBICAL_GOBJECT_INTROSPECTION        |
| WITH_CXX_BINDINGS            | LIBICAL_CXX_BINDINGS                 |
| ENABLE_LTO_BUILD             | CMAKE_INTERPROCEDURAL_OPTIMIZATION   |
| SHARED_ONLY                  | removed                              |
| STATIC_ONLY                  | LIBICAL_STATIC                       |

Developer-specific:

| Old Name                     | New Name                             |
|------------------------------|--------------------------------------|
| ABI_DUMPER                   | LIBICAL_DEVMODE_ABI_DUMPER           |
| ADDRESS_SANITIZER            | LIBICAL_DEVMODE_ADDRESS_SANITIZER    |
| LIBICAL_SYNCMODE_THREADLOCAL | LIBICAL_DEVMODE_SYNCMODE_THREADLOCAL |
| THREAD_SANITIZER             | LIBICAL_DEVMODE_THREAD_SANITIZER     |
| UNDEFINED_SANITIZER          | LIBICAL_DEVMODE_UNDEFINED_SANITIZER  |

## Conditional compilation

To continue supporting the 3.0 version you can use conditional compilation, like so:

```C
     #if ICAL_CHECK_VERSION(4,0,0)
     <...new code for the libical 4.0 version ...>
     #else
     <...old code for the libical 3.0 version ...>
     #endif
```

you can handle code that no longer exists in 4.0 with:

```C
     #if !ICAL_CHECK_VERSION(4,0,0)
     <...old code for the libical 3.0 version ...>
     #endif
```

## ICAL_ENABLE_ERRORS_ARE_FATAL

The `ICAL_ENABLE_ERRORS_ARE_FATAL` conditional compile macro and accompanying CMake option  `ICAL_ENABLE_ERRORS_ARE_FATAL`
are removed.

To abort whenever an error is encountered use the `icalerror_set_errors_are_fatal()` and `icalerror_get_errors_are_fatal()`
functions.

## ICAL_ALLOW_EMPTY_PROPERTIES

The `ICAL_ALLOW_EMPTY_PROPERTIES` conditional compile macro and accompanying CMake option `ICAL_ALLOW_EMPTY_PROPERTIES`
are removed.

To allow empty properties you can use the new runtime functions `icalproperty_set_allow_empty_properties()`
and `icalproperty_get_allow_empty_properties()`.

## PVL_USE_MACROS

The `PVL_USE_MACROS` conditional compile macro is removed.
The pvl unit always compiles the `pvl_data` function.

## ICAL_SETERROR_ISFUNC

The `ICAL_SETERROR_ISFUNC` conditional compile macro is removed.
The icalerror unit always compiles the `icalerror_set_errno` function.

## ICAL_BOOLEAN_TRUE and ICAL_BOOLEAN_FALSE

The `ICAL_BOOLEAN_TRUE` and `ICAL_BOOLEAN_FALSE` macros are removed.
The C library is C99 standards compliant and uses bool types.

## C library

### Modified functions

* `icalrecurrencetype_from_string()` was replaced by `icalrecurrencetype_new_from_string()`,
   which returns a `struct icalrecurrencetype *` rather than a `struct icalrecurrencetype`.
   Free the allocated memory using `icalrecurrencetype_unref()`.

* The following functions now take arguments of type `struct icalrecurrencetype *` rather than
  `struct icalrecurrencetype`:
  * `icalproperty_new_rrule()`
  * `icalproperty_get_rrule()`
  * `icalproperty_set_rrule()`
  * `icalproperty_vanew_rrule()`
  * `icalproperty_new_exrule()`
  * `icalproperty_set_exrule()`
  * `icalproperty_get_exrule()`
  * `icalproperty_vanew_exrule()`
  * `icalrecur_iterator_new()`
  * `icalvalue_new_recur()`
  * `icalvalue_set_recur()`
  * `icalvalue_get_recur()`

* The following functions now return a value of type `struct icalrecurrencetype *` rather than
  `struct icalrecurrencetype`:
  * `icalproperty_get_rrule()`
  * `icalproperty_get_exrule()`
  * `icalvalue_get_recur()`

* `icaltimezone_convert_time()` now populates the icaltimetype zone member on conversion;
  i.e. the timezone information is not lost during a conversion.

* To more clearly illustrate their intended purpose, the `icaldurationtype_from_int()` and `icaldurationtype_as_int()`
  have been renamed to `icaldurationtype_from_seconds()` and `icaldurationtype_as_seconds()`, respectively.
  The functionality of `icaldurationtype_from_seconds` has not changed. The functionality for
  `icaldurationtype_as_seconds` has changed, such that a duration with days or weeks fails
  with a ICAL_MALFORMEDDATA_ERROR and returns 0. To preserve the former logic of
  `icaldurationtype_as_int`, use he newly introduced `icaldurationtype_as_utc_seconds`.

* Similarly, the `icaltime_add()` and `icaltime_subtract()` functions are now called
  `icalduration_extend()` and `icalduration_from_times()`.  Their functionality has not changed.

* The `get_zone_directory()` and `set_zone_directory()` functions are have been renamed to
  `icaltimezone_get_zone_directory()` and `icaltimezone_set_zone_directory()`, respectively.

* The `icaltzutil_set_zone_directory()` and `icaltzutil_get_zone_directory()` functions are now called
  `icaltimezone_set_system_zone_directory()` and `icaltimezone_get_system_zone_directory()` respectively.

* In previous versions, the `icalvalue_compare()` function returned 0 if unknown or null value types
  were encountered; in this version, ICAL_XLICCOMPARETYPE_NONE is returned instead.

* In previous versions, the `icalcomponent_get_status()` returned 0 if a problem parsing the status
  property was detected; in this version, ICAL_STATUS_NONE is returned instead.

* The `ical_bt()` and `icalerrno_return()` functions have been renamed to
  `icalerror_backtrace()` and icalerror_icalerrno()` respectively for the sake
  of consistent function name-spacing.

### New functions

The following functions have been added:

* `icallimit_set()`
* `icallimit_get()`
* `icalarray_set_element_at()`
* `icalrecurrencetype_new()`
* `icalrecurrencetype_ref()`
* `icalrecurrencetype_unref()`
* `icalrecurrencetype_clone()`
* `icalrecurrencetype_encode_day()`
* `icalrecurrencetype_encode_month()`
* `icalcomponent_clone()`
* `icalproperty_clone()`
* `icalproperty_set_allow_empty_properties()`
* `icalproperty_get_allow_empty_properties()`
* `icalparameter_clone()`
* `icalparameter_kind_value_kind()`
* `icalparameter_is_multivalued()`
* `icalparameter_decode_value()`
* `icalvalue_clone()`
* `icalcluster_clone()`
* `icalrecur_iterator_prev()`
* `icalrecur_resize_by()`
* `icalrecurrencetype_new()`
* `icalrecurrencetype_ref()`
* `icalrecurrencetype_unref()`
* `icalrecurrencetype_clone()`
* `icalrecurrencetype_from_string()`
* `icalcomponent_set_x_name()`
* `icalcomponent_get_x_name()`
* `icalcomponent_get_component_name()`
* `icalcomponent_get_component_name_r()`
* `ical_set_invalid_rrule_handling_setting()`
* `ical_get_invalid_rrule_handling_setting()`
* `icalparser_get_ctrl()`
* `icalparser_set_ctrl()`
* `icaltimezone_tzid_prefix()`
* `icaltimezone_set_system_zone_directory()`
* `icalcompiter_is_valid()`
* `icalpropiter_is_valid()`
* and the new functions for the `icalstrarray` and `icalenumarray` data types

### Removed functions

* `icalmime_parse()` has been removed. Please use another library if you need a MIME parser.

* `icaltime_week_number()` has been removed. (it never properly accounted for the
   start day of the week in different locales).

* `icalrecurrencetype_clear()` has been removed.

* `icaltimezone_release_zone_tab()` has been removed.
   Use `icaltimezone_free_builtin_timezones()  instead.

* `icalrecurrencetype_rscale_is_supported()` has been removed as
   RSCALE=GREGORIAN is supported without libicu now.
   Replace `icalrecurrencetype_rscale_is_supported()` calls with a true condition.

* These deprecated functions have been removed:
  * `caldat()`
  * `juldat()`
  * `icalcomponent_new_clone()`
  * `icalparameter_new_clone()`
  * `icalproperty_new_clone()`
  * `icalvalue_new_clone()`
  * `icalcluster_new_clone()`

* No longer publicly visible functions:
  * `icaltzutil_fetch_timezone()`
  * `icalrecurrencetype_clear()`
  * `icalproperty_new_impl()`

### Removed macros

These convenience macros were added in version 3 to ease porting from older versions.
They have been removed in version 4 and should be replaced with their actual function
names as follows:

| Old Macro Name                       | Actual Function Name                   |
|--------------------------------------|----------------------------------------|
| icalenum_action_to_string            | icalproperty_action_to_string          |
| icalenum_class_to_string             | icalproperty_class_to_string           |
| icalenum_component_kind_to_string    | icalcomponent_kind_to_string           |
| icalenum_method_to_string            | icalproperty_method_to_string          |
| icalenum_participanttype_to_string   | icalproperty_participanttype_to_string |
| icalenum_property_kind_to_string     | icalproperty_kind_to_string            |
| icalenum_property_kind_to_value_kind | icalproperty_kind_to_value_kind        |
| icalenum_resourcetype_to_string      | icalproperty_resourcetype_to_string    |
| icalenum_status_to_string            | icalproperty_status_to_string          |
| icalenum_string_to_action            | icalproperty_string_to_action          |
| icalenum_string_to_class             | icalproperty_string_to_class           |
| icalenum_string_to_component_kind    | icalcomponent_string_to_kind           |
| icalenum_string_to_method            | icalproperty_string_to_method          |
| icalenum_string_to_participanttype   | icalproperty_string_to_participanttype |
| icalenum_string_to_property_kind     | icalproperty_string_to_kind            |
| icalenum_string_to_resourcetype      | icalproperty_string_to_resourcetype    |
| icalenum_string_to_status            | icalproperty_string_to_status          |
| icalenum_string_to_transp            | icalproperty_string_to_transp          |
| icalenum_string_to_value_kind        | icalvalue_string_to_kind               |
| icalenum_transp_to_string            | icalproperty_transp_to_string          |
| icalenum_value_kind_to_string        | icalvalue_kind_to_string               |

### Added data types

* These data types have been added:
  * icalstrarray - for manipulating an array of strings
  * icalenumarray_element - structure to hold a generic enum value
  * icalenumarray - for manipulating an array of enum elements

### Removed data types

* These data structures have been removed (as they were never used):
  * struct icaltimezonetype
  * struct icaltimezonephase

### Migrating from 3.0 to 4.0

### const pointers

Many function signatures have been changed to use const pointers.

### bool return values

A number of function signatures have been changed to use 'bool' rather than 'int' types.

This is implemented using the C99 standards compliant <stdbool.h> header.

### Clone functions

Replace all `ical*_new_clone()` function calls with `ical*_clone()` .
ie, use `icalcomponent_clone()` rather then `icalcomponent_new_clone()`.

### `icalrecurrencetype` now passed by reference

The way `struct icalrecurrencetype` is passed between functions has been changed. While it was
usually passed by value in 3.0, it is now passed by reference. A reference counting mechanism is
applied that takes care of de-allocating an instance as soon as the reference counter goes to 0.

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

### `icalgeotype` now uses character strings rather than doubles

The members of `struct icalgeotype` for latitude ('lat`) and longitude ('lon`) have been changed
to use ICAL_GEO_LEN long character strings rather than the double type.

This means that simple assignments in 3.0 must be replaced by string copies.

```C
     geo.lat = 0.0;
     geo.lon = 10.0;
```

becomes

```C
     strncpy(geo.lat, "0.0", ICAL_GEO_LEN-1);
     strncpy(geo.lon, "10.0", ICAL_GEO_LEN-1);
```

and

```C
    double lat = geo.lat;
    double lon = geo.lon;
```

becomes

```C
    double lat, lon;
    sscanf(geo.lat, "%lf", &lat);
    sscanf(geo.lon, "%lf", &lon);
```

## icaltime_adjust

The `icaltime_adjust` function no longer adjusts null icaltimetypes.

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

### Multi-valued parameters

Support for these multi-valued parameters is added in libical 4.0.

* DELEGATED-FROM (RFC 5545)
* DELEGATED-TO (RFC 5545)
* MEMBER (RFC 5545)
* DISPLAY (RFC 7986)
* FEATURE (RFC 7986)

You can access the 'nth' value for such parameters using the new "_nth" functions.

For example, to access the first delegated-to attendee use

```c
param = icalproperty_get_first_parameter(prop, ICAL_DELEGATEDTO_PARAMETER);
icalparameter_get_delegatedto_nth(param, 0)
```

### Setting the tzid

The `icaltimezone_set_tzid_prefix` function now allows setting an empty prefix.
In older libical versions, calling `icaltimezone_set_tzid_prefix` with an empty tzid prefix
would reset to the BUILTIN_TZID_PREFIX value (i.e. ""/freeassociation.sourceforge.net/").

The new publicly visible function `icaltimezone_tzid_prefix` returns the current tzid prefix string.

Note that the tzid prefix must be globally unique (such as a domain name owned by the developer
of the calling application), and begin and end with forward slashes. The tzid string must be
fewer than 256 characters long.

## C++ library

### Modified methods

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

### `icalrecurrencetype.by_xxx` static arrays replaced by dynamically allocated ones

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
required anymore.  The array is iterated by comparing the iterator to the `size` member value.

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

`i_cal_bt` and `i_cal_errno_return` are renamed to `i_calerror_backtrace` and
`i_cal_errno_return`, respectively.

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
