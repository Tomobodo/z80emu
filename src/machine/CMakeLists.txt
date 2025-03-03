add_subdirectory(cpu)

target_sources(
  ${PROJECT_NAME} PRIVATE
  memory.cpp
  mother_board.cpp
  component.cpp
)
