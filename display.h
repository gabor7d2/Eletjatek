#ifndef ELETJATEK_DISPLAY_H
#define ELETJATEK_DISPLAY_H

#include "typedefs.h"

void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

GridParams *create_grid_params(short width, short height, short cellsX, short cellsY, short padding,
                               SDL_Color *deadColor, SDL_Color *liveColor, SDL_Color *borderColor, SDL_Color *bgColor);

void free_grid_params(GridParams *params);

void clear_background(SDL_Renderer *renderer, short width, short height, SDL_Color *bgcolor);

void draw_grid(SDL_Renderer *renderer, GridParams *params);

void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *field);

SDL_Color *create_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif //ELETJATEK_DISPLAY_H
