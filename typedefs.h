#ifndef ELETJATEK_TYPEDEFS_H
#define ELETJATEK_TYPEDEFS_H

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "debugmalloc.h"

// Enum a cellaállapotokhoz
typedef enum {
    DEAD, LIVE
} CellState;

typedef enum {
    CLICKME
} MenuAction;

typedef enum {
    BUTTON, TEXTFIELD
} MenuElementType;

typedef struct {
    short x, y;
} Vector2s;

typedef struct {
    double x, y;
} Vector2d;

// Struct a játéktér (cellák) tárolására
typedef struct {
    // Jelenlegi cellák és új cellák
    // (a newCells tömbbe kerülnek a következő iteráció cellaértékei majd a 2 fel lesz cserélve)
    CellState **cells, **newCells;
    // Cellák mennyisége mindkét irányban
    Vector2s size;
} GameField;

// Struct a grid paramétereknek
typedef struct {
    SDL_Rect gameArea, gridArea;
    Vector2s borderWidth, padding, cells;
    Vector2d cellSize;
    SDL_Color deadColor, liveColor, borderColor, bgColor;
} GridParams;

// Struct a szimulációhoz szükséges adatok mozgatására
typedef struct {
    bool *running, *renderNeeded;
    int *speedMs;
    GameField *gameField;
} SimData;

typedef struct {
    SDL_Rect area;
    char *text;
    TTF_Font *textFont;
    SDL_Color textColor;
    SDL_Texture *texture;
} Text;

typedef struct {
    /* @param edgeColor A menüelem szélének a színe.
    * @param normalColor A menüelem háttérszíne normál állapotban.
    * @param hoverColor A menüelem háttérszíne amikor a kurzor felette van, vagy meg van nyomva.*/
    SDL_Color edgeColor, normalColor, interactColor;
} MenuElementColors;

typedef struct {
    MenuElementType type;
    MenuAction action;
    MenuElementColors colors;
    SDL_Rect area;
    Text *text;
    short interactAlpha;
    bool clicked;
} MenuElement;

typedef struct {
    SDL_Rect area;
    SDL_Color bgColor;

    int textCount;
    Text **texts;

    int elementCount;
    MenuElement **elements;
    MenuElement *foundElement;
    MenuElement *selTextField;
} Menu;

typedef struct {
    SDL_Renderer *renderer;
    SDL_Rect windowArea;
    GameField *gameField;
    GridParams *gridParams;
    Menu *menu;
} Game;

#endif //ELETJATEK_TYPEDEFS_H
