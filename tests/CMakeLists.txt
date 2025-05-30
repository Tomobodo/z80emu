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
