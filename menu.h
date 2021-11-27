#ifndef ELETJATEK_MENU_H
#define ELETJATEK_MENU_H

#include "typedefs.h"

TTF_Font *create_font(char *fontPath, int fontSize);

Menu *create_menu(SDL_Rect menuArea, Uint32 bgColor);

void free_menu(Menu *menu);

void add_button(Menu *menu, Button *button);

Button *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action, char *text, TTF_Font *textFont, SDL_Color *textColor);

void draw_button(SDL_Renderer *renderer, Button *button, Vector2s *offset);

Button *find_button(Menu *menu, SDL_Point *point);

void free_button(Button *button);

#endif //ELETJATEK_MENU_H
