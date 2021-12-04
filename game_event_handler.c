#include "game_event_handler.h"
#include "menu.h"
#include "menu_element.h"
#include "game_display.h"
#include "game_logic.h"
#include "file_handling.h"
#include "math.h"
#include "utils.h"

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
 * Kicseréli a művelet alapján megtalált szövegmező szövegét az új szövegre.
 * A csere abból áll, hogy a menüelem jelenlegi char* szövege fel lesz szabadítva,
 * tehát ennek kötelezően dinamikusan foglaltnak kell lennie a függvény helyes
 * működéséhez.
 * @param game A játék példány.
 * @param action A keresett szövegmező művelete.
 * @param newText Az új beállítandó szöveg.
 */
void replace_text_field_text(Game *game, MenuAction action, char *newText) {
    MenuElement *element = search_element(game->menu, action);
    free(element->text->text);
    edit_element_text(game->renderer, element, newText);
}

/**
 * Feldolgoz egy menüelem kattintást a menüelem művelete alapján.
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
                import_game(search_element(game->menu, EDIT_FILE)->text->text, game->gameField);
            break;
        case EXPORT:
            if (!game->drawing && !game->simRunning)
                export_game(search_element(game->menu, EDIT_FILE)->text->text, game->gameField);
            break;
        case INC_SPEED:
            game->simSpeedMs = (int) fmax(game->simSpeedMs - 10, 1);
            replace_text_field_text(game, EDIT_SPEED, parse_int(game->simSpeedMs));
            break;
        case DEC_SPEED:
            game->simSpeedMs = (int) fmin(game->simSpeedMs + 10, 996);
            replace_text_field_text(game, EDIT_SPEED, parse_int(game->simSpeedMs));
            break;
        case INC_CELLS_X:
        case DEC_CELLS_X:
            resize_game(game, element->action);
            replace_text_field_text(game, EDIT_CELLS_X, parse_int(game->gameField->size.x));
            break;
        case INC_CELLS_Y:
        case DEC_CELLS_Y:
            resize_game(game, element->action);
            replace_text_field_text(game, EDIT_CELLS_Y, parse_int(game->gameField->size.y));
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

/**
 * Feldolgozza egy szövegmező szerkesztésének befejezését.
 * A szövegmező művelete alapján a megfelelő játékváltozót beállítja a szövegmező értékére.
 * @param game A játék példány.
 * @param textField A szerkesztett szövegmező.
 */
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

/**
 * Ellenőrzi, hogy a megadott szöveg megfelel-e az adott műveletű
 * szövegmezőnek.
 * @param textField A szövegmező.
 * @param input A szövegmező tervezett új tartalma.
 * @return Ha megfelel, igaz, ha nem, hamis.
 */
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