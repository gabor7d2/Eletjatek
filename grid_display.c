#include "debugmalloc.h"
#include "grid_display.h"

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
 * Létrehoz egy SDL_Color példányt, ami dinamikusan van lefoglalva a memóriában.
 * @return A létrehozott struct példány, a hivó kötelessége felszabadítani.
 */
SDL_Color *create_color(Uint32 color) {
    SDL_Color *c = (SDL_Color*) malloc(sizeof(SDL_Color));
    c->r = (color >> 24) & 0xff;
    c->g = (color >> 16) & 0xff;
    c->b = (color >> 8) & 0xff;
    c->a = color & 0xff;
    return c;
}

/**
 * Újraszámolja a grid paraméterek struct értékeit a megadott új értékek használatával.
 * @param params A grid paraméterek példány.
 * @param width A játéktér szélessége pixelben.
 * @param height A játéktér magassága pixelben.
 * @param cellsX Cellák száma víszintes irányban.
 * @param cellsY Cellák száma függőleges irányban.
 * @param padding A játéktér margójának szélessége.
 */
void resize_grid_params(GridParams *params, short width, short height, short cellsX, short cellsY, short padding) {
    // Ratio kiszámolása - ez az az érték, ami megmondja, milyen vastagak legyenek az elválasztó szegélyek
    short ratio = (short) (min(width, height) / max(cellsX, cellsY));

    params->cellsX = cellsX;
    params->cellsY = cellsY;
    params->borderWidth = ceil(ratio / 12.0);
    params->padding = (short) (padding + params->borderWidth);
    params->width = (short) (width - 2 * params->padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    params->height = (short) (height - 2 * params->padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    params->cellWidth = params->width / (double) cellsX;
    params->cellHeight = params->height / (double) cellsY;
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
                               Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor) {
    // Hely lefoglalása
    GridParams *params = (GridParams*) malloc(sizeof(GridParams));

    // Értékek előre kiszámítása és beállítása
    resize_grid_params(params, width, height, cellsX, cellsY, padding);
    params->deadColor = create_color(deadColor);
    params->liveColor = create_color(liveColor);
    params->borderColor = create_color(borderColor);
    params->bgColor = create_color(bgColor);
    return params;
}

/**
 * Felszabadítja a memóriából a megadott grid paraméterek példányt.
 * @param params A grid paraméterek példány.
 */
void free_grid_params(GridParams *params) {
    if (params == NULL) return;
    free(params->deadColor);
    free(params->liveColor);
    free(params->borderColor);
    free(params->bgColor);
    free(params);
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