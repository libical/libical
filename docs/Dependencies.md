# Dependencies

## Required

- CMake version 3.20 or above
- A fully C99 standards complaint C compiler
- Perl

## Highly Recommended

For the C++ bindings a fully C++11 standards compliant C++ compiler

For RSCALE support the libicu development package is needed.

## GLib Bindings

- pkgconfig
- gobject-introspection (-devel)
- libxml2 (-devel)

and to build the associated GLib bindings documentation you'll need;

- gi-docgen

in addition, to build the Vala support you'll also need:

- vala (valac, the Vala compiler)

## Optional dependencies

- berkeleydb (-devel) for Berkeley DB storage support in libicalss
