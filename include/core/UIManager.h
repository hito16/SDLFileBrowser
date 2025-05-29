#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

struct FileItem;

extern const int HIGHLIGHT_BORDER_THICKNESS;
extern const int LINE_HEIGHT;

class UIManager
{
public:
    UIManager(SDL_Window *window, SDL_Renderer *renderer, int screenWidth, int screenHeight, const std::string &fontPath, int fontSize);
    ~UIManager();

    bool init();

    void clearRenderer();
    void presentRenderer();

    void drawCurrentPath(const std::string &path);
    void drawFileList(const std::vector<FileItem> &items, int selectedIndex, int scrollOffset, int visibleItemsCount);
    void drawHelpText(const std::string &text, int visibleItemsCount);

    void drawScrollbar(int totalItems, int visibleItems, int scrollOffset);

    int getScreenWidth() const;
    int getScreenHeight() const;
    int getFontSize() const { return fontSize; }

private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    TTF_Font *font;
    int screenWidth;
    int screenHeight;
    int fontSize;
    std::string fontPath;

    void drawText(const std::string &text, int x, int y, SDL_Color color);
    void drawThickRect(const SDL_Rect &rect, int thickness, SDL_Color color);
    void drawHorizontalLine(int y, int thickness, SDL_Color color, int startX, int endX);
};

#endif // UIMANAGER_H