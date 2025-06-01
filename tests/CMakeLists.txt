cmake_minimum_required(VERSION 3.25)

add_executable(emuz80_tests
	main.cpp
	machine_utils.cpp
	load_operations_tests.cpp
)

target_link_libraries(emuz80_tests PRIVATE
    doctest::doctest
		machine_lib
)

target_include_directories(emuz80_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
		${doctest_SOURCE_DIR}
)

set_target_properties(emuz80_tests PROPERTIES
    CXX_STANDARD 23
    CMAKE_CXX_STANDARD_REQUIRED O
)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(emuz80_tests)

find_program(SJASMPLUS_EXECUTABLE
  NAMES sjasmplus sjasmplus.exe
  DOC "sjasmplus assembler"
)

find_program(MAKE_EXECUTABLE 
  NAMES make gmake nmake 
  DOC "needed to build the test programs"
  REQUIRED
)

add_custom_target(test_programs
  COMMAND ${MAKE_EXECUTABLE}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_programs
  COMMENT "Compile the test programs"
  VERBATIM
)

if (TARGET emuz80_tests)
  add_dependencies(emuz80_tests test_programs)
endif()

add_custom_command(
  TARGET emuz80_tests PRE_BUILD
  COMMAND ${MAKE_EXECUTABLE}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_programs
  COMMENT "Auto compile the test programs"
  VERBATIM
)
