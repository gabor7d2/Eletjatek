#include "menu.h"
#include "menu_element.h"
#include "game_display.h"
#include "game_logic.h"
#include "sdl_utils.h"
#include "game_event_handler.h"
#include "menu_text.h"

#define DEFAULT_SIM_SPEED_MS 101
#define DEFAULT_CELLS_X 40
#define DEFAULT_CELLS_Y DEFAULT_CELLS_X

// SDL kezeléséhez használt dokumentáció: https://infoc.eet.bme.hu/sdl/

// Időzítő, mely 20ms-enként generál egy SDL_USEREVENT-et.
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
    SDL_Rect windowArea = {0, 0, 1050, 800};
    SDL_Rect gameArea = {0, 0, (short) windowArea.w - MENU_WIDTH, windowArea.h};
    SDL_Rect menuArea = {gameArea.w, 0, windowArea.w - gameArea.w, windowArea.h};

    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init(windowArea.w, windowArea.h, "Game Of Life", &window, &renderer);
    SDL_SetWindowMinimumSize(window, 600, 600);
    TTF_Init();

    GridParams *gridParams = create_grid_params(gameArea, cells, 0x212121ff, 0xffb300ff, 0x424242ff, 0xfff176ff);
    GameField *gameField = create_field(cells);
    Menu *menu = create_menu(menuArea, 0x7a7a7a77);

    Game game = {renderer, window, windowArea, gameField, gridParams, menu, DEFAULT_SIM_SPEED_MS, false, false, .cursorPos.x = 0, .cursorPos.y = 0};
    return game;
}

// Felépíti a menü elemeit, szövegeket, gombokat, szövegmezőket.
void build_menu(Game *game) {
    TTF_Font *normalFont = create_font("Chalkboard.ttf", 20);
    TTF_Font *midFont = create_font("Chalkboard.ttf", 18);
    TTF_Font *smallFont = create_font("Chalkboard.ttf", 16);

    MenuElementColors btnColors;
    set_menu_element_colors(0xcccccc7f, 0x505050ff, 0x28282800, 0x00, &btnColors);
    Uint32 textColor = 0xffffffff;
    SDL_Rect area;

    set_rect(25, 30, 200, 60, &area);
    add_element(game->menu, create_button(game->renderer, area, AUTO_STEP_TOGGLE, "Auto léptetés BE", normalFont, textColor, btnColors));

    set_rect(25, 190, 200, 60, &area);
    add_element(game->menu, create_button(game->renderer, area, STEP, "Léptetés", normalFont, textColor, btnColors));

    set_rect(25, 270, 200, 60, &area);
    add_element(game->menu, create_button(game->renderer, area, CLEAR, "Törlés", normalFont, textColor, btnColors));

    set_rect(35, 420, 80, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, IMPORT, "Import", smallFont, textColor, btnColors));

    set_rect(135, 420, 80, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, EXPORT, "Export", smallFont, textColor, btnColors));

    set_rect(50, 130, 40, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, DEC_SPEED, "-", smallFont, textColor, btnColors));

    set_rect(160, 130, 40, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, INC_SPEED, "+", smallFont, textColor, btnColors));

    set_rect(50, 520, 40, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, DEC_CELLS_X, "-", smallFont, textColor, btnColors));

    set_rect(160, 520, 40, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, INC_CELLS_X, "+", smallFont, textColor, btnColors));

    set_rect(50, 605, 40, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, DEC_CELLS_Y, "-", smallFont, textColor, btnColors));

    set_rect(160, 605, 40, 40, &area);
    add_element(game->menu, create_button(game->renderer, area, INC_CELLS_Y, "+", smallFont, textColor, btnColors));

    textColor = 0x1f1f1fff;
    set_rect(80, 95, 90, 40, &area);
    add_text(game->menu, create_text(game->renderer, area, "Sebesség", midFont, textColor));
    set_rect(80, 485, 90, 40, &area);
    add_text(game->menu, create_text(game->renderer, area, "Szélesség", midFont, textColor));
    set_rect(80, 570, 90, 40, &area);
    add_text(game->menu, create_text(game->renderer, area, "Magasság", midFont, textColor));

    MenuElementColors tfColors;
    set_menu_element_colors(0x111111ff, 0xbbbbbbff, 0xa0a0a000, 0xffffffff, &tfColors);

    set_rect(25, 370, 200, 40, &area);
    add_element(game->menu, create_text_field(game->renderer, area, EDIT_FILE, create_string("palya.dat"), smallFont, textColor, tfColors));

    set_rect(90, 130, 70, 40, &area);
    add_element(game->menu, create_text_field(game->renderer, area, EDIT_SPEED, parse_int(DEFAULT_SIM_SPEED_MS), smallFont, textColor, tfColors));

    set_rect(90, 520, 70, 40, &area);
    add_element(game->menu, create_text_field(game->renderer, area, EDIT_CELLS_X, parse_int(DEFAULT_CELLS_X), smallFont, textColor, tfColors));

    set_rect(90, 605, 70, 40, &area);
    add_element(game->menu, create_text_field(game->renderer, area, EDIT_CELLS_Y, parse_int(DEFAULT_CELLS_Y), smallFont, textColor, tfColors));
}

// Elindítja az időzítőket.
void start_timers(Game *game) {
    // Renderelési időzítő létrehozása és elindítása
    if (SDL_AddTimer(20, render_tick, NULL) == 0) {
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
