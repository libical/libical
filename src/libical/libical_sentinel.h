/**
  SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
  SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
*/

#ifndef LIBICAL_SENTINEL_H
#define LIBICAL_SENTINEL_H

/* sentinel attribute macro */
#if defined(NO_SENTINEL_WARNINGS)
#define LIBICAL_SENTINEL
#else
#if !defined(LIBICAL_SENTINEL)
#if defined(__GNUC__) || defined(__clang__)
#define LIBICAL_SENTINEL __attribute__((sentinel))
#else
/* sentinel attributes are unknown to MSVC */
#define LIBICAL_SENTINEL
#endif
#endif
#endif

#endif
