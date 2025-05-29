#include "app/FileBrowserApp.h"
#include <iostream>
#include <utility>

// UI constants from UIManager.cpp, necessary for calculating visible items
extern const int HIGHLIGHT_BORDER_THICKNESS;
extern const int LINE_HEIGHT;

Uint32 FileBrowserApp::getRequiredSDLInitFlags()
{
    return SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC;
}

FileBrowserApp::FileBrowserApp()
    : uiManager(nullptr), fileBrowser(nullptr), running(false),
      m_currentDialogResult(DialogResult::None), m_selectedFilePath("")
{
}

void FileBrowserApp::closeGameControllers()
{
    for (auto const &[instanceID, controller] : gameControllers)
    {
        if (controller)
        {
            SDL_GameControllerClose(controller);
        }
    }
    gameControllers.clear();
}

FileBrowserApp::~FileBrowserApp()
{
    closeGameControllers();
    if (fileBrowser)
    {
        delete fileBrowser;
        fileBrowser = nullptr;
    }
    if (uiManager)
    {
        delete uiManager;
        uiManager = nullptr;
    }
}

bool FileBrowserApp::init(SDL_Window *window, SDL_Renderer *renderer, const std::string &fontPath, int fontSize)
{
    int screenWidth, screenHeight;
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);

    uiManager = new UIManager(window, renderer, screenWidth, screenHeight, fontPath, fontSize);
    if (!uiManager->init())
    {
        std::cerr << "FileBrowserApp: Failed to initialize UI Manager!" << std::endl;
        return false;
    }

    fileBrowser = new FileBrowser();

    const int PADDING_Y = 20;

    int currentPathAreaHeight = uiManager->getFontSize() + 10 + HIGHLIGHT_BORDER_THICKNESS + 10;
    int helpTextAreaHeight = uiManager->getFontSize() + PADDING_Y;

    int listRenderHeight = uiManager->getScreenHeight() - currentPathAreaHeight - helpTextAreaHeight;
    int visibleItems = listRenderHeight / LINE_HEIGHT;
    fileBrowser->setVisibleItemsCount(visibleItems);

    running = true;
    m_currentDialogResult = DialogResult::None;
    m_selectedFilePath = "";

    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER))
    {
        for (int i = 0; i < SDL_NumJoysticks(); ++i)
        {
            if (SDL_IsGameController(i))
            {
                SDL_GameController *c = SDL_GameControllerOpen(i);
                if (c)
                {
                    SDL_JoystickID instanceID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c));
                    gameControllers[instanceID] = c;
                    printf("Game Controller Added: %s (Instance ID: %d)\n", SDL_GameControllerName(c), instanceID);
                }
                else
                {
                    fprintf(stderr, "Could not open game controller: %s\\n", SDL_GetError());
                }
            }
        }
    }
    else
    {
        std::cerr << "SDL_INIT_GAMECONTROLLER was not initialized. Controller support may be limited." << std::endl;
    }

    return true;
}

void FileBrowserApp::updateAndRender()
{
    if (!running || !uiManager || !fileBrowser)
    {
        return;
    }

    uiManager->clearRenderer();
    uiManager->drawCurrentPath(fileBrowser->getCurrentPath());
    uiManager->drawFileList(fileBrowser->getCurrentItems(),
                            fileBrowser->getSelectedIndex(),
                            fileBrowser->getScrollOffset(),
                            fileBrowser->getVisibleItemsCount());
    uiManager->drawScrollbar(fileBrowser->getCurrentItems().size(),
                             fileBrowser->getVisibleItemsCount(),
                             fileBrowser->getScrollOffset());
    uiManager->drawHelpText("Keyboard: Arrows/Enter/Bksp/Esc | Controller: DPad/A/B/Start", fileBrowser->getVisibleItemsCount());
    uiManager->presentRenderer();
}

void FileBrowserApp::handleInput(SDL_Event &e)
{
    if (!running)
        return;

    Action action = Action::None; // Default action

    // Step 2: Map SDL events to our custom Action enum
    switch (e.type)
    {
    case SDL_KEYDOWN:
        // clang-format off
            switch (e.key.keysym.sym) {
                case SDLK_UP:       action = Action::NavigateUp; break;
                case SDLK_DOWN:     action = Action::NavigateDown; break;
                case SDLK_BACKSPACE:action = Action::NavigateParent; break;
                case SDLK_RETURN:   action = Action::SelectConfirm; break;
                case SDLK_ESCAPE:   action = Action::Cancel; break;
                default: break;
            }
        // clang-format on
        break;

    case SDL_CONTROLLERBUTTONDOWN:
        // clang-format off
            switch (e.cbutton.button) {
                case SDL_CONTROLLER_BUTTON_DPAD_UP:   action = Action::NavigateUp; break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: action = Action::NavigateDown; break;
                case SDL_CONTROLLER_BUTTON_B:         action = Action::NavigateParent; break; // B for Back
                case SDL_CONTROLLER_BUTTON_A:         action = Action::SelectConfirm; break;
                case SDL_CONTROLLER_BUTTON_START:     action = Action::Cancel; break; // Start to cancel/quit
                default: break;
            }
        // clang-format on
        break;

    case SDL_QUIT:
        action = Action::QuitApp;
        break;

    case SDL_CONTROLLERDEVICEADDED:
    {
        SDL_GameController *c = SDL_GameControllerOpen(e.cdevice.which);
        if (c)
        {
            SDL_JoystickID instanceID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(c));
            gameControllers[instanceID] = c;
            printf("Game Controller Added: %s (Instance ID: %d)\n", SDL_GameControllerName(c), instanceID);
        }
        else
        {
            fprintf(stderr, "Could not open game controller: %s\\n", SDL_GetError());
        }
    }
    break;

    case SDL_CONTROLLERDEVICEREMOVED:
    {
        auto it = gameControllers.find(e.cdevice.which);
        if (it != gameControllers.end())
        {
            SDL_GameController *c = it->second;
            printf("Game Controller Removed: %s (Instance ID: %d).\\n", SDL_GameControllerName(c), e.cdevice.which);
            SDL_GameControllerClose(c);
            gameControllers.erase(it);
        }
        else
        {
            fprintf(stderr, "Attempted to remove untracked controller with Instance ID: %d\\n", e.cdevice.which);
        }
    }
    break;
    }

    // Step 3: Execute the determined action
    switch (action)
    {
    case Action::NavigateUp:
        fileBrowser->selectPreviousItem();
        break;
    case Action::NavigateDown:
        fileBrowser->selectNextItem();
        break;
    case Action::NavigateParent:
        fileBrowser->goUpDirectory();
        break;
    case Action::SelectConfirm:
    {
        if (fileBrowser->getCurrentItems().empty())
            return;

        const FileItem &selectedItem = fileBrowser->getCurrentItems()[fileBrowser->getSelectedIndex()];
        if (!selectedItem.isDirectory)
        {
            m_selectedFilePath = fileBrowser->getCurrentPath() + "/" + selectedItem.name;
            m_currentDialogResult = DialogResult::FileSelected;
            running = false; // Exit the loop
        }
        else
        {
            fileBrowser->tryOpenSelectedItem(); // Open directory
        }
    }
    break;
    case Action::Cancel:
        m_currentDialogResult = DialogResult::Cancelled;
        running = false; // Exit the loop
        break;
    case Action::QuitApp: // Handle the application quit action
        m_currentDialogResult = DialogResult::QuitApp;
        running = false; // Exit the main loop
        break;
    case Action::None:
        // Do nothing for unmapped inputs or other event types
        break;
    }
}

FileBrowserApp::DialogResult FileBrowserApp::runStandaloneLoop()
{
    SDL_Event e;
    while (running)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            handleInput(e);
        }
        updateAndRender();
        SDL_Delay(16);
    }
    return m_currentDialogResult;
}

std::string FileBrowserApp::showFileSelectionDialog(SDL_Window *window, SDL_Renderer *renderer,
                                                    int screenWidth, int screenHeight,
                                                    const std::string &fontPath, int fontSize)
{
    FileBrowserApp modalApp;

    if (!modalApp.init(window, renderer, fontPath, fontSize))
    {
        std::cerr << "FileBrowserApp::showFileSelectionDialog: Failed to initialize modal app." << std::endl;
        return "";
    }

    DialogResult result = modalApp.runStandaloneLoop();

    if (result == DialogResult::FileSelected)
    {
        return modalApp.m_selectedFilePath;
    }
    else
    {
        return "";
    }
}