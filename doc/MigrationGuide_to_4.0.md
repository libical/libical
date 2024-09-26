# Migrating to version 4.0

## `icalrecurrencetype` now passed by reference
The way `struct icalrecurrencetype` is passed between functions has been changed. While it was usually passed by value in 3.0, it is now passed by reference. A reference counting mechanism is
applied that takes care of deallocating an instance as soon as the reference counter goes to 0.

### libical C library
#### Modified functions
* `icalrecurrencetype_from_string()` was replaced by `icalrecurrencetype_new_from_string()`, which returns a `struct icalrecurrencetype *` rather than a `struct icalrecurrencetype`.
* The following functions now take arguments of type `struct icalrecurrencetype *` rather than `struct icalrecurrencetype`:
  * `icalrecur_iterator_new()`
  * `icalvalue_new_recur()`
  * `icalvalue_set_recur()`

* The following functions now returns a value of type `struct icalrecurrencetype *` rather than `struct icalrecurrencetype`:
  * `icalvalue_get_recur()`

#### New functions
The following functions have been added:
* `icalrecurrencetype_new()`
* `icalrecurrencetype_ref()`
* `icalrecurrencetype_unref()`
* `icalrecurrencetype_clone()`

#### Removed functions
* `icalrecurrencetype_clear()` has been removed.

### Migrating from 3.0 to 4.0

Instead of allocating `icalrecurrencetype` on the stack, use one of the constructor functions and take care of deallocation.

#### Objects allocated on the stack
**libical 3.0**
```C
struct icalrecurrencetype recur;

icalrecurrencetype_clear(&recur);

// Work with the object
```

**libical 4.0**
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
**libical 3.0**

```C
icalvalue *recur_value = ...;
struct icalrecurrencetype recur = icalvalue_get_recur(recur_value);

// Work with the object
```

**libical 4.0**

```C
icalvalue *recur_value = ...;
struct icalrecurrencetype *recur = icalvalue_get_recur(recur_value);

// Work with the object
// No need to unref
```

### C++ library
#### Modified methods
* The following methods now take arguments of type `struct icalrecurrencetype *` rather than `const struct icalrecurrencetype &`:
  * `ICalValue.set_recur()`
  * `ICalProperty.set_exrule()`
  * `ICalProperty.set_rrule()`

* The following methods now returns a value of type `struct icalrecurrencetype *` rather than `struct icalrecurrencetype`:
  * `ICalValue.get_recur()`
  * `ICalProperty.get_exrule()`
  * `ICalProperty.get_rrule()`
