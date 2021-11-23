#ifndef ELETJATEK_GRID_DISPLAY_H
#define ELETJATEK_GRID_DISPLAY_H

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "typedefs.h"

void resize_grid_params(GridParams *params, Vector2s gameArea, Vector2s cells, Vector2s padding);

GridParams *create_grid_params(Vector2s gameArea, Vector2s cells, Vector2s padding,
                               Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor);

void free_grid_params(GridParams *params);

void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *field);

void draw_grid(SDL_Renderer *renderer, GridParams *params);

#endif //ELETJATEK_GRID_DISPLAY_H
