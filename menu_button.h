#ifndef ELETJATEK_MENU_BUTTON_H
#define ELETJATEK_MENU_BUTTON_H

#include "typedefs.h"

Button *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action, char *text, TTF_Font *textFont, Uint32 textColor);

void edit_button_text(SDL_Renderer *renderer, Button *button, char *newText);

void draw_button(SDL_Renderer *renderer, Button *button, Vector2s offset);

Button *find_button(Menu *menu, SDL_Point *point);

void free_button(Button *button);

#endif //ELETJATEK_MENU_BUTTON_H
