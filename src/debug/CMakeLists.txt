add_library(
  debug_card
)

set_target_properties(
  debug_card PROPERTIES
  CXX_STANDARD 23
  CMAKE_CXX_STANDARD_REQUIRED ON
)

target_sources(
  debug_card PRIVATE
  debug_card.cpp
)

target_include_directories(
  debug_card PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/libs
  ${imgui_SOURCE_DIR}
  ${SDL3_SOURCE_DIR}/include/SDL3
)

target_link_libraries(
  debug_card PUBLIC
  SDL3::SDL3
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  debug_card
)
