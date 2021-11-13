#include <stdbool.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <math.h>
#include <time.h>

#include "debugmalloc.h"
#include "display.h"
#include "gamelogic.h"

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

// félkész dokumentáció
// modulok
// menü nélkül?
// debugmalloc.h?

Uint32 timer_tick(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

int main(int argc, char *argv[]) {
    short WIDTH = 1000;
    short HEIGHT = 1000;
    short cellsX = 50;
    short cellsY = cellsX;

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(WIDTH, HEIGHT, "Game Of Life", &window, &renderer);

    SDL_Color *deadColor = create_color(145, 105, 35, 255);
    SDL_Color *liveColor = create_color(255, 162, 0, 255);
    SDL_Color *borderColor = create_color(100, 100, 100, 255);
    SDL_Color *bgColor = create_color(0, 0, 0, 255);
    GridParams *gridParams = create_grid_params(WIDTH, HEIGHT, cellsX, cellsY, 10, deadColor, liveColor, borderColor, bgColor);

    GameField* gameField = create_field(cellsX, cellsY);

    SDL_TimerID id = SDL_AddTimer(20, timer_tick, NULL);

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
                // TODO increase polling rate/drawing speed
                if (drawing) {
                    change_cell(gameField, gridParams, prevPosX, prevPosY, drawMode);
                    render_needed = true;
                    prevPosX = event.motion.x;
                    prevPosY = event.motion.y;
                }
                break;
            case SDL_KEYDOWN:

                break;
            case SDL_USEREVENT:
                if (render_needed) {
                    clear_background(renderer, WIDTH, HEIGHT, bgColor);
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
