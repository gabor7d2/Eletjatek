#ifndef ELETJATEK_DISPLAY_H
#define ELETJATEK_DISPLAY_H

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include "typedefs.h"

void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

void clear_background(SDL_Renderer *renderer, Vector2s size, SDL_Color *bgcolor);

#endif //ELETJATEK_DISPLAY_H
