# SDLFileBrowser: 
Select a file, pass on the string file name 

## Project Overview

`SDLFileBrowser` is a lightweight, cross-platform file selection interface.  Sometimes you just want to select file, and pass it into an existing program.


## Design
The project targets small embedded systems and gaming consoles that may not support proper terminal/keyboard interaction, but do support a screen and a simple input device.  Often, existing code just needs a file name to run (ex. an image viewer, media player, etc).  This makes it simple to collect user input and move on.
* include a simple library in your SDL app
* SDL_Init(), SDL_CreateWindow(), SDL_CreateRenderer and SDL_Quit() are called outside this code (see the example)


## Building the Project

### Prerequisites

* **CMake:** Version 3.10 or higher.
* **C++ Compiler:** A C++17 compatible compiler (e.g., GCC 7+, Clang 5+, MSVC 19.14+).
* **SDL2 Development Libraries:**
    * `SDL2`
    * `SDL2_ttf` (for text rendering)
    * Ensure these are installed and discoverable by CMake on your system.

### Build Steps

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/hito16/SDLFileBrowser.git](https://github.com/hito16/SDLFileBrowser.git)
    cd SDLFileBrowser
    ```

2.  **Create a build directory:**
    It's recommended to build out-of-source.
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project with CMake:**
    ```bash
    cmake ..
    ```
    If SDL2 or SDL2_ttf are not found, you might need to specify their paths using CMake variables (e.g., `-DSDL2_PATH=/path/to/your/sdl2`).

4.  **Build the project:**
    ```bash
    cmake --build .
    ```

After a successful build, the `filebrowser` library (e.g., `libfilebrowser.a` or `libfilebrowser.so`) and the `file_browser_example` executable will be found in the `bin/` directory at the root of your project.

## Usage

### Running the Example

Once built, you can run the example application from the project root:

```bash
./bin/file_browser_example
```
Use your keyboard arrow keys, Enter, Backspace, Esc, or a connected game controller (D-Pad, A, B, Start buttons) to navigate the file system.

### Key/Button Functions

| Key/Button        | Function               |
| :---------------- | :--------------------- |
| Up Arrow / D-Pad Up | Select Previous Item   |
| Down Arrow / D-Pad Down | Select Next Item       |
| Enter / A Button  | Open Item / Select File |
| Backspace / B Button | Go Up Directory        |
| Escape / Start Button | Cancel / Exit Dialog   |

### Integrating the Library into Your Project

To use the `filebrowser` library in your own CMake-based project:

1.  **Add `SDLFileBrowser` as a subdirectory or use `find_package`:**
    The simplest way is often to add it as a subdirectory:
    ```cmake
    # In your project's CMakeLists.txt
    add_subdirectory(path/to/SDLFileBrowser)

    # Then, link your executable to the library
    target_link_libraries(YourApp PRIVATE filebrowser)
    ```

1b. **(or) for Makefiles:**
    just build this project, and point your Makefile at the .h / .a files

2.  **Include headers:**
    ```cpp
    #include "app/FileBrowserApp.h"
    // Other headers like "core/FileBrowser.h", "core/UIManager.h" if needed
    ```
3.  **Instantiate and use `FileBrowserApp`:**
    Refer to `examples/file_browser_example/main.cpp` for a basic usage example.
