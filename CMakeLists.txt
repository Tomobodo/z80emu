cmake_minimum_required(VERSION 3.25)

project(
  z80emu
  LANGUAGES CXX
  VERSION 0.1.0
)

add_executable(${PROJECT_NAME})

set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 23
  CMAKE_CXX_STANDARD_REQUIRED ON
)

add_subdirectory(src)
