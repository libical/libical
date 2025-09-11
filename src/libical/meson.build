# SPDX-FileCopyrightText: Corentin Noël <corentin.noel@collabora.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

generation_files = {
  'icalderivedproperty': [mk_derived_properties, [ical_properties_design, ical_value_types_design]],
  'icalderivedparameter': [mk_derived_parameters, [ical_parameters_design]],
  'icalderivedvalue': [mk_derived_values, [ical_value_types_design]],
}

generated_headers = []
generated_sources = []

foreach filename, design_files : generation_files
  generated_headers += configure_file(
    input: '@0@.h.in'.format(filename),
    output: '@0@.h'.format(filename),
    command: [perl_bin, '-I', scripts_dir, design_files[0], '-i', '@INPUT@', '-h', design_files[1]],
    capture: true,
  )
  generated_sources += configure_file(
    input: '@0@.c.in'.format(filename),
    output: '@0@.c'.format(filename),
    command: [perl_bin, '-I', scripts_dir, design_files[0], '-i', '@INPUT@', '-c', design_files[1]],
    capture: true,
  )
endforeach

generated_sources += configure_file(
  input: 'icalrestriction.c.in',
  output: 'icalrestriction.c',
  command: [perl_bin, '-I', scripts_dir, mk_restriction_table, '-i', '@INPUT@', ical_restrictions_design],
  capture: true,
)

icaltime_h = configure_file(input: 'icaltime.h.cmake', output: 'icaltime.h', configuration: config_data, format: 'cmake')

icalversion_h = configure_file(input: 'icalversion.h.cmake', output: 'icalversion.h', configuration: config_data)

libical_lib = library(
  'ical',
  generated_sources,
  generated_headers,
  'libical_ical_export.h',
  'icalarray.c',
  'icalarray.h',
  'icalattach.h',
  'icalattachimpl.h',
  'icalattach.c',
  'icalcomponent.c',
  'icalcomponent.h',
  'icalenumarray.c',
  'icalenumarray.h',
  'icalenums.c',
  'icalenums.h',
  'icalerror.c',
  'icalerror.h',
  'icalmemory.c',
  'icalmemory.h',
  'icalmime.c',
  'icalmime.h',
  'icalparameter.c',
  'icalparameter.h',
  'icalparameterimpl.h',
  'icalparser.c',
  'icalparser.h',
  'icalproperty.c',
  'icalproperty.h',
  'icalrecur.c',
  'icalrecur.h',
  'icalrestriction.h',
  'icalstrarray.c',
  'icalstrarray.h',
  'icaltime.c',
  icaltime_h,
  'icaltz-util.c',
  'icaltz-util.h',
  'icaltimezone.c',
  'icaltimezone.h',
  'icaltimezoneimpl.h',
  'icalduration.h',
  'icalduration.c',
  'icalperiod.h',
  'icalperiod.c',
  'icaltypes.c',
  'icaltypes.h',
  'icalvalue.c',
  'icalvalue.h',
  'icalvalueimpl.h',
  'pvl.c',
  'pvl.h',
  'sspm.c',
  'sspm.h',
  'qsort_gen.c',
  'qsort_gen.h',
  'icallangbind.h',
  'icallangbind.c',
  'icaldate_p.c',
  'icaldate_p.h',
  'byref.c',
  dependencies: [config_dep, thread_dep],
  soversion: lib_soversion_string,
)

libical_dep = declare_dependency(link_with: libical_lib, include_directories: include_directories('.'))

install_headers(
  'icalarray.h',
  'icalattach.h',
  'icalcomponent.h',
  generated_headers,
  'icaldate_p.h',
  'icalduration.h',
  'icalenumarray.h',
  'icalenums.h',
  'icalerror.h',
  'icallangbind.h',
  'icalmemory.h',
  'icalmime.h',
  'icalparameter.h',
  'icalparser.h',
  'icalperiod.h',
  'icalproperty.h',
  'icalrecur.h',
  'icalrestriction.h',
  'icalstrarray.h',
  icaltime_h,
  'icaltz-util.h',
  'icaltimezone.h',
  'icaltypes.h',
  'icalvalue.h',
  'libical_deprecated.h',
  'libical_sentinel.h',
  'libical_ical_export.h',
  'pvl.h',
  'sspm.h',
  subdir: 'libical',
)

pkg.generate(
  libical_lib,
  description: 'An implementation of basic iCAL protocols',
  filebase: 'libical',
  url: project_url,
)

if cxx_bindings_enabled
  libical_cxx_lib = library(
    'ical_cxx',
    'icalparameter_cxx.cpp',
    'icalparameter_cxx.h',
    'icalproperty_cxx.cpp',
    'icalproperty_cxx.h',
    'icalvalue_cxx.cpp',
    'icalvalue_cxx.h',
    'icptrholder_cxx.h',
    'vcomponent_cxx.cpp',
    'vcomponent_cxx.h',
    cpp_args: ['-DBUILD_LIBICALDLL'],
    dependencies: [libical_dep, config_dep, thread_dep],
    soversion: lib_soversion_string,
  )

  libical_cxx_dep = declare_dependency(
    link_with: libical_cxx_lib,
    include_directories: include_directories('.'),
    dependencies: [libical_dep],
  )

  install_headers(
    'icalparameter_cxx.h',
    'icalproperty_cxx.h',
    'icalvalue_cxx.h',
    'icptrholder_cxx.h',
    'vcomponent_cxx.h',
    subdir: 'libical',
  )
endif
