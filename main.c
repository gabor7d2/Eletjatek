#include <stdbool.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <math.h>

#include "debugmalloc.h"
#include "megjelenites.h"
#include "jateklogika.h"

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

Uint32 timer_tick(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

int min(int x, int y) {
    if (x > y) return y;
    else return x;
}

int max(int x, int y) {
    if (x > y) return x;
    else return y;
}

int main(int argc, char *argv[]) {
    short WIDTH = 1000;
    short HEIGHT = 1000;
    short cellsX = 40;
    short cellsY = cellsX;
    short ratio = (short) (min(WIDTH, HEIGHT) / max(cellsX, cellsY));
    printf("Ratio: %d\n", ratio);

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(WIDTH, HEIGHT, "Game Of Life", &window, &renderer);

    SDL_Color *deadColor = create_color(145, 105, 35, 255);
    SDL_Color *liveColor = create_color(255, 162, 0, 255);
    SDL_Color *borderColor = create_color(100, 100, 100, 255);
    SDL_Color *bgColor = create_color(0, 0, 0, 255);
    // TODO gridparams dynamic?
    GridParams gridParams = { .width = WIDTH, .height = HEIGHT, .cellsX = cellsX, .cellsY = cellsY, .borderWidth = ceil(ratio / 12.0), .padding = 10, .deadColor = deadColor, .liveColor = liveColor, .borderColor = borderColor, .bgColor = bgColor};

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
