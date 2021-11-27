#include "menu.h"
#include "menu_element.h"
#include "menu_text.h"
#include "sdl_utils.h"

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
    menu->elementCount = 0;
    menu->elements = NULL;
    return menu;
}

void draw_menu(SDL_Renderer *renderer, Menu *menu) {
    fill_rect(renderer, &menu->area, menu->bgColor);
    Vector2s offset = {.x = (short) menu->area.x, .y = (short) menu->area.y};

    for (int i = 0; i < menu->elementCount; ++i) {
        draw_element(renderer, menu->elements[i], offset);
    }

    for (int i = 0; i < menu->textCount; ++i) {
        draw_text(renderer, menu->texts[i], offset);
    }
}

/**
 * Felszabadítja a megadott menüt, és az összes benne levő menüelemet.
 * @param menu A menü.
 */
void free_menu(Menu *menu) {
    for (int i = 0; i < menu->textCount; ++i) {
        free_text(menu->texts[i]);
    }
    for (int i = 0; i < menu->elementCount; ++i) {
        free_element(menu->elements[i]);
    }

    free(menu->texts);
    free(menu->elements);
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
 * Hozzáadja a megadott menüelemet és a hozzá tartozó szöveget a menühöz.
 * @param menu A menü.
 * @param element A menüelem.
 */
void add_element(Menu *menu, MenuElement *element) {
    menu->elements = (MenuElement **) realloc(menu->elements, sizeof(MenuElement *) * (menu->elementCount + 1));
    menu->elements[menu->elementCount++] = element;
    add_text(menu, element->text);
}
