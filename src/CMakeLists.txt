# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

add_subdirectory(libical)
add_subdirectory(libicalss)
add_subdirectory(libicalvcal)
if(ICAL_GLIB)
  add_subdirectory(libical-glib)
endif()
add_subdirectory(libicalvcard)
if(LIBICAL_BUILD_TESTING)
  add_subdirectory(test)
endif()
