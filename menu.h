#ifndef ELETJATEK_MENU_H
#define ELETJATEK_MENU_H

#include "typedefs.h"

/**
 * Létrehoz egy Menu struct példányt a megfelelő kezdőadatokkal.
 * @param menuArea A menü területe az ablakon belül.
 * @param bgColor A menü háttérszíne.
 * @return A létrehozott struct példány pointere, a hívó kötelessége felszabadítani a free_menu() függvény hívásával.
 */
Menu *create_menu(SDL_Rect menuArea, Uint32 bgColor);

/**
 * A rendererbe rajzolja a menüt.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param menu A menü.
 */
void draw_menu(SDL_Renderer *renderer, Menu *menu);

/**
 * Felszabadítja a megadott menüt, és az összes benne levő menüelemet.
 * @param menu A menü.
 */
void free_menu(Menu *menu);

/**
 * Hozzáadja a megadott szöveget a menühöz.
 * @param menu A menü.
 * @param text A szöveg.
 */
void add_text(Menu *menu, Text *text);

/**
 * Hozzáadja a megadott menüelemet és a hozzá tartozó szöveget a menühöz.
 * @param menu A menü.
 * @param element A menüelem.
 */
void add_element(Menu *menu, MenuElement *element);

/**
 * Megkeresi az első menüelemet, ami a megadott képernyőkoordinátákon van
 * és a Menu struct foundElement értékét átállítja a talált elem pointerére,
 * vagy NULL-ra, ha nincs találat.
 * @param menu A menü.
 * @param point A képernyőkoordináták.
 * @return A megtalált menüelem pointere, vagy NULL, ha nincs találat.
 */
MenuElement *find_element(Menu *menu, SDL_Point point);

MenuElement *search_element(Menu *menu, MenuAction action);

#endif //ELETJATEK_MENU_H
