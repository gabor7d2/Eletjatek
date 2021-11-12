#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "debugmalloc.h"
#include "megjelenites.h"

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

/**
 * Kitölti az összes pixelt az ablakban a 0,0 saroktól a megadott width-1,height-1 sarokig a megadott színnel.
 *  A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param width A szélesség, amekkora területet szeretnénk átszínezni.
 * @param height A magasság, amekkora területet szeretnénk átszínezni.
 * @param bgcolor A színezéshez használt szín.
 */
void clear_background(SDL_Renderer *renderer, short width, short height, SDL_Color *bgcolor) {
    boxRGBA(renderer, 0, 0, (short) (width - 1), (short) (height - 1), bgcolor->r, bgcolor->g, bgcolor->b, bgcolor->a);
}

/**
 * Egy négyzetrácsot rajzol a rendererbe a megadott grid paraméterekkel.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param params A grid paraméterek, amikkel a négyzetrács pozícionálása és színezése történik.
 */
void draw_grid(SDL_Renderer *renderer, GridParams *params) {
    SDL_Color bc = *params->borderColor;
    //SDL_Color bgc = *params->bgColor;

    // Clear background
    // TODO dynamic cell drawing
    //boxRGBA(renderer, 0, 0, params->width, params->height, bgc.r, bgc.g, bgc.b, bgc.a);

    short padding = (short) (params->padding + params->borderWidth);
    short width = (short) (params->width - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    short height = (short) (params->height - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));

    printf("draw grid 1\n");

    // Draw vertical borders
    for (int x = 0; x <= params->cellsX; ++x) {
        short posX = (width / (double) params->cellsX) * x + padding;
        thickLineRGBA(renderer, posX, padding, posX, (short) (height + padding), params->borderWidth, bc.r, bc.g, bc.b, bc.a);
    }
    // Draw horizontal borders
    for (int y = 0; y <= params->cellsY; ++y) {
        short posY = (height / (double) params->cellsY) * y + padding;
        thickLineRGBA(renderer, padding, posY, (short) (width + padding), posY, params->borderWidth, bc.r, bc.g, bc.b, bc.a);
    }
    printf("draw grid 2\n");

    // Draw dead cells
    // TODO dynamic cell drawing
}

/**
 * Kirajzolja a játéktér élő és halott celláit a megadott játéktér és grid paraméterek alapján.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param params A grid paraméterek.
 * @param gameField A játéktér.
 */
void draw_cells(SDL_Renderer *renderer, GridParams *params, GameField *gameField) {
    short padding = (short) (params->padding + params->borderWidth);
    short width = (short) (params->width - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    short height = (short) (params->height - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));

    double cellWidth = width / (double) gameField->sizeX;
    double cellHeight = height / (double) gameField->sizeY;
    printf("draw cells 1\n");

    for (int x = 0; x < gameField->sizeX; ++x) {
        for (int y = 0; y < gameField->sizeY; ++y) {
            SDL_Color *c = gameField->cells[y][x] == LIVE ? params->liveColor : params->deadColor;
            boxRGBA(renderer, x * cellWidth + padding, y * cellHeight + padding, (x + 1) * cellWidth - 1 + padding, (y + 1) * cellHeight - 1 + padding, c->r, c->g, c->b, c->a);
        }
    }

    printf("draw cells 2\n");
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