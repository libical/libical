#-------------------------------------------------
# some argument checking:
# test_cmd is the command to run with all its arguments
if(NOT test_cmd)
  message(FATAL_ERROR "Variable test_cmd not defined")
endif()

# convert the space-separated string to a list
separate_arguments(test_args)

execute_process(
  COMMAND ${test_cmd} ${test_args}
  RESULT_VARIABLE test_not_successful
  ERROR_VARIABLE err
)

if(test_not_successful)
  message(SEND_ERROR "Unable to run test '${test_cmd}': ${err} : shell output: ${test_not_successful}!")
endif()
