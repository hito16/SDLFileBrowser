#include "core/UIManager.h"
#include "core/FileBrowser.h"
#include <iostream>
#include <algorithm>

// --- UI Constants  ---
const SDL_Color BLUE_BACKGROUND_BRIGHTER = {0x2C, 0x5D, 0x8A, 0xFF};
const SDL_Color WHITE_COLOR = {255, 255, 255, 255};
const SDL_Color HIGHLIGHT_BORDER_COLOR = {0x00, 0xA0, 0xFF, 0xFF};
const SDL_Color DEFAULT_CELL_BORDER_COLOR = {255, 255, 255, 255};

const int HIGHLIGHT_BORDER_THICKNESS = 3;
const int CELL_SPACING = 5;
const int LINE_HEIGHT = 40;

const int SCROLLBAR_WIDTH = 10;
const int SCROLLBAR_MARGIN_RIGHT = 10;
const int SCROLLBAR_TO_LINE_PADDING = 10;
const int LEFT_MARGIN = 20;

UIManager::UIManager(SDL_Window *window, SDL_Renderer *renderer, int screenWidth, int screenHeight, const std::string &fontPath, int fontSize)
    : m_window(window), m_renderer(renderer), font(nullptr),
      screenWidth(screenWidth), screenHeight(screenHeight), fontSize(fontSize),
      fontPath(fontPath)
{
}

UIManager::~UIManager()
{
    if (font)
    {
        TTF_CloseFont(font);
        font = nullptr;
    }

    TTF_Quit();
}

bool UIManager::init()
{
    if (TTF_Init() == -1)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font)
    {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

void UIManager::clearRenderer()
{
    SDL_SetRenderDrawColor(m_renderer, BLUE_BACKGROUND_BRIGHTER.r, BLUE_BACKGROUND_BRIGHTER.g, BLUE_BACKGROUND_BRIGHTER.b, BLUE_BACKGROUND_BRIGHTER.a);
    SDL_RenderClear(m_renderer);
}

void UIManager::presentRenderer()
{
    SDL_RenderPresent(m_renderer);
}

void UIManager::drawThickRect(const SDL_Rect &rect, int thickness, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < thickness; ++i)
    {
        SDL_Rect currentRect = {
            rect.x - i,
            rect.y - i,
            rect.w + 2 * i,
            rect.h + 2 * i};
        SDL_RenderDrawRect(m_renderer, &currentRect);
    }
}

void UIManager::drawHorizontalLine(int y, int thickness, SDL_Color color, int startX, int endX)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < thickness; ++i)
    {
        SDL_RenderDrawLine(m_renderer, startX, y + i, endX, y + i);
    }
}

void UIManager::drawText(const std::string &text, int x, int y, SDL_Color color)
{
    if (!font)
    {
        std::cerr << "Font not loaded!" << std::endl;
        return;
    }
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface)
    {
        std::cerr << "TTF_RenderText_Blended Error: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    if (!texture)
    {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    }
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(m_renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void UIManager::drawCurrentPath(const std::string &path)
{
    drawText("Current Path: " + path, LEFT_MARGIN, 20, WHITE_COLOR);

    // Calculate endX for the line to avoid scrollbar
    int lineEndX = screenWidth - SCROLLBAR_WIDTH - SCROLLBAR_MARGIN_RIGHT - SCROLLBAR_TO_LINE_PADDING;
    drawHorizontalLine(20 + fontSize + 10, HIGHLIGHT_BORDER_THICKNESS, WHITE_COLOR, LEFT_MARGIN, lineEndX);
}

void UIManager::drawFileList(const std::vector<FileItem> &items, int selectedIndex, int scrollOffset, int visibleItemsCount)
{
    // List starts after the path text and separator line
    // Path text (fontSize) at Y=20, separator at Y=20+fontSize+10, give some padding after line
    int startY = 20 + fontSize + 10 + HIGHLIGHT_BORDER_THICKNESS + 10; // Start Y for the first file cell
    const int CELL_PADDING_X = 10;
    // Adjust CELL_WIDTH to account for scrollbar area
    const int CELL_WIDTH = screenWidth - LEFT_MARGIN - (SCROLLBAR_WIDTH + SCROLLBAR_MARGIN_RIGHT + SCROLLBAR_TO_LINE_PADDING);

    for (int i = 0; i < visibleItemsCount; ++i)
    {
        int itemIndex = scrollOffset + i;
        if (itemIndex >= 0 && itemIndex < items.size())
        {
            std::string displayName = items[itemIndex].name;

            if (items[itemIndex].isDirectory)
            {
                displayName += "/";
            }

            // Calculate cell position and size, accounting for spacing
            SDL_Rect cellRect = {
                LEFT_MARGIN, // Use LEFT_MARGIN for consistent left alignment
                startY + (i * LINE_HEIGHT) + (CELL_SPACING / 2),
                CELL_WIDTH,
                LINE_HEIGHT - CELL_SPACING};

            if (itemIndex == selectedIndex)
            {
                drawThickRect(cellRect, HIGHLIGHT_BORDER_THICKNESS, HIGHLIGHT_BORDER_COLOR);
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer, DEFAULT_CELL_BORDER_COLOR.r, DEFAULT_CELL_BORDER_COLOR.g, DEFAULT_CELL_BORDER_COLOR.b, DEFAULT_CELL_BORDER_COLOR.a);
                SDL_RenderDrawRect(m_renderer, &cellRect);
            }

            drawText(displayName, cellRect.x + CELL_PADDING_X, cellRect.y + (cellRect.h - fontSize) / 2, WHITE_COLOR);
        }
    }
}

void UIManager::drawScrollbar(int totalItems, int visibleItems, int scrollOffset)
{
    if (totalItems <= visibleItems)
    {
        return;
    }

    int scrollbarHeight = screenHeight - (2 * 20);                           // Top and bottom padding for the entire scrollbar track
    int scrollbarX = screenWidth - SCROLLBAR_WIDTH - SCROLLBAR_MARGIN_RIGHT; // Use new constants

    float visibleRatio = (float)visibleItems / totalItems;
    int thumbHeight = std::max(20, (int)(scrollbarHeight * visibleRatio));
    float scrollRatio = (float)scrollOffset / (totalItems - visibleItems);
    int thumbY = 20 + (int)((scrollbarHeight - thumbHeight) * scrollRatio); // Start Y for the scrollbar track is 20

    SDL_SetRenderDrawColor(m_renderer, 0x40, 0x40, 0x40, 0xFF); // Dark gray background for scrollbar track
    SDL_Rect scrollbarBgRect = {scrollbarX, 20, SCROLLBAR_WIDTH, scrollbarHeight};
    SDL_RenderFillRect(m_renderer, &scrollbarBgRect);

    SDL_SetRenderDrawColor(m_renderer, 0x80, 0x80, 0x80, 0xFF); // Lighter gray for scrollbar thumb
    SDL_Rect thumbRect = {scrollbarX, thumbY, SCROLLBAR_WIDTH, thumbHeight};
    SDL_RenderFillRect(m_renderer, &thumbRect);
}

void UIManager::drawHelpText(const std::string &text, int visibleItemsCount)
{
    // Calculate the Y coordinate of the bottom of the file list area.
    // This is the top of the separator line for the help text.
    int calculatedListStart = 20 + fontSize + 10 + HIGHLIGHT_BORDER_THICKNESS + 10;
    int lineTopY = calculatedListStart + (visibleItemsCount * LINE_HEIGHT) + 10; // 10px padding above the line

    // Bottom of the line (including its thickness)
    int lineBottomY = lineTopY + HIGHLIGHT_BORDER_THICKNESS;

    // The available height for the text, from the bottom of the line to the bottom of the screen.
    int availableHeight = screenHeight - lineBottomY;

    // Calculate the Y position to center the text vertically within the available space.
    // (Available space / 2) - (text height / 2)
    int textY = lineBottomY + (availableHeight / 2) - (fontSize / 2);

    int lineEndX = screenWidth - SCROLLBAR_WIDTH - SCROLLBAR_MARGIN_RIGHT - SCROLLBAR_TO_LINE_PADDING;
    drawHorizontalLine(lineTopY, HIGHLIGHT_BORDER_THICKNESS, WHITE_COLOR, LEFT_MARGIN, lineEndX);
    drawText(text, LEFT_MARGIN, textY, WHITE_COLOR);
}

int UIManager::getScreenWidth() const
{
    return screenWidth;
}

int UIManager::getScreenHeight() const
{
    return screenHeight;
}