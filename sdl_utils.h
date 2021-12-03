#ifndef ELETJATEK_SDL_UTILS_H
#define ELETJATEK_SDL_UTILS_H

#include "typedefs.h"

/**
 * Inicializálja az SDL-t és létrehoz egy ablakot és annak a rendererjét.
 * @param width Ablak szélessége (pixel).
 * @param height Ablak magassága (pixel).
 * @param title Ablak neve.
 * @param pwindow A létrehozott ablak pointere milyen memóriacímre legyen írva.
 * @param prenderer A létrehozott renderer pointere milyen memóriacímre legyen írva.
 */
void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer);

/**
 * Megsemmisíti a megadott ablakot és renderert, majd meghívja az SDL_Quit() függvényt.
 * @param pwindow Az ablak pointere.
 * @param prenderer A renderer pointere.
 */
void sdl_exit(SDL_Window **pwindow, SDL_Renderer **prenderer);

/**
 * A megadott 32 bites szám R, G, B és A értékeit beleírja a cím szerint megadott 'color' struct-ba.
 * @param value Egy 32 bites szám, melynek bájtjai rendre az R, G, B, A értékeket határozzák meg.
 * @param color SDL_Color struct példányra mutató pointer, aminek az értékei át lesznek állítva.
 */
void set_color(Uint32 value, SDL_Color *color);

/**
 * A megadott értékeket beleírja a cím szerint megadott 'rect' struct-ba.
 * @param rect SDL_Rect struct példányra mutató pointer, aminek az értékei át lesznek állítva.
 */
void set_rect(int x, int y, int w, int h, SDL_Rect *rect);

/**
 * Létrehoz egy fontot a megadott betűtípus elérési út és betűméret alapján.
 * Ezen függvény hívása előtt a TTF_Init() mindenképpen történjen meg!
 * @param fontPath A betűtípus fájl elérési útja.
 * @param fontSize A betűtípus milyen mérettel kerüljön betöltésre.
 * @return A létrehozott font, a hívó kötelessége felszabadítani a TTF_CloseFont(); függvény hívásával.
 */
TTF_Font *create_font(char *fontPath, int fontSize);

/**
 * Kitölti a megadott területet a megadott színnel.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param area A terület, amit át szeretnénk színezni.
 * @param color A színezéshez használt szín.
 */
void fill_rect(SDL_Renderer *renderer, SDL_Rect *area, SDL_Color color);

/**
 * Kitölti a megadott területet a megadott színnel.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param area A terület, amit át szeretnénk színezni.
 * @param color A színezéshez használt szín.
 * @param offset A színezni kívánt terület mennyivel legyen eltolva.
 */
void fill_rect_offset(SDL_Renderer *renderer, SDL_Rect *area, SDL_Color color, Vector2s offset);

/**
 * Megmondja, hogy a 'point' rajta van-e az 'offset'-tel eltolt 'rect' területen.
 * @return Igaz, ha benne van, hamis, ha nincs benne.
 */
bool inside_rect(SDL_Rect *rect, SDL_Point *point, SDL_Rect *offset);

// A megadott stringet dinamikus területre másolja és visszaadja a terület pointerét.
char *create_string(char *str);

// A megadott számot egy dinamikus területen foglalt stringbe másolja és visszaadja a terület pointerét.
char *parse_int(int num);

#endif //ELETJATEK_SDL_UTILS_H
