#ifndef ELETJATEK_MENU_TEXT_H
#define ELETJATEK_MENU_TEXT_H

#include "typedefs.h"

/**
 * Létrehoz egy dinamikusan foglalt szövegelemet a megadott paraméterekkel.
 * @param renderer A renderer.
 * @param area A szöveg helye a menühöz relatívan.
 * @param text A megjelenítendő szöveg.
 * @param textFont A szöveg betűtípusa.
 * @param textColor A szöveg színe.
 * @return A szövegelemre mutató pointer, a hívó kötelessége felszabadítani a free_text() függvény hívásával.
 */
Text *create_text(SDL_Renderer *renderer, SDL_Rect area, char *text, TTF_Font *textFont, Uint32 textColor);

/**
 * Szerkeszti egy szövegelem szövegét. A betűtípus, a betűméret és a szöveg színe nem változik.
 * @param renderer A renderer.
 * @param text A szerkesztendő szövegelem.
 * @param area A szöveg helye a menühöz relatívan.
 * @param newText Az új szöveg.
 */
void edit_text(SDL_Renderer *renderer, Text *text, SDL_Rect area, char *newText);

/**
 * A rendererbe rajzolja a megadott szövegelemet.
 * @param renderer A renderer.
 * @param text A szövegelem.
 * @param offset A szövegelem területéhez képest mennyivel legyen eltolva a szövegelem helye.
 */
void draw_text(SDL_Renderer *renderer, Text *text, Vector2s offset);

/**
 * Felszabadítja a megadott szövegelemet.
 * @param text A szövegelem.
 */
void free_text(Text *text);

#endif //ELETJATEK_MENU_TEXT_H
