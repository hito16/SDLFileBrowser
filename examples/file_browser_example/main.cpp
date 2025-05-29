#include "app/FileBrowserApp.h"
#include <iostream>
#include <SDL.h>

const int SCREEN_WIDTH = 854;
const int SCREEN_HEIGHT = 480;
const int FONT_SIZE = 24;

int main(int argc, char* args[]) {
    std::string fontPath = "res/Roboto-Regular.ttf";

    Uint32 requiredFlags = FileBrowserApp::getRequiredSDLInitFlags();
    if (SDL_Init(requiredFlags) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Simple File Browser", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Demonstrate usage of the new modal dialog function ---
    std::cout << "Launching modal file selection dialog..." << std::endl;
    std::string selectedFile = FileBrowserApp::showFileSelectionDialog(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, fontPath, FONT_SIZE);

    if (!selectedFile.empty()) {
        std::cout << "Selected file: " << selectedFile << std::endl;
    } else {
        std::cout << "File selection cancelled or no file selected." << std::endl;
    }
    // --- End of modal dialog demonstration ---

    // The existing FileBrowserApp instance and its standalone loop
    // can still be used here if desired, demonstrating that behavior is preserved.
    // FileBrowserApp app;
    // if (!app.init(window, renderer, fontPath, FONT_SIZE)) {
    //     std::cerr << "Application initialization failed!" << std::endl;
    //     SDL_DestroyRenderer(renderer);
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     return 1;
    // }
    // FileBrowserApp::DialogResult standaloneResult = app.runStandaloneLoop();
    // if (standaloneResult == FileBrowserApp::DialogResult::QuitApp) {
    //     std::cout << "Standalone app quit." << std::endl;
    // }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}