#ifndef ELETJATEK_MENU_ELEMENT_H
#define ELETJATEK_MENU_ELEMENT_H

#include "typedefs.h"

MenuElement *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action,
                           char *text, TTF_Font *textFont, Uint32 textColor, MenuElementColors colors);

MenuElement *create_text_field(SDL_Renderer *renderer, SDL_Rect area, MenuAction action,
                               char *text, TTF_Font *textFont, Uint32 textColor, MenuElementColors colors);

void edit_element_text(SDL_Renderer *renderer, MenuElement *element, char *newText);

void draw_element(SDL_Renderer *renderer, MenuElement *element, Vector2s offset);

void free_element(MenuElement *element);

#endif //ELETJATEK_MENU_ELEMENT_H
