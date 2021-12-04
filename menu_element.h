#ifndef ELETJATEK_MENU_ELEMENT_H
#define ELETJATEK_MENU_ELEMENT_H

#include "typedefs.h"

/**
 * Létrehoz egy dinamikusan foglalt gombot a megadott paraméterekkel.
 * @param renderer A renderer.
 * @param area A gomb helye a menühöz relatívan.
 * @param action A gomb milyen műveletet végez kattintáskor.
 * @param text A gomb szövege. Kötelezően nem dinamikusan foglaltnak kell lennie (konstansra mutató pointer).
 * @param textFont A gomb szövegének betűtípusa.
 * @param textColor A gomb szövegének színe.
 * @param colors A gomb színei
 * @return A gombra mutató pointer, a hívó kötelessége felszabadítani a free_menu_element() függvény hívásával.
 */
MenuElement *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action,
                           char *text, TTF_Font *textFont, Uint32 textColor, MenuElementColors colors);

/**
 * Létrehoz egy dinamikusan foglalt szövegmezőt a megadott paraméterekkel.
 * @param renderer A renderer.
 * @param area A szövegmező helye a menühöz relatívan.
 * @param action A szövegmező milyen műveletet végez kattintáskor.
 * @param text A szövegmező szövege. Kötelezően dinamikusan foglaltnak kell lennie.
 * @param textFont A szövegmező szövegének betűtípusa.
 * @param textColor A szövegmező szövegének színe.
 * @param colors A szövegmező színei
 * @return A szövegmezőre mutató pointer, a hívó kötelessége felszabadítani a free_menu_element() függvény hívásával.
 */
MenuElement *create_text_field(SDL_Renderer *renderer, SDL_Rect area, MenuAction action,
                               char *text, TTF_Font *textFont, Uint32 textColor, MenuElementColors colors);

/**
 * Szerkeszti egy menüelem szövegét. A betűtípus, a betűméret és a szöveg színe nem változik.
 * @param renderer A renderer.
 * @param element A szerkesztendő menüelem.
 * @param newText Az új szöveg.
 */
void edit_element_text(SDL_Renderer *renderer, MenuElement *element, char *newText);

/**
 * A rendererbe rajzolja a megadott menüelemet.
 * @param renderer A renderer.
 * @param element A menüelem.
 * @param offset A menüelem területéhez képest mennyivel legyen eltolva a menüelem helye.
 */
void draw_element(SDL_Renderer *renderer, MenuElement *element, Vector2s offset);

/**
 * Felszabadítja a megadott menüelemet.
 * @param element A menüelem.
 */
void free_element(MenuElement *element);

/**
 * Beállítja a 'colors' struct értékeit a megadott értékekre.
 * @param edgeColor A menüelem szélének a színe.
 * @param normalColor A menüelem háttérszíne normál állapotban.
 * @param interactColor A menüelem háttérszíne amikor a kurzor felette van, vagy le van nyomva.
 * @param selectColor A menüelem háttérszíne, amikor ki van választva (szövegmezőnél használt).
 * @param colors A struct, aminek az értékei változni fognak.
 */
void set_menu_element_colors(Uint32 edgeColor, Uint32 normalColor, Uint32 interactColor, Uint32 selectColor, MenuElementColors *colors);

#endif //ELETJATEK_MENU_ELEMENT_H
