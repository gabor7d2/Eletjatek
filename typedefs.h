#ifndef ELETJATEK_TYPEDEFS_H
#define ELETJATEK_TYPEDEFS_H

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "debugmalloc.h"

#define DEFAULT_WINDOW_X 1050
#define DEFAULT_WINDOW_Y 800
#define MIN_WINDOW_X 600
#define MIN_WINDOW_Y 600

#define MENU_WIDTH 250

#define DEFAULT_CELLS_X 20
#define DEFAULT_CELLS_Y 20
#define FRAMETIME_MS 20
#define DEFAULT_SIM_SPEED_MS 101

// Enum a cellaállapotokhoz
typedef enum CellState {
    DEAD, LIVE
} CellState;

typedef enum MenuAction {
    CLICKME
} MenuAction;

typedef enum MenuElementType {
    BUTTON, TEXTFIELD
} MenuElementType;

typedef struct Vector2s {
    short x, y;
} Vector2s;

typedef struct Vector2d {
    double x, y;
} Vector2d;

// Struct a játéktér (cellák) tárolására
typedef struct GameField {
    // Jelenlegi cellák és új cellák
    // (a newCells tömbbe kerülnek a következő iteráció cellaértékei majd a 2 fel lesz cserélve)
    CellState **cells, **newCells;
    // Cellák mennyisége mindkét irányban
    Vector2s size;
} GameField;

// Struct a grid paramétereknek
typedef struct GridParams {
    SDL_Rect gameArea, gridArea;
    Vector2s borderWidth, padding, cells;
    Vector2d cellSize;
    SDL_Color deadColor, liveColor, borderColor, bgColor;
} GridParams;

typedef struct Text {
    SDL_Rect area;
    char *text;
    TTF_Font *textFont;
    SDL_Color textColor;
    SDL_Texture *texture;
} Text;

typedef struct {
    /* @param edgeColor
    * @param normalColor
    * @param hoverColor A menüelem háttérszíne amikor a kurzor felette van, vagy meg van nyomva.*/
    SDL_Color edgeColor, normalColor, interactColor, selectColor;
} MenuElementColors;

typedef struct MenuElement {
    MenuElementType type;
    MenuAction action;
    MenuElementColors colors;
    SDL_Rect area;
    Text *text;
    Uint8 interactAlpha;
    bool clicked;
} MenuElement;

typedef struct Menu {
    SDL_Rect area;
    SDL_Color bgColor;

    int textCount;
    Text **texts;

    int elementCount;
    MenuElement **elements;
    MenuElement *foundElement;
    MenuElement *selTextField;
} Menu;

typedef struct Game {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Rect windowArea;
    GameField *gameField;
    GridParams *gridParams;
    Menu *menu;

    int simSpeedMs;
    bool simRunning, drawing;
    CellState drawMode;
    SDL_Point cursorPos;
} Game;

#endif //ELETJATEK_TYPEDEFS_H
