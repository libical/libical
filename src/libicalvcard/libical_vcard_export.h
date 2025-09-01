/**
  SPDX-FileCopyrightText: 2022, Fastmail Pty. Ltd. (https://fastmail.com)
  SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
*/

#ifndef LIBICAL_VCARD_EXPORT_H
#define LIBICAL_VCARD_EXPORT_H

#if !defined(S_SPLINT_S)

#ifdef LIBICAL_VCARD_STATIC_DEFINE
#define LIBICAL_VCARD_EXPORT
#define LIBICAL_VCARD_NO_EXPORT
#else
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__)
#if defined(libical_vcard_EXPORTS)
/* We are building this library */
#define LIBICAL_VCARD_EXPORT __declspec(dllexport)
#else
/* We are using this library */
#define LIBICAL_VCARD_EXPORT __declspec(dllimport)
#endif
#define LIBICAL_VCARD_NO_EXPORT
#else
#define LIBICAL_VCARD_EXPORT    __attribute__((visibility("default")))
#define LIBICAL_VCARD_NO_EXPORT __attribute__((visibility("hidden")))
#endif
#endif

#endif

#endif
