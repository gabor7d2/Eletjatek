#ifndef ELETJATEK_JATEKLOGIKA_H
#define ELETJATEK_JATEKLOGIKA_H

#include <SDL.h>

// Enum a cellaállapotokhoz
typedef enum {
    LIVE, DEAD
} CellState;

// Struct a grid paramétereknek
typedef struct {
    short width, height, cellsX, cellsY, borderWidth, padding;
    SDL_Color *deadColor, *liveColor, *borderColor, *bgColor;
} GridParams;

// Struct a játéktér (cellák) tárolására
typedef struct {
    CellState **cells;
    // Cellák mennyisége mindkét irányban
    short sizeX, sizeY;
} GameField;

GameField* create_field(short sizeX, short sizeY);

void free_field(GameField *field);

#endif //ELETJATEK_JATEKLOGIKA_H
