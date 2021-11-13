#ifndef ELETJATEK_TYPEDEFS_H
#define ELETJATEK_TYPEDEFS_H

#include <SDL.h>

// Enum a cellaállapotokhoz
typedef enum {
    LIVE, DEAD
} CellState;

// Struct a játéktér (cellák) tárolására
typedef struct {
    CellState **cells;
    // Cellák mennyisége mindkét irányban
    short sizeX, sizeY;
} GameField;

// Struct a grid paramétereknek
typedef struct {
    short width, height, cellsX, cellsY, borderWidth, padding;
    double cellWidth, cellHeight;
    SDL_Color *deadColor, *liveColor, *borderColor, *bgColor;
} GridParams;

#endif //ELETJATEK_TYPEDEFS_H
