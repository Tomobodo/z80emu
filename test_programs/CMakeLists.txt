find_program(SJASMPLUS_EXECUTABLE
  NAMES sjasmplus sjasmplus.exe
  DOC "sjasmplus assembler"
  REQUIRED
)

find_program(MAKE_EXECUTABLE
  NAMES make gmake nmake
  DOC "needed to build the test programs"
  REQUIRED
)

set(TEST_BIN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/tests)

file(MAKE_DIRECTORY ${TEST_BIN_OUTPUT_DIR})

add_custom_target(build_test_programs
  COMMAND ${MAKE_EXECUTABLE} OUTPUT_DIR=${TEST_BIN_OUTPUT_DIR}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_programs
  COMMENT "Compile the test programs"
  VERBATIM
)


