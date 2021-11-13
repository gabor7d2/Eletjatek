#ifndef ELETJATEK_DISPLAY_H
#define ELETJATEK_DISPLAY_H

#include "typedefs.h"

void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

GridParams *create_grid_params(short width, short height, short cellsX, short cellsY, short padding,
                               Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor);

void free_grid_params(GridParams *params);

void clear_background(SDL_Renderer *renderer, short width, short height, SDL_Color *bgcolor);

void draw_grid(SDL_Renderer *renderer, GridParams *params);

void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *field);

#endif //ELETJATEK_DISPLAY_H
