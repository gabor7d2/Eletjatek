#include "game_event_handler.h"
#include "menu.h"
#include "menu_element.h"
#include "game_logic.h"
#include "game_display.h"
#include "file_handling.h"
#include "sdl_utils.h"

/**
 * Átméretezi a játékteret. (Csökkenti/növeli az oszlopainak vagy sorainak a számát.)
 * @param game A játék példány.
 * @param action A menü művelet ami alapján meg lesz állapítva, milyen átméretezésről van szó.
 */
void resize_game(Game *game, MenuAction action) {
    Vector2s size = game->gameField->size;

    if (action == INC_CELLS_X)
        size.x++;
    else if (action == DEC_CELLS_X)
        size.x--;
    if (action == INC_CELLS_Y)
        size.y++;
    else if (action == DEC_CELLS_Y)
        size.y--;

    if (size.x < 1) size.x = 1;
    if (size.y < 1) size.y = 1;

    resize_field(game->gameField, size);
    resize_grid_params(game->gridParams, game->gridParams->gameArea, size);
}

/**
 * Feldolgoz egy menüelem kattintást.
 * @param game A játék példány.
 * @param event Az event, ami előidézte a kattintást.
 * @param element A kattintott menüelem.
 */
void process_element_click(Game *game, SDL_Event *event, MenuElement *element) {
    switch (element->action) {
        case AUTO_STEP_TOGGLE:
            if (!game->drawing) {
                game->simRunning = !game->simRunning;
                edit_element_text(game->renderer, element, game->simRunning ? "Auto léptetés KI" : "Auto léptetés BE");
            }
            break;
        case STEP:
            if (!game->drawing && !game->simRunning)
                evolve(game->gameField);
            break;
        case CLEAR:
            if (!game->drawing && !game->simRunning)
                clear_cells(game->gameField);
            break;
        case IMPORT:
            if (!game->drawing && !game->simRunning)
                import_game("palya.dat", game->gameField);
            break;
        case EXPORT:
            if (!game->drawing && !game->simRunning)
                export_game("palya.dat", game->gameField);
            break;
        case INC_SPEED:
            if (game->simSpeedMs >= 10)
                game->simSpeedMs -= 10;
            else game->simSpeedMs = 1;
            break;
        case DEC_SPEED:
            if (game->simSpeedMs <= 996)
                game->simSpeedMs += 10;
            else game->simSpeedMs = 996;
            break;
        case INC_CELLS_X:
        case DEC_CELLS_X:
        case INC_CELLS_Y:
        case DEC_CELLS_Y:
            resize_game(game, element->action);
            break;
        /*case EDIT_FILE:
        case EDIT_SPEED:
        case EDIT_CELLS_X:
        case EDIT_CELLS_Y:
            break;*/
    }
}

void mouse_button_down(Game *game, SDL_Event *event) {
    game->cursorPos.x = event->button.x;
    game->cursorPos.y = event->button.y;

    MenuElement *element = find_element(game->menu, game->cursorPos);
    if (element != NULL) {
        element->clicked = true;
        process_element_click(game, event, element);
    }

    if (!game->simRunning && !game->drawing) {
        if (event->button.button == SDL_BUTTON_RIGHT || event->button.button == SDL_BUTTON_LEFT) {
            game->drawing = true;
            game->drawMode = event->button.button == SDL_BUTTON_LEFT ? LIVE : DEAD;
        }
    }
}

void mouse_button_up(Game *game, SDL_Event *event) {
    if (game->menu->foundElement != NULL) {
        game->menu->foundElement->clicked = false;
        find_element(game->menu, game->cursorPos);
    }

    if (game->drawing) {
        if (event->button.button == SDL_BUTTON_LEFT || event->button.button == SDL_BUTTON_RIGHT)
            change_cell(game);
        game->drawing = false;
    }
}

void mouse_motion(Game *game, SDL_Event *event) {
    if (game->menu->foundElement == NULL || !game->menu->foundElement->clicked)
        find_element(game->menu, game->cursorPos);

    if (game->drawing)
        change_cell(game);

    game->cursorPos.x = event->button.x;
    game->cursorPos.y = event->button.y;
}

void key_down(Game *game, SDL_Event *event) {

}

void text_input(Game *game, SDL_Event *event) {

}

void user_event(Game *game, SDL_Event *event) {
    // clear window
    fill_rect(game->renderer, &game->windowArea, game->gridParams->bgColor);
    // draw game area
    draw_game(game);
    // draw menu
    draw_menu(game->renderer, game->menu);
    // render
    SDL_RenderPresent(game->renderer);
}

void window_event(Game *game, SDL_Event *event) {
    // window resize
    if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        // update window area
        game->windowArea.w = (short) event->window.data1;
        game->windowArea.h = (short) event->window.data2;

        // update game area
        SDL_Rect gameArea = game->gridParams->gameArea;
        gameArea.w = (short) game->windowArea.w - MENU_WIDTH;
        gameArea.h = game->windowArea.h;

        // update menu area
        game->menu->area.x = gameArea.w;
        game->menu->area.w = game->windowArea.w - gameArea.w;
        game->menu->area.h = game->windowArea.h;

        resize_grid_params(game->gridParams, gameArea, game->gameField->size);
    }
}