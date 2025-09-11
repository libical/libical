# SPDX-FileCopyrightText: Corentin Noël <corentin.noel@collabora.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
project(
  'libical',
  'c',
  version: '3.99.99',
  license: 'MPL-2.0 OR LGPL-2.1-only',
  license_files: 'LICENSE.txt',
  meson_version: '>=1.1',
)

lib_major_soversion = '4'
lib_soversion_string = '@0@.0'.format(lib_major_soversion)
project_version = '4.0'
project_url = 'https://libical.github.io/libical/'

languages = ['c']
cxx_bindings_enabled = add_languages('cpp', native: false, required: get_option('cxx_bindings'))
if cxx_bindings_enabled
  languages += 'cpp'
endif

pkg = import('pkgconfig')
cc = meson.get_compiler('c')

icu_uc_dep = dependency('icu-uc', required: false)
icu_i18n_dep = dependency('icu-i18n', required: false)
if (icu_uc_dep.found())
  icu_binary_dir = icu_uc_dep.get_variable('exec_prefix', default_value: get_option('prefix')) / 'sbin'
endif


# Config checks
config_data = configuration_data()
config_data.set('CMAKE_PROJECT_HOMEPAGE_URL', project_url)
config_data.set('CMAKE_PROJECT_NAME', meson.project_name())
config_data.set('PROJECT_VERSION', project_version)
config_data.set('CMAKE_PROJECT_VERSION_MAJOR', meson.project_version().split('.')[0])
config_data.set('CMAKE_PROJECT_VERSION_MINOR', meson.project_version().split('.')[1])
config_data.set('CMAKE_PROJECT_VERSION_PATCH', meson.project_version().split('.')[2])

config_data.set10('HAVE_BYTESWAP_H', cc.has_header('byteswap.h'))
config_data.set10('HAVE_DIRENT_H', cc.has_header('dirent.h'))
config_data.set10('HAVE_ENDIAN_H', cc.has_header('endian.h'))
config_data.set10('HAVE_INTTYPES_H', cc.has_header('inttypes.h'))
config_data.set10('HAVE_PTHREAD_H', cc.has_header('pthread.h'))
config_data.set10('HAVE_SYS_ENDIAN_H', cc.has_header('sys/endian.h'))
config_data.set10('HAVE_SYS_PARAM_H', cc.has_header('sys/param.h'))
config_data.set10('HAVE_SYS_UTSNAME_H', cc.has_header('sys/utsname.h'))
config_data.set10('HAVE_FCNTL_H', cc.has_header('fcntl.h'))
config_data.set10('HAVE_UNISTD_H', cc.has_header('unistd.h'))
config_data.set10('HAVE_WCTYPE_H', cc.has_header('wctype.h'))
config_data.set10('HAVE_STDBOOL_H', cc.has_header('stdbool.h'))

if target_machine.system() == 'windows' and cc.get_id() == 'msvc'
  config_data.set10('HAVE__ACCESS', cc.has_function('_access', prefix: '#include <io.h>'))
  config_data.set10('HAVE__GETPID', cc.has_function('_getpid', prefix: '#include <process.h>'))
  config_data.set10('HAVE__MKDIR', cc.has_function('_mkdir', prefix: '#include <direct.h>'))
  config_data.set10('HAVE__OPEN', cc.has_function('_open', prefix: '#include <io.h>'))
  config_data.set10('HAVE__SNPRINTF', cc.has_function('_snprintf', prefix: '#include <stdio.h>'))
  config_data.set10('HAVE__STAT', cc.has_function('_stat', prefix: '#include <sys/types.h>\n#include <sys/stat.h>'))
  config_data.set10(
    'HAVE__FSTAT64',
    cc.has_function('_fstat64', prefix: '#include <sys/types.h>\n#include <sys/stat.h>'),
  )
  config_data.set10('HAVE__STRDUP', cc.has_function('_strdup', prefix: '#include <string.h>'))
  config_data.set10('HAVE__STRICMP', cc.has_function('_stricmp', prefix: '#include <string.h>'))
  config_data.set10('HAVE__STRNICMP', cc.has_function('_strnicmp', prefix: '#include <string.h>'))
  config_data.set10('HAVE__READ', cc.has_function('_read', prefix: '#include <io.h>'))
  config_data.set10('HAVE__WRITE', cc.has_function('_write', prefix: '#include <io.h>'))
else
  config_data.set10('HAVE_ACCESS', cc.has_function('access', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_FORK', cc.has_function('fork', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_GETOPT', cc.has_function('getopt', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_GETPID', cc.has_function('getpid', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_GETPWENT', cc.has_function('getpwent', prefix: '#include <sys/types.h>\n#include <pwd.h>'))
  config_data.set10('HAVE_GMTIME_R', cc.has_function('gmtime_r', prefix: '#include <time.h>'))
  config_data.set10('HAVE_LOCALTIME_R', cc.has_function('localtime_r', prefix: '#include <time.h>'))
  config_data.set10('HAVE_MKDIR', cc.has_function('mkdir', prefix: '#include <sys/stat.h>\n#include <sys/types.h>'))
  config_data.set10(
    'HAVE_OPEN',
    cc.has_function('open', prefix: '#include <sys/stat.h>\n#include <sys/types.h>\n#include <fcntl.h>'),
  )
  config_data.set10('HAVE_NANOSLEEP', cc.has_function('nanosleep', prefix: '#include <time.h>'))
  config_data.set10('HAVE_SIGNAL', cc.has_function('signal', prefix: '#include <signal.h>'))
  config_data.set10(
    'HAVE_STAT',
    cc.has_function('stat', prefix: '#include <sys/stat.h>\n#include <sys/types.h>\n#include <unistd.h>'),
  )
  config_data.set10(
    'HAVE_FSTAT',
    cc.has_function('fstat', prefix: '#include <sys/stat.h>\n#include <sys/types.h>\n#include <unistd.h>'),
  )
  config_data.set10('HAVE_STRDUP', cc.has_function('strdup', prefix: '#include <string.h>'))
  config_data.set10('HAVE_STRCASECMP', cc.has_function('strcasecmp', prefix: '#include <strings.h>'))
  config_data.set10('HAVE_STRNCASECMP', cc.has_function('strncasecmp', prefix: '#include <strings.h>'))
  config_data.set10('HAVE_READ', cc.has_function('read', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_UNLINK', cc.has_function('unlink', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_USLEEP', cc.has_function('usleep', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_WAITPID', cc.has_function('waitpid', prefix: '#include <sys/types.h>\n#include <sys/wait.h>'))
  config_data.set10('HAVE_WRITE', cc.has_function('write', prefix: '#include <unistd.h>'))
  config_data.set10('HAVE_ALARM', cc.has_function('alarm', prefix: '#include <unistd.h>'))
endif

config_data.set10('HAVE_BACKTRACE', cc.has_function('backtrace', prefix: '#include <execinfo.h>'))
config_data.set10('HAVE_ISWSPACE', cc.has_function('iswspace', prefix: '#include <wctype.h>'))
config_data.set10('HAVE_SETENV', cc.has_function('setenv', prefix: '#include <stdlib.h>'))
config_data.set10('HAVE_UNSETENV', cc.has_function('unsetenv', prefix: '#include <stdlib.h>'))
config_data.set10('HAVE_SNPRINTF', cc.has_function('snprintf', prefix: '#include <stdio.h>'))

types = {
  'intptr_t': '#include <stdint.h>',
  'pid_t': '#include <sys/types.h>',
  'size_t': '#include <stddef.h>',
  'ssize_t': '#include <sys/types.h>',
  'time_t': '#include <time.h>',
  '__time64_t': '#include <time.h>',
  'wint_t': '#include <wctype.h>',
}

foreach type, prefix : types
  type_size = cc.sizeof(type, prefix: prefix)
  if type_size > 0
    upper_type = type.to_upper()
    config_data.set10('HAVE_SIZEOF_@0@'.format(upper_type), true)
    config_data.set('SIZEOF_@0@'.format(upper_type), type_size)
  endif
endforeach

if get_option('64bit_icaltime_t')
  if config_data.has('HAS_SIZEOF___TIME64_T')
    config_data.set('ICAL_ICALTIME_T_TYPE', '__time64_t')
  else
    error(
      'Option LIBICAL_ENABLE_64BIT_ICALTIME_T is not supported for this compiler or architecture since the __time64_t type is not available.',
    )
  endif
else
  if config_data.get('SIZEOF_TIME_T') == 4
    warning(
      'The 32-bit time_t type has been detected. Consider reconfiguring with -D64bit_icaltime_t=true to enable 64-bit time_t types.',
    )
  endif
  config_data.set('ICAL_ICALTIME_T_TYPE', 'time_t')
endif

thread_dep = dependency('threads')
config_data.set10(
  'HAVE_PTHREAD_ATTR_GET_NP',
  cc.has_function('pthread_attr_get_np', dependencies: thread_dep, prefix: '#define _GNU_SOURCE\n#include <pthread.h>'),
)
config_data.set10(
  'HAVE_PTHREAD_GETATTR_NP',
  cc.has_function('pthread_getattr_np', dependencies: thread_dep, prefix: '#define _GNU_SOURCE\n#include <pthread.h>'),
)
config_data.set10(
  'HAVE_PTHREAD_CREATE',
  cc.has_function('pthread_create', dependencies: thread_dep, prefix: '#include <pthread.h>'),
)
config_data.set10('HAVE_PTHREAD_H', cc.has_header('pthread_np.h', dependencies: thread_dep))

config_data.set10('ICAL_ALLOW_EMPTY_PROPERTIES', get_option('allow_empty_properties'))
config_data.set10('ICAL_ERRORS_ARE_FATAL', get_option('errors_are_fatal'))

config_h = configure_file(input: 'config.h.cmake', output: 'config.h', format: 'cmake', configuration: config_data)

config_dep = declare_dependency(
  include_directories: include_directories('.'),
  compile_args: '-DHAVE_CONFIG_H=1',
  sources: config_h,
)

perl_bin = find_program('perl', native: true)

add_project_arguments(
  '-DICAL_ALLOW_EMPTY_PROPERTIES=@0@'.format(get_option('errors_are_fatal').to_int()),
  language: languages,
)
add_project_arguments('-DICAL_ERRORS_ARE_FATAL=@0@'.format(get_option('errors_are_fatal').to_int()), language: languages)

package_data_dir = get_option('prefix') / get_option('datadir') / 'libical'
add_project_arguments('-DPACKAGE_DATA_DIR="@0@"'.format(package_data_dir), language: languages)

subdir('design-data')
subdir('scripts')
subdir('src')
