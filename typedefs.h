#ifndef ELETJATEK_TYPEDEFS_H
#define ELETJATEK_TYPEDEFS_H

#include <SDL.h>

// Enum a cellaállapotokhoz
typedef enum {
    DEAD, LIVE
} CellState;

// Struct a játéktér (cellák) tárolására
typedef struct {
    // Jelenlegi cellák és új cellák
    // (az új cellák arraybe kerülnek a következő iteráció cellaértékei majd a 2 fel lesz cserélve)
    CellState **cells, **newCells;
    // Cellák mennyisége mindkét irányban
    short sizeX, sizeY;
} GameField;

// Struct a grid paramétereknek
typedef struct {
    short width, height, cellsX, cellsY, borderWidth, padding;
    double cellWidth, cellHeight;
    SDL_Color *deadColor, *liveColor, *borderColor, *bgColor;
} GridParams;

// Struct a szimulációhoz szükséges adatok mozgatására
typedef struct {
    bool *running, *renderNeeded;
    int *speedMs;
    GameField *gameField;
} SimData;

#endif //ELETJATEK_TYPEDEFS_H
