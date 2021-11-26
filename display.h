#ifndef ELETJATEK_DISPLAY_H
#define ELETJATEK_DISPLAY_H

#include "typedefs.h"

SDL_Color *create_color(Uint32 color);

void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

void fill_rect(SDL_Renderer *renderer, SDL_Rect *area, SDL_Color *bgcolor);

Menu *create_menu(SDL_Rect *menuArea, Uint32 bgColor);

void free_menu(Menu *menu);

TTF_Font *create_font(char *fontPath, int fontSize);

void add_button(Menu *menu, Button *button);

Button *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action, char *text, TTF_Font *textFont, SDL_Color textColor);

void draw_button(SDL_Renderer *renderer, Button *button, Vector2s *offset);

Button *find_button(Menu *menu, SDL_Point *point);

void free_button(Button *button);

#endif //ELETJATEK_DISPLAY_H
