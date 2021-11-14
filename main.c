#include <stdbool.h>
#include <SDL.h>

#include "debugmalloc.h"
#include "display.h"
#include "gamelogic.h"

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

Uint32 render_tick(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

Uint32 sim_tick(Uint32 ms, void *param) {
    SimData *data = (SimData*) param;
    if (*(data->running)) {
        evolve(data->gameField);
        *(data->renderNeeded) = true;
    }
    return *(data->speedMs);
}

int main(int argc, char *argv[]) {
    short WIDTH = 1000;
    short HEIGHT = 1000;
    short cellsX = 100;
    short cellsY = 100;

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(WIDTH, HEIGHT, "Game Of Life", &window, &renderer);

    GridParams *gridParams = create_grid_params(WIDTH, HEIGHT, cellsX, cellsY, 10, 0x212121ff, 0xffb300ff, 0x424242ff, 0xfff176ff);
    GameField *gameField = create_field(cellsX, cellsY);

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
    int simSpeedMs = 251;
    SDL_Event event;

    // Create simulation timer
    SimData simData = { .running = &simRunning, .renderNeeded = &renderNeeded, .speedMs = &simSpeedMs, .gameField = gameField };
    if (SDL_AddTimer(simSpeedMs, sim_tick, &simData) == 0) {
        SDL_Log("Couldn't start simulation timer: %s", SDL_GetError());
        exit(2);
    }

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
                }
                break;
            case SDL_USEREVENT:
                if (renderNeeded) {
                    clear_background(renderer, WIDTH, HEIGHT, gridParams->bgColor);
                    draw_cells(renderer, gridParams, gameField);
                    draw_grid(renderer, gridParams);
                    SDL_RenderPresent(renderer);
                    renderNeeded = false;
                }
                break;
        }
    }

    SDL_Quit();

    free_grid_params(gridParams);
    free_field(gameField);

    return 0;
}
