#include "menu.h"
#include "menu_element.h"
#include "game_display.h"
#include "game_logic.h"
#include "sdl_utils.h"
#include "game_event_handler.h"

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

// Időzítő, mely FRAMETIME_MS-enként generál egy SDL_USEREVENT-et.
Uint32 render_tick(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

// Változtatható gyorsaságú időzítő,
// mely a szimuláció következő iterációját számolja ki.
Uint32 sim_tick(Uint32 ms, void *param) {
    Game *game = (Game *) param;
    if (game->simRunning)
        evolve(game->gameField);
    return game->simSpeedMs;
}

// Létrehozza a program futásához szükséges adatstruktúrákat, és inicializálja az SDL-t.
Game init() {
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

    Game game = {renderer, window, windowArea, gameField, gridParams, menu, DEFAULT_SIM_SPEED_MS, false, false, .cursorPos.x = 0, .cursorPos.y = 0};
    return game;
}

// Felépíti a menü elemeit, szövegeket, gombokat, szövegmezőket.
void build_menu(Game *game) {
    TTF_Font *font = create_font("Chalkboard.ttf", 20);
    Uint32 textColor = 0xee70ccff;
    MenuElementColors colors;
    set_menu_element_colors(0xcccccc7f, 0x505050ff, 0x28282800, 0xffffffff, &colors);
    SDL_Rect btnArea;
    set_rect(20, 60, 200, 80, &btnArea);
    add_element(game->menu, create_button(game->renderer, btnArea, CLICKME, "Click me!", font, textColor, colors));
    set_rect(20, 160, 200, 80, &btnArea);
    add_element(game->menu, create_button(game->renderer, btnArea, CLICKME, "Click me!", font, textColor, colors));
    set_rect(20, 260, 200, 80, &btnArea);
    add_element(game->menu, create_button(game->renderer, btnArea, CLICKME, "Click me!", font, textColor, colors));
}

// Elindítja az időzítőket.
void start_timers(Game *game) {
    // Renderelési időzítő létrehozása és elindítása
    if (SDL_AddTimer(FRAMETIME_MS, render_tick, NULL) == 0) {
        SDL_Log("FATAL: Couldn't start render timer: %s", SDL_GetError());
        exit(2);
    }

    // Szimulációs időzítő létrehozása és elindítása
    if (SDL_AddTimer(game->simSpeedMs, sim_tick, game) == 0) {
        SDL_Log("FATAL: Couldn't start simulation timer: %s", SDL_GetError());
        exit(2);
    }
}

// Továbbítja a kapott event-et az event handler megfelelő függvényéhez.
void forward_event(Game *game, SDL_Event *event) {
    switch (event->type) {
        case SDL_MOUSEBUTTONDOWN:
            mouse_button_down(game, event);
            break;
        case SDL_MOUSEBUTTONUP:
            mouse_button_up(game, event);
            break;
        case SDL_MOUSEMOTION:
            mouse_motion(game, event);
            break;
        case SDL_KEYDOWN:
            key_down(game, event);
            break;
        case SDL_TEXTINPUT:
            text_input(game, event);
            break;
        case SDL_USEREVENT:
            user_event(game, event);
            break;
        case SDL_WINDOWEVENT:
            window_event(game, event);
            break;
    }
}

// Felszabadítja a program memóriafoglalásait.
void end(Game *game) {
    // TODO free fonts
    sdl_exit(&game->window, &game->renderer);

    // felszabadítás
    free_grid_params(game->gridParams);
    free_field(game->gameField);
    free_menu(game->menu);
}

int main(int argc, char *argv[]) {
    Game game = init();
    build_menu(&game);
    start_timers(&game);

    // Main event loop
    SDL_Event event;
    while (SDL_WaitEvent(&event) && event.type != SDL_QUIT) {
        forward_event(&game, &event);
    }

    end(&game);
    return 0;
}
