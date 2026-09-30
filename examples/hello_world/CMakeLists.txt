add_executable(hello_world main.cpp)
target_link_libraries(hello_world PRIVATE ps2)
set_target_properties(hello_world PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/hello_world"
)

# --- Copy assets to build directory ---
file(GLOB_RECURSE ASSET_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/res/*")

add_custom_target(hello_world_copy_res
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/res"
    "${CMAKE_BINARY_DIR}/examples/hello_world/res"
  COMMENT "Copying res to build directory"
  SOURCES ${ASSET_FILES}
)

add_dependencies(hello_world hello_world_copy_res)