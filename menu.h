#ifndef ELETJATEK_MENU_H
#define ELETJATEK_MENU_H

#include "typedefs.h"

Menu *create_menu(SDL_Rect menuArea, Uint32 bgColor);

void draw_menu(SDL_Renderer *renderer, Menu *menu);

void free_menu(Menu *menu);

void add_text(Menu *menu, Text *text);

void add_element(Menu *menu, MenuElement *element);

#endif //ELETJATEK_MENU_H
