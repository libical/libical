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
check_compiler_flag(CXX "-Wall" CXX_SUPPORTS_WALL)
if(CXX_SUPPORTS_WALL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
check_compiler_flag(CXX "-Wextra" CXX_SUPPORTS_WEXTRA)
if(CXX_SUPPORTS_WEXTRA)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()
check_compiler_flag(CXX "-Wformat" CXX_SUPPORTS_WFORMAT)
if(CXX_SUPPORTS_WFORMAT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat")
endif()
check_compiler_flag(CXX "-Wformat=2" CXX_SUPPORTS_WFORMAT2)
if(CXX_SUPPORTS_WFORMAT2)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2")
endif()
check_compiler_flag(CXX "-Wconversion" CXX_SUPPORTS_WCONVERSION)
if(CXX_SUPPORTS_WCONVERSION)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
endif()
check_compiler_flag(CXX "-Wsign-conversion" CXX_SUPPORTS_WSIGNCONVERSION)
if(CXX_SUPPORTS_WSIGNCONVERSION)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
endif()
check_compiler_flag(CXX "-Wtrampolines" CXX_SUPPORTS_WTRAMPOLINES)
if(CXX_SUPPORTS_WTRAMPOLINES)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wtrampolines")
endif()
check_compiler_flag(CXX "-Wimplicit-fallthrough" CXX_SUPPORTS_WIMPLICIT_FALLTHROUGH)
if(CXX_SUPPORTS_WIMPLICIT_FALLTHROUGH)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit-fallthrough")
endif()
check_compiler_flag(CXX "-Wbidi-chars=any" CXX_SUPPORTS_WBIDI_CHARS_ANY)
if(CXX_SUPPORTS_WBIDI_CHARS_ANY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wbidi-chars=any")
endif()
check_compiler_flag(CXX "-Werror=format-security" CXX_SUPPORTS_WERROR_FORMAT_SECURITY)
if(CXX_SUPPORTS_WERROR_FORMAT_SECURITY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=format-security")
endif()
check_compiler_flag(CXX "-Werror=implicit" CXX_SUPPORTS_WERROR_IMPLICIT)
if(CXX_SUPPORTS_WERROR_IMPLICIT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=implicit")
endif()
check_compiler_flag(CXX "-Werror=incompatible-pointer-types" CXX_SUPPORTS_WERROR_INCOMPATIBLE_POINTER_TYPES)
if(CXX_SUPPORTS_WERROR_INCOMPATIBLE_POINTER_TYPES)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=incompatible-pointer-types")
endif()
check_compiler_flag(CXX "-Werror=int-conversion" CXX_SUPPORTS_WERROR_INT_CONVERSION)
if(CXX_SUPPORTS_WERROR_INT_CONVERSION)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=int-conversion")
endif()
# Note that the below should only be enabled during development, not source distribution
#check_compiler_flag(CXX "-Werror" CXX_SUPPORTS_WERROR)
#if(CXX_SUPPORTS_WERROR)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#endif()

# Table 2: Recommended compiler options that enable run-time protection mechanisms.
check_compiler_flag(CXX "-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3" CXX_SUPPORTS_FORTIFY_SOURCE)
if(CXX_SUPPORTS_FORTIFY_SOURCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3")
endif()
check_compiler_flag(CXX "-D_GLIBCXX_ASSERTIONS" CXX_SUPPORTS_GLIBCXX_ASSERTIONS)
if(CXX_SUPPORTS_GLIBCXX_ASSERTIONS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_ASSERTIONS")
endif()
check_compiler_flag(CXX "-fstrict-flex-arrays=3" CXX_SUPPORTS_FSTRICT_FLEX_ARRAYS3)
if(CXX_SUPPORTS_FSTRICT_FLEX_ARRAYS3)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-flex-arrays=3")
endif()
check_compiler_flag(CXX "-fstack-clash-protection" CXX_SUPPORTS_FSTACK_CLASH_PROTECTION)
if(CXX_SUPPORTS_FSTACK_CLASH_PROTECTION)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-clash-protection")
endif()
check_compiler_flag(CXX "-fstack-protector-strong" CXX_SUPPORTS_FSTACK_PROTECTOR_STRONG)
if(CXX_SUPPORTS_FSTACK_PROTECTOR_STRONG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
endif()
check_compiler_flag(CXX "-fcf-protection=full" CXX_SUPPORTS_FCF_PROTECTION_FULL)
if(CXX_SUPPORTS_FCF_PROTECTION_FULL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcf-protection=full")
endif()
check_compiler_flag(CXX "-mbranch-protection=standard" CXX_SUPPORTS_MBRANCH_PROTECTION_STANDARD)
if(CXX_SUPPORTS_MBRANCH_PROTECTION_STANDARD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbranch-protection=standard")
endif()
check_compiler_flag(CXX "-fno-delete-null-pointer-checks" CXX_SUPPORTS_FNO_DELETE_NULL_POINTER_CHECKS)
if(CXX_SUPPORTS_FNO_DELETE_NULL_POINTER_CHECKS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
endif()
check_compiler_flag(CXX "-fno-strict-overflow" CXX_SUPPORTS_FNO_STRICT_OVERFLOW)
if(CXX_SUPPORTS_FNO_STRICT_OVERFLOW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow")
endif()
check_compiler_flag(CXX "-fno-strict-aliasing" CXX_SUPPORTS_FNO_STRICT_ALIASING)
if(CXX_SUPPORTS_FNO_STRICT_ALIASING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
endif()
check_compiler_flag(CXX "-ftrivial-auto-var-init" CXX_SUPPORTS_FTRIVIAL_AUTO_VAR_INIT)
if(CXX_SUPPORTS_FTRIVIAL_AUTO_VAR_INIT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftrivial-auto-var-init")
endif()

check_linker_flag(CXX "-Wl,-z,nodlopen" CXX_LINKER_SUPPORTS_NODLOPEN)
if(CXX_LINKER_SUPPORTS_NODLOPEN)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,nodlopen")
endif()
check_linker_flag(CXX "-Wl,-z,noexecstack" CXX_LINKER_SUPPORTS_NOEXECSTACK)
if(CXX_LINKER_SUPPORTS_NOEXECSTACK)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,noexecstack")
endif()
check_linker_flag(CXX "-Wl,-z,relro" CXX_LINKER_SUPPORTS_RELRO)
if(CXX_LINKER_SUPPORTS_RELRO)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
endif()
check_linker_flag(CXX "-Wl,-z,now" CXX_LINKER_SUPPORTS_NOW)
if(CXX_LINKER_SUPPORTS_NOW)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
endif()
check_linker_flag(CXX "-Wl,--as-needed" CXX_LINKER_SUPPORTS_AS_NEEDED)
if(CXX_LINKER_SUPPORTS_AS_NEEDED)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
endif()
check_linker_flag(CXX "-Wl,--no-copy-dt-needed-entries" CXX_LINKER_SUPPORTS_NO_COPY_DT_NEEDED_ENTRIES)
if(CXX_LINKER_SUPPORTS_NO_COPY_DT_NEEDED_ENTRIES)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-copy-dt-needed-entries")
endif()
