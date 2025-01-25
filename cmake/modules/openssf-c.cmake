# from https://www.stevenengelhardt.com/2024/11/12/cmake-implementation-of-openssf-compiler-hardening-options/ "which I’m sharing for public use"

# SPDX-FileCopyrightText: 2024 Steven Engelhardt <https://www.stevenengelhardt.com>
# SPDX-License-Identifier: Unlicense

# openssf.cmake: Implement OpenSSF compiler-hardening options from
# https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++
#
# Only works on clang/GNU-based compilers (GCC, Clang, etc.) because OpenSSF
# does not provide compiler-hardening options for MSVC.

include(CheckCompilerFlag)
include(CheckLinkerFlag)

# Table 1: Recommended compiler options that enable strictly compile-time checks.
check_compiler_flag(C "-Wall" C_SUPPORTS_WALL)
if(C_SUPPORTS_WALL)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()
check_compiler_flag(C "-Wextra" C_SUPPORTS_WEXTRA)
if(C_SUPPORTS_WEXTRA)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
endif()
check_compiler_flag(C "-Wformat" C_SUPPORTS_WFORMAT)
if(C_SUPPORTS_WFORMAT)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat")
endif()
check_compiler_flag(C "-Wformat=2" C_SUPPORTS_WFORMAT2)
if(C_SUPPORTS_WFORMAT2)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat=2")
endif()
#check_compiler_flag(C "-Wconversion" C_SUPPORTS_WCONVERSION)
#if(C_SUPPORTS_WCONVERSION)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")
#endif()
check_compiler_flag(C "-Wsign-conversion" C_SUPPORTS_WSIGNCONVERSION)
if(C_SUPPORTS_WSIGNCONVERSION)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsign-conversion")
endif()
check_compiler_flag(C "-Wtrampolines" C_SUPPORTS_WTRAMPOLINES)
if(C_SUPPORTS_WTRAMPOLINES)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wtrampolines")
endif()
check_compiler_flag(C "-Wimplicit-fallthrough" C_SUPPORTS_WIMPLICIT_FALLTHROUGH)
if(C_SUPPORTS_WIMPLICIT_FALLTHROUGH)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wimplicit-fallthrough")
endif()
check_compiler_flag(C "-Wbidi-chars=any" C_SUPPORTS_WBIDI_CHARS_ANY)
if(C_SUPPORTS_WBIDI_CHARS_ANY)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wbidi-chars=any")
endif()
check_compiler_flag(C "-Werror=format-security" C_SUPPORTS_WERROR_FORMAT_SECURITY)
if(C_SUPPORTS_WERROR_FORMAT_SECURITY)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=format-security")
endif()
check_compiler_flag(C "-Werror=implicit" C_SUPPORTS_WERROR_IMPLICIT)
if(C_SUPPORTS_WERROR_IMPLICIT)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit")
endif()
check_compiler_flag(C "-Werror=incompatible-pointer-types" C_SUPPORTS_WERROR_INCOMPATIBLE_POINTER_TYPES)
if(C_SUPPORTS_WERROR_INCOMPATIBLE_POINTER_TYPES)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=incompatible-pointer-types")
endif()
check_compiler_flag(C "-Werror=int-conversion" C_SUPPORTS_WERROR_INT_CONVERSION)
if(C_SUPPORTS_WERROR_INT_CONVERSION)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=int-conversion")
endif()
# Note that the below should only be enabled during development, not source distribution
#check_compiler_flag(C "-Werror" C_SUPPORTS_WERROR)
#if(C_SUPPORTS_WERROR)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
#endif()

# Table 2: Recommended compiler options that enable run-time protection mechanisms.
check_compiler_flag(C "-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3" C_SUPPORTS_FORTIFY_SOURCE)
if(C_SUPPORTS_FORTIFY_SOURCE)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3")
endif()
check_compiler_flag(C "-D_GLIBC_ASSERTIONS" C_SUPPORTS_GLIBC_ASSERTIONS)
if(C_SUPPORTS_GLIBC_ASSERTIONS)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GLIBC_ASSERTIONS")
endif()
check_compiler_flag(C "-fstrict-flex-arrays=3" C_SUPPORTS_FSTRICT_FLEX_ARRAYS3)
if(C_SUPPORTS_FSTRICT_FLEX_ARRAYS3)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-flex-arrays=3")
endif()
check_compiler_flag(C "-fstack-clash-protection" C_SUPPORTS_FSTACK_CLASH_PROTECTION)
if(C_SUPPORTS_FSTACK_CLASH_PROTECTION)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-clash-protection")
endif()
check_compiler_flag(C "-fstack-protector-strong" C_SUPPORTS_FSTACK_PROTECTOR_STRONG)
if(C_SUPPORTS_FSTACK_PROTECTOR_STRONG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-strong")
endif()
check_compiler_flag(C "-fcf-protection=full" C_SUPPORTS_FCF_PROTECTION_FULL)
if(C_SUPPORTS_FCF_PROTECTION_FULL)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcf-protection=full")
endif()
check_compiler_flag(C "-mbranch-protection=standard" C_SUPPORTS_MBRANCH_PROTECTION_STANDARD)
if(C_SUPPORTS_MBRANCH_PROTECTION_STANDARD)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mbranch-protection=standard")
endif()
check_compiler_flag(C "-fno-delete-null-pointer-checks" C_SUPPORTS_FNO_DELETE_NULL_POINTER_CHECKS)
if(C_SUPPORTS_FNO_DELETE_NULL_POINTER_CHECKS)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-delete-null-pointer-checks")
endif()
check_compiler_flag(C "-fno-strict-overflow" C_SUPPORTS_FNO_STRICT_OVERFLOW)
if(C_SUPPORTS_FNO_STRICT_OVERFLOW)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-overflow")
endif()
check_compiler_flag(C "-fno-strict-aliasing" C_SUPPORTS_FNO_STRICT_ALIASING)
if(C_SUPPORTS_FNO_STRICT_ALIASING)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
endif()
check_compiler_flag(C "-ftrivial-auto-var-init" C_SUPPORTS_FTRIVIAL_AUTO_VAR_INIT)
if(C_SUPPORTS_FTRIVIAL_AUTO_VAR_INIT)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ftrivial-auto-var-init")
endif()

check_linker_flag(C "-Wl,-z,nodlopen" C_LINKER_SUPPORTS_NODLOPEN)
if(C_LINKER_SUPPORTS_NODLOPEN)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,nodlopen")
endif()
check_linker_flag(C "-Wl,-z,noexecstack" C_LINKER_SUPPORTS_NOEXECSTACK)
if(C_LINKER_SUPPORTS_NOEXECSTACK)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,noexecstack")
endif()
check_linker_flag(C "-Wl,-z,relro" C_LINKER_SUPPORTS_RELRO)
if(C_LINKER_SUPPORTS_RELRO)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
endif()
check_linker_flag(C "-Wl,-z,now" C_LINKER_SUPPORTS_NOW)
if(C_LINKER_SUPPORTS_NOW)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
endif()
check_linker_flag(C "-Wl,--as-needed" C_LINKER_SUPPORTS_AS_NEEDED)
if(C_LINKER_SUPPORTS_AS_NEEDED)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
endif()
check_linker_flag(C "-Wl,--no-copy-dt-needed-entries" C_LINKER_SUPPORTS_NO_COPY_DT_NEEDED_ENTRIES)
if(C_LINKER_SUPPORTS_NO_COPY_DT_NEEDED_ENTRIES)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-copy-dt-needed-entries")
endif()