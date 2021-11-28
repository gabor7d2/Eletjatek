#include "menu.h"
#include "menu_element.h"
#include "menu_text.h"
#include "sdl_utils.h"

Menu *create_menu(SDL_Rect menuArea, Uint32 bgColor) {
    Menu *menu = (Menu *) malloc(sizeof(Menu));
    menu->area = menuArea;
    set_color(bgColor, &menu->bgColor);
    menu->textCount = 0;
    menu->texts = NULL;
    menu->elementCount = 0;
    menu->elements = NULL;
    menu->foundElement = NULL;
    menu->selTextField = NULL;
    return menu;
}

/*
 * Kiszámolja a menüben levő menüelemek interact színéhez használt átlátszóságot.
 * (Hover animáció)
 */
void calc_interact_alphas(Menu *menu) {
    for (int i = 0; i < menu->elementCount; ++i) {
        MenuElement *element = menu->elements[i];
        if (element == menu->foundElement) {
            if (element->interactAlpha < 120) element->interactAlpha += 15;
        } else {
            if (element->interactAlpha > 0) element->interactAlpha -= 15;
        }
    }
}

void draw_menu(SDL_Renderer *renderer, Menu *menu) {
    calc_interact_alphas(menu);

    fill_rect(renderer, &menu->area, menu->bgColor);
    Vector2s offset = {.x = (short) menu->area.x, .y = (short) menu->area.y};

    for (int i = 0; i < menu->elementCount; ++i) {
        draw_element(renderer, menu->elements[i], offset);
    }

    for (int i = 0; i < menu->textCount; ++i) {
        draw_text(renderer, menu->texts[i], offset);
    }
}

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

void add_text(Menu *menu, Text *text) {
    menu->texts = (Text **) realloc(menu->texts, sizeof(Text *) * (menu->textCount + 1));
    menu->texts[menu->textCount++] = text;
}

void add_element(Menu *menu, MenuElement *element) {
    menu->elements = (MenuElement **) realloc(menu->elements, sizeof(MenuElement *) * (menu->elementCount + 1));
    menu->elements[menu->elementCount++] = element;
    add_text(menu, element->text);
}

MenuElement *find_element(Menu *menu, SDL_Point point) {
    for (int i = 0; i < menu->elementCount; ++i) {
        if (inside_rect(&menu->elements[i]->area, &point, &menu->area)) {
            menu->foundElement = menu->elements[i];
            return menu->foundElement;
        }
    }
    menu->foundElement = NULL;
    return NULL;
}
