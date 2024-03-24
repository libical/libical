# SPDX-FileCopyrightText: Allen Winter <winter@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0

#-------------------------------------------------
# some argument checking:
# test_cmd is the command to run with all its arguments
if(NOT test_cmd)
  message(FATAL_ERROR "Variable test_cmd not defined")
endif()

# output_blessed contains the name of the "blessed" output file
if(NOT output_blessed)
  message(FATAL_ERROR "Variable output_blessed not defined")
endif()

# output_test contains the name of the output file the test_cmd will produce
if(NOT output_test)
  message(FATAL_ERROR "Variable output_test not defined")
endif()

# convert the space-separated string to a list
separate_arguments(test_args UNIX_COMMAND ${test_args})

execute_process(
  COMMAND ${test_cmd} ${test_args}
  RESULT_VARIABLE test_not_successful
  ERROR_VARIABLE err
)

if(test_not_successful)
  message(SEND_ERROR "Unable to run test '${test_cmd}': ${err} : shell output: ${test_not_successful}!")
endif()

#cmake v3.14 compare_files can ignore line endings
set(ignore_eol "")
if(NOT CMAKE_VERSION VERSION_LESS 3.14)
  set(ignore_eol "--ignore-eol")
endif()
execute_process(
  COMMAND ${CMAKE_COMMAND} -E compare_files ${ignore_eol} ${output_blessed} ${output_test}
  RESULT_VARIABLE test_not_successful
  ERROR_VARIABLE err
)

if(test_not_successful)
  message(SEND_ERROR
    "Output does not match for ${output_blessed} and ${output_test}: ${err} : shell output: ${test_not_successful}!"
  )
endif()
