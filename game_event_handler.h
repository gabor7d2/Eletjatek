#ifndef ELETJATEK_GAME_EVENT_HANDLER_H
#define ELETJATEK_GAME_EVENT_HANDLER_H

#include "typedefs.h"

// SDL_MOUSEBUTTONDOWN event esetén meghívandó függvény.
void mouse_button_down(Game *game, SDL_Event *event);

// SDL_MOUSEBUTTONUP event esetén meghívandó függvény.
void mouse_button_up(Game *game, SDL_Event *event);

// SDL_MOUSEMOTION event esetén meghívandó függvény.
void mouse_motion(Game *game, SDL_Event *event);

// SDL_KEYDOWN event esetén meghívandó függvény.
void key_down(Game *game, SDL_Event *event);

// SDL_TEXTINPUT event esetén meghívandó függvény.
void text_input(Game *game, SDL_Event *event);

// SDL_USEREVENT event esetén meghívandó függvény.
void user_event(Game *game, SDL_Event *event);

// SDL_WINDOWEVENT event esetén meghívandó függvény.
void window_event(Game *game, SDL_Event *event);

#endif //ELETJATEK_GAME_EVENT_HANDLER_H
