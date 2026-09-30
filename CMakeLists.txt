cmake_minimum_required(VERSION 3.16)
project(ps2 CXX)

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

# --- Library ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME}
  ${SOURCES}
  "${GLAD_DIR}/src/glad.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC
  include
  "${SDL_DIR}/include"
  "${GLAD_DIR}/include"
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  SDL3::SDL3
  ${CMAKE_DL_LIBS}   # glad needs dlopen/dlsym on Linux
)


# example programs
option(PS2_BUILD_DEMOS "Build demo programs" ON)

if (PS2_BUILD_DEMOS)
  add_subdirectory(examples/hello_world)
endif()