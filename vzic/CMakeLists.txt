# This is the top-level CMakeLists.txt file for the vzic project.
#
# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
#

# Uses the standalone version if STANDALONE is specified, else uses the integrated version.
# Stand-alone is available for people who want to build vzic but don't need to build all of libical.
#  For example: cmake -DSTANDALONE=True (you'll need the glib and pkgconfig development packages)

# Pass the following variables to cmake to control the build:
# (See docs/developer/HOWTO-update-zoneinfo.md for more information)
#
# -DPRODUCT_ID=[propid]
#  This variable is used as the PRODID property on the iCalendar files output.
#  It identifies the product which created the iCalendar objects.
#  So you need to substitute your own organization name and product.
#  Default=-//citadel.org//NONSGML Citadel calendar//EN
#
# -DTZID_PREFIX=[tzid_prefix]
#  This variable is used to create unique IDs for each VTIMEZONE component.
#  The prefix is put before each timezone city name. It should start and end
#  with a '/'. The first part, i.e. 'myorganization.org' below, should be
#  a unique vendor ID, e.g. use a hostname. The part after that can be
#  anything you want. We use a date and version number for libical. The %D
#  gets expanded to today's date. There is also a vzic-merge.pl which can be
#  used to merge changes into a master set of VTIMEZONEs. If a VTIMEZONE has
#  changed, it bumps the version number on the end of this prefix. */
#  Default=/citadel.org/%D_1/
#
# -DCREATE_SYMLINK=[true|false]
#  This option is used to indicate how timezone aliases (indicated by a Link line
#  in Olson files) should be generated: The default is to symbolically link
#  the Link zone file to its authoritative zone. Alternatively, if set to False,
#  a VTIMEZONE file is generated for each Link.
#  Default=False
#
# -DIGNORE_TOP_LEVEL_LINK=[true|false]
#  This option indicates if top-level timezone aliases (a timezone name without
#  any '/' such as "EST5EDT") should be ignored. If False, a VTIMEZONE is
#  generated also for top-level aliases. This option only has
#  an effect if CREATE_SYMLINK is False, and mainly is useful for backward
#  compatibility with previous vzic versions.
#  Default=False
#

if(DEFINED STANDALONE)
  cmake_minimum_required(VERSION 3.20.0)
  project(vzic LANGUAGES C)
  include(FeatureSummary)
  get_filename_component(LIBICAL_CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
  list(APPEND CMAKE_MODULE_PATH "${LIBICAL_CMAKE_MODULE_PATH}/cmake/modules")

  find_package(PkgConfig QUIET)
  set(MIN_GLIB "2.44")
  find_package(GLib ${MIN_GLIB} REQUIRED)
else()
  set(STANDALONE False)
endif()

message(STATUS "Building vzic stand-alone ${STANDALONE}")
include(CMakeLists-integrated.txt)
