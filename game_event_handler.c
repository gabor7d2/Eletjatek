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
    MenuElement *elem;
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
                import_game(search_element(game->menu, EDIT_FILE)->text->text, game->gameField);
            break;
        case EXPORT:
            if (!game->drawing && !game->simRunning)
                export_game(search_element(game->menu, EDIT_FILE)->text->text, game->gameField);
            break;
        case INC_SPEED:
            if (game->simSpeedMs > 10)
                game->simSpeedMs -= 10;
            else game->simSpeedMs = 1;
            elem = search_element(game->menu, EDIT_SPEED);
            free(elem->text->text);
            edit_element_text(game->renderer, elem, parse_int(game->simSpeedMs));
            break;
        case DEC_SPEED:
            if (game->simSpeedMs <= 986)
                game->simSpeedMs += 10;
            else game->simSpeedMs = 996;
            elem = search_element(game->menu, EDIT_SPEED);
            free(elem->text->text);
            edit_element_text(game->renderer, elem, parse_int(game->simSpeedMs));
            break;
        case INC_CELLS_X:
        case DEC_CELLS_X:
        case INC_CELLS_Y:
        case DEC_CELLS_Y:
            resize_game(game, element->action);
            elem = search_element(game->menu, EDIT_CELLS_X);
            free(elem->text->text);
            edit_element_text(game->renderer, elem, parse_int(game->gameField->size.x));
            elem = search_element(game->menu, EDIT_CELLS_Y);
            free(elem->text->text);
            edit_element_text(game->renderer, elem, parse_int(game->gameField->size.y));
            break;
        case EDIT_FILE:
        case EDIT_SPEED:
        case EDIT_CELLS_X:
        case EDIT_CELLS_Y:
            if (game->menu->selTextField == NULL) {
                game->menu->selTextField = element;
                element->selected = true;
                element->interactAlpha = 0;
            }
            break;
    }
}

void process_value_edit(Game *game, MenuElement *textField) {
    switch (textField->action) {
        case EDIT_SPEED: {
            int newSpeedMs = (int) strtol(textField->text->text, NULL, 10);
            game->simSpeedMs = newSpeedMs;
            break;
        }
        case EDIT_CELLS_X:
        case EDIT_CELLS_Y: {
            Vector2s newSize = game->gameField->size;
            short newSizeXY = (short) strtol(textField->text->text, NULL, 10);
            if (textField->action == EDIT_CELLS_X) newSize.x = newSizeXY;
            else newSize.y = newSizeXY;
            resize_field(game->gameField, newSize);
            resize_grid_params(game->gridParams, game->gridParams->gameArea, newSize);
            break;
        }
        default:
            break;
    }
}

bool only_numbers(const char *str) {
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

bool validate_input(MenuElement *textField, char *input) {
    switch (textField->action) {
        case EDIT_FILE:
            return true;
        case EDIT_SPEED:
        case EDIT_CELLS_X:
        case EDIT_CELLS_Y:
            return strlen(textField->text->text) < 3 && only_numbers(input);
        default:
            return false;
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
        if (event->button.button == SDL_BUTTON_RIGHT || event->button.button == SDL_BUTTON_LEFT)
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
    // csak akkor dolgozzuk fel, ha van kiválasztva szövegmező
    if (game->menu->selTextField == NULL) return;

    switch (event->key.keysym.sym) {
        case SDLK_BACKSPACE: {
            // forrás: infoc
            char *text = game->menu->selTextField->text->text;
            int textlen = (int) strlen(text);
            do {
                if (textlen == 0) {
                    break;
                }
                if ((text[textlen - 1] & 0x80) == 0x00) {
                    /* Egy bajt */
                    text[textlen - 1] = 0x00;
                    break;
                }
                if ((text[textlen - 1] & 0xC0) == 0x80) {
                    /* Bajt, egy tobb-bajtos szekvenciabol */
                    text[textlen - 1] = 0x00;
                    textlen--;
                }
                if ((text[textlen - 1] & 0xC0) == 0xC0) {
                    /* Egy tobb-bajtos szekvencia elso bajtja */
                    text[textlen - 1] = 0x00;
                    break;
                }
            } while (true);
            // pointer átméretezése
            text = (char *) realloc(text, sizeof(char) * (strlen(text) + 1));
            // elem szövegének frissítése
            edit_element_text(game->renderer, game->menu->selTextField, text);
            break;
        }
        case SDLK_RETURN:
            process_value_edit(game, game->menu->selTextField);
            game->menu->selTextField->selected = false;
            game->menu->selTextField = NULL;
            break;
    }
}

void text_input(Game *game, SDL_Event *event) {
    // csak akkor dolgozzuk fel, ha van kiválasztva szövegmező
    if (game->menu->selTextField == NULL) return;

    // bevitt szöveg validálása
    if (!validate_input(game->menu->selTextField, event->text.text)) return;

    char *text = game->menu->selTextField->text->text;

    // pointer átméretezése
    text = (char *) realloc(text, sizeof(char) * (strlen(text) + strlen(event->text.text) + 1));

    // új szöveg bemásolása
    strcat(text, event->text.text);

    // elem szövegének frissítése
    edit_element_text(game->renderer, game->menu->selTextField, text);
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