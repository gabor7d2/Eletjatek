#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "debugmalloc.h"

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

// Enum a cellaállapotokhoz
typedef enum {
    LIVE, DEAD
} CellState;

// Struct a grid paramétereknek
typedef struct {
    short width, height, cellsX, cellsY, borderWidth, padding;
    SDL_Color *deadColor, *liveColor, *borderColor, *bgColor;
} GridParams;

// Struct a játéktér (cellák) tárolására
typedef struct {
    CellState **cells;
    // Cellák mennyisége mindkét irányban
    short sizeX, sizeY;
} GameField;

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

    short padding = (short) (params->padding + params->borderWidth - 1);
    short width = (short) (params->width - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    short height = (short) (params->height - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));

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
    short padding = (short) (params->padding + params->borderWidth - 1);
    short width = (short) (params->width - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));
    short height = (short) (params->height - 2 * padding - (params->borderWidth % 2 == 0 ? 0 : 1));

    double cellWidth = width / (double) gameField->sizeX;
    double cellHeight = height / (double) gameField->sizeY;

    for (int x = 0; x < gameField->sizeX; ++x) {
        for (int y = 0; y < gameField->sizeY; ++y) {
            SDL_Color *c = gameField->cells[y][x] == LIVE ? params->liveColor : params->deadColor;
            boxRGBA(renderer, x * cellWidth + padding, y * cellHeight + padding, (x + 1) * cellWidth - 1 + padding, (y + 1) * cellHeight - 1 + padding, c->r, c->g, c->b, c->a);
        }
    }
}

/**
 * Létrehozza a megjelenítéshez szükséges grid paramétereket tartalmazó structot.
 * @return A létrehozott struct pointere, amit a hívó kötelessége felszabadítani.
 */
/*GridParams* create_grid_params(int width, int height, int cellsX, int cellsY, int borderWidth, int padding, SDL_Color deadColor, SDL_Color liveColor, SDL_Color borderColor) {
    GridParams *gridParams = malloc(sizeof(GridParams));
    gridParams->width = width;
    gridParams->height = height;
    gridParams->cellsX = cellsX;
    gridParams->cellsY = cellsY;
    gridParams->borderWidth = borderWidth;
    gridParams->padding = padding;
    return gridParams;
}*/

/**
 * Létrehoz egy megadott méretű játékteret.
 * @param sizeX A játéktér hány cella széles legyen.
 * @param sizeY A játéktér hány cella magas legyen.
 * @return A játéktér pointere, a hívó kötelessége felszabadítani a free_field() függvény hívásával.
 */
GameField* create_field(short sizeX, short sizeY) {
    srand(time(0));

    GameField* field = malloc(sizeof(GameField));
    field->sizeX = sizeX;
    field->sizeY = sizeX;

    if (sizeX < 0 || sizeY < 0) {
        field->cells = NULL;
        return field;
    }

    // Sor pointerek foglalása
    CellState **cells = (CellState**) malloc(sizeY * sizeof(CellState*));

    // Értékek pointerének foglalása (kilapított sor)
    cells[0] = (CellState*) malloc(sizeX * sizeY * sizeof(CellState));

    // Minden sor pointerének beállítása a kilapított sorban megfelelőre
    for (int i = 1; i < sizeY; ++i) {
        cells[i] = cells[0] + i * sizeX;
    }

    // Feltöltés alapállapottal
    for (int x = 0; x < sizeX; ++x) {
        for (int y = 0; y < sizeY; ++y) {
            cells[y][x] = rand() % 2 == 0 ? DEAD : LIVE;
        }
    }

    field->cells = cells;
    return field;
}

/**
 * Felszabadítja a memóriából a megadott játékteret.
 * @param field A játéktér.
 */
void free_field(GameField *field) {
    if (field == NULL || field->cells == NULL || field->cells[0] == NULL) return;
    free(field->cells[0]);
    free(field->cells);
    free(field);
}

Uint32 timer_tick(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
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

int main(int argc, char *argv[]) {
    short WIDTH = 1000;
    short HEIGHT = 1000;
    short cellsX = 200;
    short cellsY = 200;

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(WIDTH, HEIGHT, "Game Of Life", &window, &renderer);

    SDL_Color *deadColor = create_color(145, 105, 35, 255);
    SDL_Color *liveColor = create_color(255, 162, 0, 255);
    SDL_Color *borderColor = create_color(100, 100, 100, 255);
    SDL_Color *bgColor = create_color(0, 0, 0, 255);
    // TODO gridparams dynamic?
    GridParams gridParams = { .width = WIDTH, .height = HEIGHT, .cellsX = cellsX, .cellsY = cellsY, .borderWidth = 1, .padding = 10, .deadColor = deadColor, .liveColor = liveColor, .borderColor = borderColor, .bgColor = bgColor};

    // 1004: 2*41=82 dividers, 920=23*40 cells, 2*1 padding

    SDL_TimerID id = SDL_AddTimer(20, timer_tick, NULL);

    GameField* gameField = create_field(cellsX, cellsY);

    clear_background(renderer, WIDTH, HEIGHT, bgColor);
    draw_cells(renderer, &gridParams, gameField);
    draw_grid(renderer, &gridParams);

    //lineRGBA(renderer, 0, 999, 1000, 999, 255, 0, 0, 255);
    pixelRGBA(renderer, 0, 0, 255, 0, 0, 255);
    pixelRGBA(renderer, 999, 999, 255, 0, 0, 255);
    SDL_RenderPresent(renderer);

    bool sim_running = false;
    bool drawing = false;
    CellState drawMode = LIVE;
    int prevPosX = 0;
    int prevPosY = 0;
    SDL_Event event;
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
        bool render_needed = false;

        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (!sim_running && !drawing) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        prevPosX = event.button.x;
                        prevPosY = event.button.y;
                        drawing = true;
                        drawMode = LIVE;
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        prevPosX = event.button.x;
                        prevPosY = event.button.y;
                        drawing = true;
                        drawMode = DEAD;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
                    drawing = false;
                break;
            case SDL_MOUSEMOTION:
                prevPosX = event.motion.x;
                prevPosY = event.motion.y;
                break;
            case SDL_KEYDOWN:

                break;
            case SDL_USEREVENT:

                break;
        }

        if (render_needed)
            SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    free(deadColor);
    free(liveColor);
    free(borderColor);
    free(bgColor);
    free_field(gameField);

    return 0;
}
