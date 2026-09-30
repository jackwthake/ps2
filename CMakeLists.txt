cmake_minimum_required(VERSION 3.16)
project(tundra CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
enable_language(C)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# dont build non-necessary sdl bits
set(SDL_TESTS OFF CACHE INTERNAL "")
set(SDL_TEST_LIBRARY OFF CACHE INTERNAL "")
set(SDL_EXAMPLES OFF CACHE INTERNAL "")

# --- Build SDL first ---
set(SDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL")
if(NOT EXISTS "${SDL_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "SDL source not found at ${SDL_DIR}. Check the vendored dependency path.")
endif()
add_subdirectory("${SDL_DIR}")

# --- glad (vendored, not a system package) ---
set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/glad")

# --- Dependencies ---
find_package(PkgConfig REQUIRED)

# --- Main executable ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME}
  ${SOURCES}
  "${GLAD_DIR}/src/glad.c"
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  "${SDL_DIR}/include"
  "${GLAD_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  SDL3::SDL3
  ${CMAKE_DL_LIBS}   # glad needs dlopen/dlsym on Linux
  m
)

# --- Copy assets to build directory ---
file(GLOB_RECURSE ASSET_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/res/*")

add_custom_target(copy_res
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/res"
    "${CMAKE_CURRENT_BINARY_DIR}/res"
  COMMENT "Copying res to build directory"
  SOURCES ${ASSET_FILES}
)

add_dependencies(${PROJECT_NAME} copy_res)