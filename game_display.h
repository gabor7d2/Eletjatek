#ifndef ELETJATEK_GAME_DISPLAY_H
#define ELETJATEK_GAME_DISPLAY_H

#include "typedefs.h"

void resize_grid_params(GridParams *params, SDL_Rect gameArea, Vector2s cells);

GridParams *create_grid_params(SDL_Rect gameArea, Vector2s cells, Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor);

void free_grid_params(GridParams *params);

void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *field);

void draw_grid(SDL_Renderer *renderer, GridParams *params);

#endif //ELETJATEK_GAME_DISPLAY_H
