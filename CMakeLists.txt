cmake_minimum_required(VERSION 3.25)

project(
  z80emu
  LANGUAGES CXX
  VERSION 0.1.0
)

# Include FetchContent module
include(FetchContent)

# Fetch SDL2
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.28.5
)

# Fetch ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.5
)

# Make dependencies available
FetchContent_MakeAvailable(SDL2 imgui)

# Copy ImGui backend files to libs directory
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/libs/imgui)
file(COPY ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
     ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.h
     ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
     ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h
     DESTINATION ${CMAKE_SOURCE_DIR}/libs/imgui)

add_library(imgui_lib STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(imgui_lib PRIVATE 
	${imgui_SOURCE_DIR}
	${SDL2_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME})

set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 23
  CMAKE_CXX_STANDARD_REQUIRED ON
)

add_subdirectory(src)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  SDL2::SDL2
	imgui_lib
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:SDL2>
		$<TARGET_FILE_DIR:${PROJECT_NAME}>
)