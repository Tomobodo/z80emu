add_library(machine_lib STATIC)

add_subdirectory(cpu)

target_sources(
  machine_lib PRIVATE
  memory.cpp
  mother_board.cpp
  component.cpp
)

set_target_properties(
	machine_lib PROPERTIES
	CXX_STANDARD 23
	CMAKE_CXX_STANDARD_REQUIRED ON
)

target_include_directories(
	machine_lib PRIVATE
	${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	machine_lib
)
