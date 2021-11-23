#include <stdbool.h>
#include <SDL.h>

#include "debugmalloc.h"
#include "display.h"
#include "grid_display.h"
#include "gamelogic.h"
#include "filehandling.h"

#define DEFAULT_WINDOW_X 800
#define DEFAULT_WINDOW_Y 800
#define DEFAULT_CELLS_X 20
#define DEFAULT_CELLS_Y 20
#define FRAMETIME 20
#define DEFAULT_SIM_SPEED_MS 101

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

// Időzítő, mely FRAMETIME-onként generál egy SDL_USEREVENT-et
Uint32 render_tick(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

// Időzítő, mely változtatható gyorsaságú,
// és a szimuláció következő iterációját számolja ki
Uint32 sim_tick(Uint32 ms, void *param) {
    SimData *data = (SimData*) param;
    if (*(data->running)) {
        evolve(data->gameField);
        *(data->renderNeeded) = true;
    }
    return *(data->speedMs);
}

int main(int argc, char *argv[]) {
    Vector2s windowSize = { .x = DEFAULT_WINDOW_X, .y = DEFAULT_WINDOW_Y };
    Vector2s cells = { .x = DEFAULT_CELLS_X, .y = DEFAULT_CELLS_Y };
    Vector2s padding = { .x = 10, .y = 30 };

    GridParams *gridParams = create_grid_params(windowSize, cells, padding,
                                                0x212121ff, 0xffb300ff, 0x424242ff, 0xfff176ff);
    GameField *gameField = create_field(cells.x, cells.y);

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(windowSize.x, windowSize.y, "Game Of Life", &window, &renderer);

    // Create and start render timer
    if (SDL_AddTimer(FRAMETIME, render_tick, NULL) == 0) {
        SDL_Log("Couldn't start render timer: %s", SDL_GetError());
        exit(2);
    }

    bool renderNeeded = true;
    bool simRunning = false;
    bool drawing = false;
    CellState drawMode = LIVE;
    int prevPosX = 0;
    int prevPosY = 0;
    int simSpeedMs = DEFAULT_SIM_SPEED_MS;
    SDL_Event event;

    // Create simulation timer
    SimData simData = { .running = &simRunning, .renderNeeded = &renderNeeded, .speedMs = &simSpeedMs, .gameField = gameField };
    if (SDL_AddTimer(simSpeedMs, sim_tick, &simData) == 0) {
        SDL_Log("Couldn't start simulation timer: %s", SDL_GetError());
        exit(2);
    }

    // Main event loop
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (!simRunning && !drawing) {
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
                if (drawing) {
                    if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
                        change_cell(gameField, gridParams, prevPosX, prevPosY, drawMode);
                        renderNeeded = true;
                        drawing = false;
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                if (drawing) {
                    change_cell(gameField, gridParams, prevPosX, prevPosY, drawMode);
                    renderNeeded = true;
                    prevPosX = event.motion.x;
                    prevPosY = event.motion.y;
                }
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
                        // todo resize?
                        // todo window resize?
                }
                break;
            case SDL_USEREVENT:
                if (renderNeeded) {
                    clear_background(renderer, windowSize, gridParams->bgColor);
                    draw_cells(renderer, gridParams, gameField);
                    draw_grid(renderer, gridParams);
                    SDL_RenderPresent(renderer);
                    renderNeeded = false;
                }
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    printf("resized: %d, %d\n", event.window.data1, event.window.data2);
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    windowSize.x = (short) event.window.data1;
                    windowSize.y = (short) event.window.data2;
                    resize_grid_params(gridParams, windowSize, cells, padding);
                    renderNeeded = true;
                    printf("size changed: %d, %d\n", event.window.data1, event.window.data2);
                }
                break;
        }
    }

    SDL_Quit();

    free_grid_params(gridParams);
    free_field(gameField);

    return 0;
}
