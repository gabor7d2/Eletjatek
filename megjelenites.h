#ifndef ELETJATEK_MEGJELENITES_H
#define ELETJATEK_MEGJELENITES_H

#include "jateklogika.h"

void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

void clear_background(SDL_Renderer *renderer, short width, short height, SDL_Color *bgcolor);

void draw_grid(SDL_Renderer *renderer, GridParams *params);

void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *gameField);

SDL_Color *create_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif //ELETJATEK_MEGJELENITES_H
