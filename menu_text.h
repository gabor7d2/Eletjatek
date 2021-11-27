#ifndef ELETJATEK_MENU_TEXT_H
#define ELETJATEK_MENU_TEXT_H

#include "typedefs.h"

Text *create_text(SDL_Renderer *renderer, SDL_Rect area, char *text, TTF_Font *textFont, Uint32 textColor);

void edit_text(SDL_Renderer *renderer, Text *text, SDL_Rect area, char *newText);

void draw_text(SDL_Renderer *renderer, Text *text, Vector2s offset);

void free_text(Text *text);

#endif //ELETJATEK_MENU_TEXT_H
