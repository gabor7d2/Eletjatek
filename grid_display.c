#include "debugmalloc.h"
#include "grid_display.h"

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
 * @param gameArea A játéktér mérete pixelben (margókkal együtt).
 * @param cells A cellák száma vízszintesen és függőlegesen.
 * @param padding A játéktér margójának nagysága pixelben.
 */
void resize_grid_params(GridParams *params, Vector2s gameArea, Vector2s cells, Vector2s padding) {
    params->cells = cells;

    Vector2s borderWidth = { .x = (short) (((double) gameArea.x / cells.x) / 12.0), .y = (short) (((double) gameArea.y / cells.y) / 12.0) };
    params->borderWidth = borderWidth;

    Vector2s newPadding = { .x = (short) (padding.x + borderWidth.x), .y = (short) (padding.y + borderWidth.y) };
    params->padding = newPadding;

    gameArea.x = (short) (gameArea.x - 2 * newPadding.x - (borderWidth.x % 2 == 0 ? 0 : 1));
    gameArea.y = (short) (gameArea.y - 2 * newPadding.y - (borderWidth.y % 2 == 0 ? 0 : 1));
    SDL_Rect newGameArea = { .x = padding.x, .y = padding.y, .w = gameArea.x, .h = gameArea.y };
    params->gameArea = newGameArea;

    Vector2d cellSize = { .x = gameArea.x / (double) cells.x, .y = gameArea.y / (double) cells.y };
    params->cellSize = cellSize;
}

/**
 * Létrehozza a grid paramétereket tartalmazó structot dinamikus memóriában.
 * @param gameArea A játéktér mérete pixelben (margókkal együtt).
 * @param cells A cellák száma vízszintesen és függőlegesen.
 * @param padding A játéktér margójának nagysága pixelben.
 * @param deadColor A halott cellák színe.
 * @param liveColor Az élő cellák színe.
 * @param borderColor A szegély színe.
 * @param bgColor A háttérszín, a margónak a színe.
 * @return A létrehozott struct példány pointere, a hívó kötelessége felszabadítani a free_grid_params() függvény hívásával.
 */
GridParams *create_grid_params(Vector2s gameArea, Vector2s cells, Vector2s padding,
                               Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor) {
    // Hely lefoglalása
    GridParams *params = (GridParams*) malloc(sizeof(GridParams));

    // Értékek előre kiszámítása és beállítása
    resize_grid_params(params, gameArea, cells, padding);
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
            boxRGBA(renderer, x * params->cellSize.x + params->padding.x, y * params->cellSize.y + params->padding.y,
                    (x + 1) * params->cellSize.x - 1 + params->padding.x, (y + 1) * params->cellSize.y - 1 + params->padding.y,
                    c->r, c->g, c->b, c->a);
        }
    }
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
    for (int x = 0; x <= params->cells.x; ++x) {
        short posX = params->cellSize.x * x + params->padding.x;
        thickLineRGBA(renderer, posX, params->padding.y, posX, (short) (params->gameArea.h + params->padding.y), params->borderWidth.x, bc->r, bc->g, bc->b, bc->a);
    }

    // Vízszintes szegélyek rajzolása
    for (int y = 0; y <= params->cells.y; ++y) {
        short posY = params->cellSize.y * y + params->padding.y;
        thickLineRGBA(renderer, params->padding.x, posY, (short) (params->gameArea.w + params->padding.x), posY, params->borderWidth.y, bc->r, bc->g, bc->b, bc->a);
    }
}