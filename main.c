#include <stdbool.h>
#include <SDL.h>

#include "debugmalloc.h"
#include "display.h"
#include "grid_display.h"
#include "gamelogic.h"
#include "filehandling.h"

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

// Időzítő, mely 20ms-enként generál egy SDL_USEREVENT-et
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
    short windowWidth = 800;
    short windowHeight = 800;
    short cellsX = 50;
    short cellsY = 50;
    short padding = 10;

    GridParams *gridParams = create_grid_params(windowWidth, windowHeight, cellsX, cellsY, padding,
                                                0x212121ff, 0xffb300ff, 0x424242ff, 0xfff176ff);
    GameField *gameField = create_field(cellsX, cellsY);

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(windowWidth, windowHeight, "Game Of Life", &window, &renderer);

    // Create and start render timer
    if (SDL_AddTimer(20, render_tick, NULL) == 0) {
        SDL_Log("Couldn't start render timer: %s", SDL_GetError());
        exit(2);
    }

    bool renderNeeded = true;
    bool simRunning = false;
    bool drawing = false;
    CellState drawMode = LIVE;
    int prevPosX = 0;
    int prevPosY = 0;
    int simSpeedMs = 101;
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
                    clear_background(renderer, windowWidth, windowHeight, gridParams->bgColor);
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
                    windowWidth = (short) event.window.data1;
                    windowHeight = (short) event.window.data2;
                    resize_grid_params(gridParams, windowWidth, windowHeight, cellsX, cellsY, padding);
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
