

# Set the C++ standard to C++17 (required for std::filesystem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../app/FileBrowserApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/FileBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/UIManager.cpp
)

add_library(FileBrowserLibrary ${LIB_SOURCES})

set_target_properties(FileBrowserLibrary PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)


find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(FileBrowserLibrary
    PUBLIC 
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
)


install(TARGETS FileBrowserLibrary DESTINATION bin)