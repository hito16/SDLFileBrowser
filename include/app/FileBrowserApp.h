#ifndef FILEBROWBERAPP_H
#define FILEBROWBERAPP_H

#include "core/UIManager.h"
#include "core/FileBrowser.h"
#include <SDL.h>
#include <string>
#include <map>

class FileBrowserApp {
public:
    enum class DialogResult {
        None,           // Loop is still active or no specific action
        FileSelected,   
        Cancelled,      
        QuitApp         // SDL_QUIT event received
    };


    enum class Action {
        NavigateUp,
        NavigateDown,
        NavigateParent,
        SelectConfirm,
        Cancel,
        QuitApp, 
        None    
    };

    static Uint32 getRequiredSDLInitFlags();

    FileBrowserApp();
    ~FileBrowserApp();

    bool init(SDL_Window* window, SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
    void updateAndRender();
    void handleInput(SDL_Event& e); 
    bool isDone() const { return !running; }

    DialogResult runStandaloneLoop();

    static std::string showFileSelectionDialog(SDL_Window* window, SDL_Renderer* renderer,
                                               int screenWidth, int screenHeight,
                                               const std::string& fontPath, int fontSize);

private:
    UIManager* uiManager;
    FileBrowser* fileBrowser;
    bool running;

    std::map<SDL_JoystickID, SDL_GameController*> gameControllers;

    DialogResult m_currentDialogResult;
    std::string m_selectedFilePath;

    void closeGameControllers();

};

#endif // FILEBROWSERAPP_H