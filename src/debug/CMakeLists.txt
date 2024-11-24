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
  ${PROJECT_SOURCE_DIR}/libs/imgui/imgui_impl_sdl2.cpp
  ${PROJECT_SOURCE_DIR}/libs/imgui/imgui_impl_sdlrenderer2.cpp
)

find_package(SDL2 REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)

target_include_directories(
  debug_card PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/libs
  ${SDL2_INCLUDE_DIR}/
  ${imgui_INCLUDE_DIR}/
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  debug_card
  SDL2::SDL2
  imgui::imgui
)
