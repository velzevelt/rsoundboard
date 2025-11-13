project(tools)

add_executable(file2c resources_embed/file2c.c ../font_sdf.c)
target_link_libraries(file2c raylib)
target_include_directories(file2c PRIVATE ../)
set_target_properties(file2c PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools")


add_executable(image2c resources_embed/image2c.c)
target_link_libraries(image2c raylib)
target_include_directories(image2c PRIVATE)
set_target_properties(image2c PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools")