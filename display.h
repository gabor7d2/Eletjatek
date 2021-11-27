#ifndef ELETJATEK_DISPLAY_H
#define ELETJATEK_DISPLAY_H

#include "typedefs.h"

SDL_Color *create_color(Uint32 color);

void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

void fill_rect(SDL_Renderer *renderer, SDL_Rect *area, SDL_Color *bgcolor);

#endif //ELETJATEK_DISPLAY_H
