#ifndef ELETJATEK_GRID_DISPLAY_H
#define ELETJATEK_GRID_DISPLAY_H

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "typedefs.h"

GridParams *create_grid_params(short width, short height, short cellsX, short cellsY, short padding,
                               Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor);

void free_grid_params(GridParams *params);

void draw_grid(SDL_Renderer *renderer, GridParams *params);

void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *field);

#endif //ELETJATEK_GRID_DISPLAY_H
