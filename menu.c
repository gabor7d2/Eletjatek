#include "menu.h"
#include "menu_button.h"
#include "menu_text.h"

/**
 * Létrehoz egy Menu struct példányt a megfelelő kezdőadatokkal.
 * @param menuArea A menü területe az ablakon belül.
 * @param bgColor A menü háttérszíne.
 * @return A létrehozott struct példány pointere, a hívó kötelessége felszabadítani a free_menu() függvény hívásával.
 */
Menu *create_menu(SDL_Rect menuArea, Uint32 bgColor) {
    Menu *menu = (Menu *) malloc(sizeof(Menu));
    menu->area = menuArea;
    set_color(bgColor, &menu->bgColor);
    menu->textCount = 0;
    menu->texts = NULL;
    menu->buttonCount = 0;
    menu->buttons = NULL;
    menu->textFieldCount = 0;
    menu->textFields = NULL;
    return menu;
}

void draw_menu(SDL_Renderer *renderer, Menu *menu) {
    fill_rect(renderer, &menu->area, menu->bgColor);
    Vector2s offset = {.x = (short) menu->area.x, .y = (short) menu->area.y};

    for (int i = 0; i < menu->buttonCount; ++i) {
        draw_button(renderer, menu->buttons[i], offset);
    }

    for (int i = 0; i < menu->textCount; ++i) {
        draw_text(renderer, menu->texts[i], offset);
    }
}

/**
 * Felszabadítja a megadott menüt, és az összes benne levő gombot és szövegdobozt.
 * @param menu A menü.
 */
void free_menu(Menu *menu) {
    for (int i = 0; i < menu->textCount; ++i) {
        free_text(menu->texts[i]);
    }
    for (int i = 0; i < menu->buttonCount; ++i) {
        free_button(menu->buttons[i]);
    }
    // TODO free textfields

    free(menu->texts);
    free(menu->buttons);
    free(menu->textFields);
    free(menu);
}

/**
 * Hozzáadja a megadott szöveget a menühöz.
 * @param menu A menü.
 * @param text A szöveg.
 */
void add_text(Menu *menu, Text *text) {
    menu->texts = (Text **) realloc(menu->texts, sizeof(Text *) * (menu->textCount + 1));
    menu->texts[menu->textCount++] = text;
}

/**
 * Hozzáadja a megadott gombot és a hozzá tartozó szöveget a menühöz.
 * @param menu A menü.
 * @param button A gomb.
 */
void add_button(Menu *menu, Button *button) {
    menu->buttons = (Button **) realloc(menu->buttons, sizeof(Button *) * (menu->buttonCount + 1));
    menu->buttons[menu->buttonCount++] = button;
    add_text(menu, button->text);
}
