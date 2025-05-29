
add_executable(file_browser_example main.cpp)

target_link_libraries(file_browser_example PRIVATE filebrowser)

set_target_properties(file_browser_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)

install(TARGETS file_browser_example DESTINATION bin)