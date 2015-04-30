include(CheckIncludeFiles)
check_include_files(byteswap.h HAVE_BYTESWAP_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(endian.h HAVE_ENDIAN_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(pthread.h HAVE_PTHREAD_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(sys/endian.h HAVE_SYS_ENDIAN_H)
check_include_files(sys/param.h HAVE_SYS_PARAM_H)
check_include_files(sys/utsname.h HAVE_SYS_UTSNAME_H)
check_include_files(time.h HAVE_TIME_H)
check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(wctype.h HAVE_WCTYPE_H)

include(CheckFunctionExists)
check_function_exists(access HAVE_ACCESS) #Unix <unistd.h>
check_function_exists(_access HAVE__ACCESS) #Windows <io.h>
check_function_exists(backtrace HAVE_BACKTRACE)
check_function_exists(fork HAVE_FORK)
check_function_exists(getpwent HAVE_GETPWENT)
check_function_exists(gmtime_r HAVE_GMTIME_R)
check_function_exists(isspace HAVE_ISSPACE)
check_function_exists(iswspace HAVE_ISWSPACE)
check_function_exists(open HAVE_OPEN) #Unix <fcntl.h>
check_function_exists(_open HAVE__OPEN) #Windows <io.h>
check_function_exists(setenv HAVE_SETENV)
check_function_exists(snprintf HAVE_SNPRINTF) #Unix <stdio.h>
check_function_exists(_snprintf HAVE__SNPRINTF) #Windows <stdio.h>
check_function_exists(stat HAVE_STAT)
check_function_exists(_stat HAVE__STAT)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strncasecmp HAVE_STRNCASECMP)
check_function_exists(stricmp HAVE_STRICMP)
check_function_exists(strnicmp HAVE_STRNICMP)
check_function_exists(strdup HAVE_STRDUP) #Unix <string.h>
check_function_exists(_strdup HAVE__STRDUP) #Windows <string.h>
check_function_exists(unsetenv HAVE_UNSETENV)
check_function_exists(unlink HAVE_UNLINK)
check_function_exists(waitpid HAVE_WAITPID)

include(CheckTypeSize)
check_type_size(mode_t SIZEOF_MODE_T)
check_type_size(size_t SIZEOF_SIZE_T)
check_type_size(intptr_t INTPTR_T) # please do not rename HAVE_INTPTR_T will automatically be defined
check_type_size(pid_t HAVE_PID_T)
check_type_size(time_t SIZEOF_TIME_T)

include(FindThreads)
check_library_exists(pthread pthread_attr_get_np "" HAVE_PTHREAD_ATTR_GET_NP)
check_library_exists(pthread pthread_getattr_np "" HAVE_PTHREAD_GETATTR_NP)
check_library_exists(pthread pthread_create "" HAVE_PTHREAD_CREATE)
check_include_files("pthread.h;pthread_np.h" HAVE_PTHREAD_NP_H)
