#ifndef LIBICAL_ICAL_EXPORT_H
#define LIBICAL_ICAL_EXPORT_H

#ifdef _MSC_VER
  #if defined(BUILD_LIBICAL_STATIC)
    #define LIBICAL_ICAL_EXPORT
  #elif defined(BUILD_LIBICAL_SHARED)
#pragma message("HERE")
    #define LIBICAL_ICAL_EXPORT __declspec(dllexport)
  #else
    #define LIBICAL_ICAL_EXPORT __declspec(dllimport)
  #endif
#else
  #if defined(BUILD_LIBICAL_STATIC)
    #define LIBICAL_ICAL_EXPORT
  #else
    #define LIBICAL_ICAL_EXPORT __attribute__((visibility("default")))
  #endif
#endif

#endif
