cmake_minimum_required(VERSION 3.10)
project(FileBrowser CXX)

# Set the C++ standard for the entire project to C++17
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

include_directories(include)

add_subdirectory(src/lib)

add_subdirectory(examples/file_browser_example) 

# Optionally, you might want to specify a build directory
# For example: cmake -S . -B build
