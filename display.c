#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "debugmalloc.h"
#include "display.h"

/**
 * Inicializálja az SDL-t és létrehoz egy ablakot és a renderert
 * @param width Ablak szélessége (pixel).
 * @param height Ablak magassága (pixel).
 * @param title Ablak neve.
 * @param pwindow A létrehozott ablak pointere milyen memóriacímre legyen írva.
 * @param prenderer A létrehozott renderer pointere milyen memóriacímre legyen írva.
 */
void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer) {
    // SDL inicializálása és ablak megnyitása
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        SDL_Log("SDL couldn't be initialized: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (window == NULL) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Couldn't create renderer: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

/*
 * Összehasonlít két számot, majd visszatéríti a kisebbet.
 */
int min(int x, int y) {
    if (x > y) return y;
    else return x;
}

/*
 * Összehasonlít két számot, majd visszatéríti a nagyobbat.
 */
int max(int x, int y) {
    if (x > y) return x;
    else return y;
}

/**
 * Létrehozza a grid paramétereket tartalmazó structot dinamikus memóriában.
 * @param width A játéktér szélessége pixelben.
 * @param height A játéktér magassága pixelben.
 * @param cellsX Cellák száma víszintes irányban.
 * @param cellsY Cellák száma függőleges irányban.
 * @param padding A játéktér margójának szélessége.
 * @param deadColor A halott cellák színe.
 * @param liveColor Az élő cellák színe.
 * @param borderColor A szegély színe.
 * @param bgColor A háttérszín, a margónak a színe.
 * @return A létrehozott struct példány pointere, a hívó kötelessége felszabadítani a free_grid_params() függvény hívásával.
 */
GridParams *create_grid_params(short width, short height, short cellsX, short cellsY, short padding,
                               SDL_Color *deadColor, SDL_Color *liveColor, SDL_Color *borderColor, SDL_Color *bgColor) {
    GridParams *params = (GridParams*) malloc(sizeof(GridParams));
    short ratio = (short) (min(width, height) / max(cellsX, cellsY));
    params->cellsX = cellsX;
    params->cellsY = cellsY;
    params->borderWidth = ceil(ratio / 12.0);
    params->padding = (short) (padding + params->borderWidth);
    params->width = (short) (width - 2 * params->padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    params->height = (short) (height - 2 * params->padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    params->cellWidth = params->width / (double) cellsX;
    params->cellHeight = params->height / (double) cellsY;
    params->deadColor = deadColor;
    params->liveColor = liveColor;
    params->borderColor = borderColor;
    params->bgColor = bgColor;
    return params;
}

void free_grid_params(GridParams *params) {
    free(params->deadColor);
    free(params->liveColor);
    free(params->borderColor);
    free(params->bgColor);
    free(params);
}

/**
 * Kitölti az összes pixelt az ablakban a 0,0 saroktól a megadott width-1,height-1 sarokig a megadott színnel.
 *  A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param width A szélesség, amekkora területet szeretnénk átszínezni.
 * @param height A magasság, amekkora területet szeretnénk átszínezni.
 * @param bgcolor A színezéshez használt szín.
 */
void clear_background(SDL_Renderer *renderer, short width, short height, SDL_Color *bgcolor) {
    if (renderer == NULL || bgcolor == NULL) return;
    boxRGBA(renderer, 0, 0, (short) (width - 1), (short) (height - 1), bgcolor->r, bgcolor->g, bgcolor->b, bgcolor->a);
}

/**
 * Egy négyzetrácsot rajzol a rendererbe a megadott grid paraméterekkel.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param params A grid paraméterek, amikkel a négyzetrács pozícionálása és színezése történik.
 */
void draw_grid(SDL_Renderer *renderer, GridParams *params) {
    if (renderer == NULL || params == NULL) return;
    SDL_Color *bc = params->borderColor;

    // Függőleges szegélyek rajzolása
    for (int x = 0; x <= params->cellsX; ++x) {
        short posX = params->cellWidth * x + params->padding;
        thickLineRGBA(renderer, posX, params->padding, posX, (short) (params->height + params->padding), params->borderWidth, bc->r, bc->g, bc->b, bc->a);
    }

    // Vízszintes szegélyek rajzolása
    for (int y = 0; y <= params->cellsY; ++y) {
        short posY = params->cellHeight * y + params->padding;
        thickLineRGBA(renderer, params->padding, posY, (short) (params->width + params->padding), posY, params->borderWidth, bc->r, bc->g, bc->b, bc->a);
    }
}

/**
 * Kirajzolja a játéktér élő és halott celláit a megadott játéktér és grid paraméterek alapján.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param params A grid paraméterek.
 * @param field A játéktér.
 */
void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *field) {
    if (renderer == NULL || params == NULL || field == NULL) return;

    for (int x = 0; x < field->sizeX; ++x) {
        for (int y = 0; y < field->sizeY; ++y) {
            SDL_Color *c = field->cells[y][x] == LIVE ? params->liveColor : params->deadColor;
            boxRGBA(renderer, x * params->cellWidth + params->padding, y * params->cellHeight + params->padding,
                    (x + 1) * params->cellWidth - 1 + params->padding, (y + 1) * params->cellHeight - 1 + params->padding,
                    c->r, c->g, c->b, c->a);
        }
    }
}

/**
 * Létrehoz egy SDL_Color példányt, ami dinamikusan van lefoglalva a memóriában.
 * @return A létrehozott struct példány, a hivó kötelessége felszabadítani.
 */

SDL_Color *create_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Color *color = (SDL_Color*) malloc(sizeof(SDL_Color));
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
    return color;
}