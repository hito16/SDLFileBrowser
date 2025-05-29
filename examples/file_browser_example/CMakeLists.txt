# This CMakeLists.txt defines the example executable for the FileBrowserLibrary.

# Define the executable target for your example application
add_executable(file_browser_example main.cpp)

# Link the executable against the FileBrowserLibrary
# The library must be defined *before* this target is configured (in src/lib/CMakeLists.txt).
target_link_libraries(file_browser_example PRIVATE FileBrowserLibrary)

# Set the output directory for the executable
# CMAKE_BINARY_DIR points to your top-level build directory (e.g., SDLFileBrowser/build).
# By using "../bin", the executable will be placed in SDLFileBrowser/bin.
set_target_properties(file_browser_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)

# Install the executable (optional, for 'cmake --install .')
install(TARGETS file_browser_example DESTINATION bin)
