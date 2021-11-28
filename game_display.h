#ifndef ELETJATEK_GAME_DISPLAY_H
#define ELETJATEK_GAME_DISPLAY_H

#include "typedefs.h"

/**
 * Létrehozza a grid paramétereket tartalmazó structot dinamikus memóriában.
 * @param gameArea A játéktér mérete pixelben (margókkal együtt).
 * @param cells A cellák száma vízszintesen és függőlegesen.
 * @param deadColor A halott cellák színe.
 * @param liveColor Az élő cellák színe.
 * @param borderColor A szegély színe.
 * @param bgColor A háttérszín, a margónak a színe.
 * @return A létrehozott struct példány pointere, a hívó kötelessége felszabadítani a free_grid_params() függvény hívásával.
 */
GridParams *create_grid_params(SDL_Rect gameArea, Vector2s cells, Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor);

/**
 * Újraszámolja a grid paraméterek struct értékeit a megadott új értékek használatával.
 * @param params A grid paraméterek példány.
 * @param gameArea A játéktér mérete pixelben (margókkal együtt).
 * @param cells A cellák száma vízszintesen és függőlegesen.
 */
void resize_grid_params(GridParams *params, SDL_Rect gameArea, Vector2s cells);

/**
 * Felszabadítja a memóriából a megadott grid paraméterek példányt.
 * @param params A grid paraméterek példány.
 */
void free_grid_params(GridParams *params);

/**
 * Kirajzolja a játéktér celláit és a köztük levő szegélyeket.
 * Hívása előtt szükséges törölni a játéktér korábbi tartalmát, és
 * a renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param params A grid paraméterek.
 * @param field A játéktér.
 */
void draw_game(SDL_Renderer *renderer, GridParams *params, GameField *field);

#endif //ELETJATEK_GAME_DISPLAY_H
