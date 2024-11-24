cmake_minimum_required(VERSION 3.25)

project(
  z80emu
  LANGUAGES CXX
  VERSION 0.1.0

)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

