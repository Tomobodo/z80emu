add_library(
  debug_card
)

target_sources(
  debug_card PRIVATE
  debug_card.cpp
)

target_include_directories(
  debug_card PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/libs/SDL/include
)

message(STATUS "test : ${CMAKE_SOURCE_DIR}/src")

add_subdirectory(${PROJECT_SOURCE_DIR}/libs/SDL SDL)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  debug_card
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  SDL3::SDL3
)
