project(tools)

add_executable(font2c resources_embed/font2c.c)
target_link_libraries(font2c raylib)
set_target_properties(font2c PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools")