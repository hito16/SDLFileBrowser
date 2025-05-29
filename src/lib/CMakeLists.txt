# This CMakeLists.txt handles building your core library.


set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../app/FileBrowserApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/FileBrowser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/UIManager.cpp
)

add_library(filebrowser ${LIB_SOURCES}) 

set_target_properties(filebrowser PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)


find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(filebrowser
    PUBLIC
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
)

install(TARGETS filebrowser DESTINATION bin) 