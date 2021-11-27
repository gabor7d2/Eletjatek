#include "menu.h"
#include "menu_element.h"
#include "game_display.h"
#include "game_logic.h"
#include "file_handling.h"
#include "sdl_utils.h"

#define DEFAULT_WINDOW_X 1050
#define DEFAULT_WINDOW_Y 800
#define MIN_WINDOW_X 600
#define MIN_WINDOW_Y 600

#define MENU_WIDTH 250

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

void setup() {

}

void exit() {

}

int main(int argc, char *argv[]) {
    // TODO split main
    Vector2s cells = {DEFAULT_CELLS_X, DEFAULT_CELLS_Y};
    SDL_Rect windowArea = {0, 0, DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y};
    SDL_Rect gameArea = {0, 0, (short) windowArea.w - MENU_WIDTH, windowArea.h};
    SDL_Rect menuArea = {gameArea.w, 0, windowArea.w - gameArea.w, windowArea.h};

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(windowArea.w, windowArea.h, "Game Of Life", &window, &renderer);
    SDL_SetWindowMinimumSize(window, MIN_WINDOW_X, MIN_WINDOW_Y);
    TTF_Init();

    GridParams *gridParams = create_grid_params(gameArea, cells, 0x212121ff, 0xffb300ff, 0x424242ff, 0xfff176ff);
    GameField *gameField = create_field(cells);
    Menu *menu = create_menu(menuArea, 0x7a7a7a77);

    Game game = {.renderer = renderer, .gridParams = gridParams, .gameField = gameField, .menu = menu, .windowArea = windowArea};

    TTF_Font *font = create_font("Chalkboard.ttf", 20);
    Uint32 textColor = 0xee70ccff;
    // TODO unify color format
    MenuElementColors colors = {200, 200, 200, 127,
                                80, 80, 80, 255,
                                20, 20, 20, 0};
    SDL_Rect btnArea;
    set_rect(20, 60, 200, 80, &btnArea);
    add_element(menu, create_button(renderer, btnArea, CLICKME, "Click me!", font, textColor, colors));

    // Create and start render timer
    if (SDL_AddTimer(FRAMETIME_MS, render_tick, NULL) == 0) {
        SDL_Log("FATAL: Couldn't start render timer: %s", SDL_GetError());
        exit(2);
    }

    bool renderNeeded = true;
    bool simRunning = false;
    int simSpeedMs = DEFAULT_SIM_SPEED_MS;

    bool drawing = false;
    CellState drawMode = LIVE;
    SDL_Point cursorPos = {.x = 0, .y = 0};

    SDL_Event event;

    // Create simulation timer
    SimData simData = {.running = &simRunning, .renderNeeded = &renderNeeded, .speedMs = &simSpeedMs, .gameField = gameField};
    if (SDL_AddTimer(simSpeedMs, sim_tick, &simData) == 0) {
        SDL_Log("FATAL: Couldn't start simulation timer: %s", SDL_GetError());
        exit(2);
    }

    MenuElement *foundElement;
    // Main event loop
    // TODO separate event handler file
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                cursorPos.x = event.button.x;
                cursorPos.y = event.button.y;
                foundElement = find_element(menu, &cursorPos);
                if (foundElement != NULL && foundElement->action == CLICKME) {
                    edit_element_text(renderer, foundElement, "Clicked :-)");
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
                        change_cell(gameField, gridParams, cursorPos, drawMode);
                        renderNeeded = true;
                        drawing = false;
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                /*if (find_button(menu, &cursorPos) != NULL) {
                    printf("hovering\n");
                }*/
                if (drawing) {
                    change_cell(gameField, gridParams, cursorPos, drawMode);
                    renderNeeded = true;
                }
                cursorPos.x = event.motion.x;
                cursorPos.y = event.motion.y;
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
                    draw_menu(renderer, menu);

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
                    menu->area.x = gameArea.w;
                    menu->area.w = windowArea.w - gameArea.w;
                    menu->area.h = windowArea.h;
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
