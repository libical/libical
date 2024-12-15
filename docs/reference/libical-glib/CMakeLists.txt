# SPDX-FileCopyrightText: Milan Crha <mcrha@redhat.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

configure_file(libical-glib.toml.in libical-glib.toml @ONLY)
generate_gi_documentation(
  libical-glib
  ${CMAKE_CURRENT_BINARY_DIR}/libical-glib.toml
  "${CMAKE_BINARY_DIR}/src/libical-glib/ICalGLib-${LIBICAL_LIB_MAJOR_VERSION}.0.gir"
)
