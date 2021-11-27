#include "menu.h"
#include "display.h"
#include "grid_display.h"
#include "game_logic.h"
#include "filehandling.h"

#define DEFAULT_WINDOW_X 1100
#define DEFAULT_WINDOW_Y 800
#define MIN_WINDOW_X 600
#define MIN_WINDOW_Y 600

#define MENU_WIDTH 300

#define DEFAULT_CELLS_X 20
#define DEFAULT_CELLS_Y 20
#define FRAMETIME_MS 20
#define DEFAULT_SIM_SPEED_MS 101

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

// Időzítő, mely FRAMETIME_MS-enként generál egy SDL_USEREVENT-et
Uint32 render_tick(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

// Időzítő, mely változtatható gyorsaságú,
// és a szimuláció következő iterációját számolja ki
Uint32 sim_tick(Uint32 ms, void *param) {
    SimData *data = (SimData *) param;
    if (*(data->running)) {
        evolve(data->gameField);
        *(data->renderNeeded) = true;
    }
    return *(data->speedMs);
}

int main(int argc, char *argv[]) {
    SDL_Rect windowArea = {.x = 0, .y = 0, .w = DEFAULT_WINDOW_X, .h = DEFAULT_WINDOW_Y};
    Vector2s cells = {.x = DEFAULT_CELLS_X, .y = DEFAULT_CELLS_Y};
    SDL_Rect gameArea = {.x = 0, .y = 0, .w = (short) windowArea.w - MENU_WIDTH, .h = windowArea.h};
    SDL_Rect menuArea = {.x = gameArea.w, .y = 0, .w = windowArea.w - gameArea.w, .h = windowArea.h};

    GridParams *gridParams = create_grid_params(gameArea, cells, 0x212121ff, 0xffb300ff, 0x424242ff, 0xfff176ff);
    GameField *gameField = create_field(cells);

    Menu *menu = create_menu(menuArea, 0x7a7a7a77);

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(windowArea.w, windowArea.h, "Game Of Life", &window, &renderer);
    SDL_SetWindowMinimumSize(window, MIN_WINDOW_X, MIN_WINDOW_Y);
    TTF_Init();

    Game game = {.renderer = renderer, .gridParams = gridParams, .gameField = gameField, .menu = menu, .windowArea = windowArea};

    TTF_Font *font = create_font("Chalkboard.ttf", 20);
    SDL_Color color = {.r = 240, .g = 60, .b = 200, .a = 255};
    SDL_Rect btnArea = {.x = 20, .y = 60, .w = 200, .h = 80};
    Button *btn = create_button(renderer, btnArea, CLICKME, "Click me!", font, &color);
    add_button(menu, btn);

    // Create and start render timer
    if (SDL_AddTimer(FRAMETIME_MS, render_tick, NULL) == 0) {
        SDL_Log("Couldn't start render timer: %s", SDL_GetError());
        exit(2);
    }

    bool renderNeeded = true;
    bool simRunning = false;
    bool drawing = false;
    CellState drawMode = LIVE;
    SDL_Point prevPos = {.x = 0, .y = 0};
    int simSpeedMs = DEFAULT_SIM_SPEED_MS;
    SDL_Event event;

    // Create simulation timer
    SimData simData = {.running = &simRunning, .renderNeeded = &renderNeeded, .speedMs = &simSpeedMs, .gameField = gameField};
    if (SDL_AddTimer(simSpeedMs, sim_tick, &simData) == 0) {
        SDL_Log("Couldn't start simulation timer: %s", SDL_GetError());
        exit(2);
    }

    Button *foundBtn;
    // Main event loop
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                prevPos.x = event.button.x;
                prevPos.y = event.button.y;
                foundBtn = find_button(menu, &prevPos);
                if (foundBtn != NULL && foundBtn->action == CLICKME) {
                    printf("clicking\n");
                }
                if (!simRunning && !drawing) {
                    if (event.button.button == SDL_BUTTON_RIGHT || event.button.button == SDL_BUTTON_LEFT) {
                        drawing = true;
                        drawMode = event.button.button == SDL_BUTTON_LEFT ? LIVE : DEAD;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (drawing) {
                    if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
                        change_cell(gameField, gridParams, prevPos, drawMode);
                        renderNeeded = true;
                        drawing = false;
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                /*if (find_button(menu, &prevPos) != NULL) {
                    printf("hovering\n");
                }*/
                if (drawing) {
                    change_cell(gameField, gridParams, prevPos, drawMode);
                    renderNeeded = true;
                }
                prevPos.x = event.motion.x;
                prevPos.y = event.motion.y;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_c:
                        if (!simRunning && !drawing) {
                            clear_cells(gameField);
                            renderNeeded = true;
                        }
                        break;
                    case SDLK_SPACE:
                        if (!drawing)
                            simRunning = !simRunning;
                        break;
                    case SDLK_UP:
                        if (simSpeedMs >= 10) simSpeedMs -= 10;
                        break;
                    case SDLK_DOWN:
                        if (simSpeedMs <= 491) simSpeedMs += 10;
                        break;
                    case SDLK_RETURN:
                        if (!drawing && !simRunning) {
                            evolve(gameField);
                            renderNeeded = true;
                        }
                        break;
                    case SDLK_e:
                        if (!drawing && !simRunning) {
                            export_game("palya.dat", gameField);
                        }
                        break;
                    case SDLK_i:
                        if (!drawing && !simRunning) {
                            import_game("palya.dat", gameField);
                            renderNeeded = true;
                        }
                        break;
                        // todo resize
                }
                break;
            case SDL_USEREVENT:
                // rendering
                if (renderNeeded) {
                    fill_rect(renderer, &windowArea, gridParams->bgColor);
                    draw_cells(renderer, gridParams, gameField);
                    draw_grid(renderer, gridParams);

                    fill_rect(renderer, &menuArea, menu->bgColor);
                    Vector2s offset = {.x = (short) menuArea.x, .y = (short) menuArea.y};
                    draw_button(renderer, btn, &offset);

                    SDL_RenderPresent(renderer);
                    renderNeeded = false;
                }
                break;
            case SDL_WINDOWEVENT:
                // window resize
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    printf("resized: %d, %d\n", event.window.data1, event.window.data2);
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    windowArea.w = (short) event.window.data1;
                    windowArea.h = (short) event.window.data2;
                    gameArea.w = (short) windowArea.w - MENU_WIDTH;
                    gameArea.h = windowArea.h;
                    menuArea.x = gameArea.w;
                    menuArea.w = windowArea.w - gameArea.w;
                    menuArea.h = windowArea.h;
                    resize_grid_params(gridParams, gameArea, cells);
                    renderNeeded = true;
                    printf("size changed: %d, %d\n", event.window.data1, event.window.data2);
                }
                break;
        }
    }

    SDL_Quit();

    // felszabadítás
    free_grid_params(gridParams);
    free_field(gameField);
    free_menu(menu);
    TTF_CloseFont(font);

    return 0;
}
