#include "game_display.h"
#include "sdl_utils.h"

#define MIN_PADDING 10

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
 * Újraszámolja a grid paraméterek struct értékeit a megadott új értékek használatával.
 * @param params A grid paraméterek példány.
 * @param gameArea A játéktér mérete pixelben (margókkal együtt).
 * @param cells A cellák száma vízszintesen és függőlegesen.
 */
void resize_grid_params(GridParams *params, SDL_Rect gameArea, Vector2s cells) {
    params->gameArea = gameArea;
    params->cells = cells;

    // Szegély vastagságának kiszámolása a játéktér mérete és a cellák száma alapján
    short borderWidthValue = (short) ceil((min(gameArea.w, gameArea.h) / (double) max(cells.x, cells.y)) / 12.0);
    Vector2s borderWidth = {.x = borderWidthValue, .y = borderWidthValue};
    params->borderWidth = borderWidth;

    // Margó kiszámolása a játéktér mérete és a cellák száma alapján,
    // hogy a cellák ugyanolyan szélesek legyenek, mint magasak
    Vector2s padding;
    if ((gameArea.w / (double) cells.x) > (gameArea.h / (double) cells.y)) {
        padding.y = MIN_PADDING;
        padding.x = (short) ((gameArea.w - ((gameArea.h - 2 * padding.y) / (double) cells.y) * cells.x) / 2);
        padding.x = (short) max(MIN_PADDING, padding.x);
    } else {
        padding.x = MIN_PADDING;
        padding.y = (short) ((gameArea.h - ((gameArea.w - 2 * padding.x) / (double) cells.x) * cells.y) / 2);
        padding.y = (short) max(MIN_PADDING, padding.y);
    }

    // Szegély vastagság beleszámolása a margóba
    padding.x = (short) (padding.x + borderWidth.x);
    padding.y = (short) (padding.y + borderWidth.y);
    params->padding = padding;

    // Négyzetháló területének kiszámolása
    SDL_Rect gridArea = {.x = padding.x, .y = padding.y};
    gridArea.w = (short) (gameArea.w - 2 * padding.x - (borderWidth.x % 2 == 0 ? 0 : 1));
    gridArea.h = (short) (gameArea.h - 2 * padding.y - (borderWidth.y % 2 == 0 ? 0 : 1));
    params->gridArea = gridArea;

    // Cellaméret kiszámolása
    Vector2d cellSize = {.x = gridArea.w / (double) cells.x, .y = gridArea.h / (double) cells.y};
    params->cellSize = cellSize;
}

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
GridParams *create_grid_params(SDL_Rect gameArea, Vector2s cells,
                               Uint32 deadColor, Uint32 liveColor, Uint32 borderColor, Uint32 bgColor) {
    // Hely lefoglalása
    GridParams *params = (GridParams *) malloc(sizeof(GridParams));

    // Értékek előre kiszámítása és beállítása
    resize_grid_params(params, gameArea, cells);
    set_color(deadColor, &params->deadColor);
    set_color(liveColor, &params->liveColor);
    set_color(borderColor, &params->borderColor);
    set_color(bgColor, &params->bgColor);
    return params;
}

/**
 * Felszabadítja a memóriából a megadott grid paraméterek példányt.
 * @param params A grid paraméterek példány.
 */
void free_grid_params(GridParams *params) {
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

    for (int x = 0; x < field->size.x; ++x) {
        for (int y = 0; y < field->size.y; ++y) {
            SDL_Color c = field->cells[y][x] == LIVE ? params->liveColor : params->deadColor;
            boxRGBA(renderer,
                    (short) (x * params->cellSize.x + params->padding.x),
                    (short) (y * params->cellSize.y + params->padding.y),
                    (short) ((x + 1) * params->cellSize.x - 1 + params->padding.x),
                    (short) ((y + 1) * params->cellSize.y - 1 + params->padding.y),
                    c.r, c.g, c.b, c.a);
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
    SDL_Color bc = params->borderColor;

    // Függőleges szegélyek rajzolása
    for (int x = 0; x <= params->cells.x; ++x) {
        short posX = (short) (params->cellSize.x * x + params->padding.x);
        thickLineRGBA(renderer, posX, params->padding.y, posX, (short) (params->gridArea.h + params->padding.y), params->borderWidth.x, bc.r, bc.g, bc.b, bc.a);
    }

    // Vízszintes szegélyek rajzolása
    for (int y = 0; y <= params->cells.y; ++y) {
        short posY = (short) (params->cellSize.y * y + params->padding.y);
        thickLineRGBA(renderer, params->padding.x, posY, (short) (params->gridArea.w + params->padding.x), posY, params->borderWidth.y, bc.r, bc.g, bc.b, bc.a);
    }
}