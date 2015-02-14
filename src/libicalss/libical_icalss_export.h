#ifndef LIBICAL_ICALSS_EXPORT_H
#define LIBICAL_ICALSS_EXPORT_H

#ifdef _MSC_VER
  #if defined(BUILD_LIBICAL_STATIC)
    #define LIBICAL_ICALSS_EXPORT extern
  #elif defined(BUILD_LIBICAL_SHARED)
    #define LIBICAL_ICALSS_EXPORT __declspec(dllexport)
  #else
    #define LIBICAL_ICALSS_EXPORT __declspec(dllimport)
  #endif
#else
  #if defined(BUILD_LIBICAL_STATIC)
    #define LIBICAL_ICALSS_EXPORT
  #else
    #define LIBICAL_ICALSS_EXPORT __attribute__((visibility("default")))
  #endif
#endif

#endif
