add_subdirectory(machine)
add_subdirectory(debug)

target_sources(
  ${PROJECT_NAME} PRIVATE
  main.cpp
)
