#ifndef LIBICALVCARD_EXPORT_H
#define LIBICALVCARD_EXPORT_H

#if !defined(S_SPLINT_S)

#ifdef LIBICALVCARD_STATIC_DEFINE
#define LIBICALVCARD_EXPORT
#define LIBICALVCARD_NO_EXPORT
#else
#if defined(_MSC_VER) || defined(__CYGWIN__)
#if defined(libical_ical_EXPORTS)
       /* We are building this library */
#define LIBICALVCARD_EXPORT __declspec(dllexport)
#else
       /* We are using this library */
#define LIBICALVCARD_EXPORT __declspec(dllimport)
#endif
#define LIBICALVCARD_NO_EXPORT
#else
#define LIBICALVCARD_EXPORT __attribute__((visibility("default")))
#define LIBICALVCARD_NO_EXPORT __attribute__((visibility("hidden")))
#endif
#endif

#endif

#endif
