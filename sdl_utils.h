#ifndef ELETJATEK_SDL_UTILS_H
#define ELETJATEK_SDL_UTILS_H

#include "typedefs.h"

void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

void set_color(Uint32 value, SDL_Color *color);

TTF_Font *create_font(char *fontPath, int fontSize);

void fill_rect(SDL_Renderer *renderer, SDL_Rect *area, SDL_Color color);

bool inside_rect(SDL_Rect *rect, SDL_Point *point, SDL_Rect *offset);

#endif //ELETJATEK_SDL_UTILS_H
