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

int main(int argc, char *argv[]) {
    short WIDTH = 1000;
    short HEIGHT = 1000;
    short cellsX = 20;
    short cellsY = 20;

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(WIDTH, HEIGHT, "Game Of Life", &window, &renderer);

    GridParams *gridParams = create_grid_params(WIDTH, HEIGHT, cellsX, cellsY, 10, 0x212121ff, 0xffb300ff, 0x424242ff, 0xfff176ff);
    GameField* gameField = create_field(cellsX, cellsY);

    if (SDL_AddTimer(20, render_tick, NULL) == 0) {
        SDL_Log("Couldn't start render timer: %s", SDL_GetError());
        exit(2);
    }

    bool render_needed = true;
    bool sim_running = false;
    bool drawing = false;
    CellState drawMode = LIVE;
    int prevPosX = 0;
    int prevPosY = 0;
    SDL_Event event;

    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
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
                if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
                    change_cell(gameField, gridParams, prevPosX, prevPosY, drawMode);
                    render_needed = true;
                    drawing = false;
                }
                break;
            case SDL_MOUSEMOTION:
                if (drawing) {
                    change_cell(gameField, gridParams, prevPosX, prevPosY, drawMode);
                    render_needed = true;
                    prevPosX = event.motion.x;
                    prevPosY = event.motion.y;
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_c:
                        if (!sim_running && !drawing) {
                            clear_cells(gameField);
                            render_needed = true;
                        }
                        break;
                }
                break;
            case SDL_USEREVENT:
                if (render_needed) {
                    clear_background(renderer, WIDTH, HEIGHT, gridParams->bgColor);
                    draw_cells(renderer, gridParams, gameField);
                    draw_grid(renderer, gridParams);
                    SDL_RenderPresent(renderer);
                    render_needed = false;
                }
                break;
        }
    }

    SDL_Quit();

    free_grid_params(gridParams);
    free_field(gameField);

    return 0;
}
