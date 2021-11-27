#include "menu_button.h"
#include "menu_text.h"

/**
 * Létrehoz egy dinamikusan foglalt gombot a megadott paraméterekkel.
 * @param renderer A renderer.
 * @param area A gomb helye a menühöz relatívan.
 * @param action A gomb milyen műveletet végez kattintáskor.
 * @param text A gomb szövege.
 * @param textFont A gomb szövegének betűtípusa.
 * @param textColor A gomb szövegének színe.
 * @return A gombra mutató pointer, a hívó kötelessége felszabadítani a free_button() függvény hívásával.
 */
Button *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action, char *text, TTF_Font *textFont, Uint32 textColor) {
    Button *btn = (Button *) malloc(sizeof(Button));
    btn->area = area;
    btn->action = action;
    btn->text = create_text(renderer, area, text, textFont, textColor);
    btn->hovered = false;
    btn->clicked = false;
    return btn;
}

void edit_button_text(SDL_Renderer *renderer, Button *button, char *newText) {
    edit_text(renderer, button->text, button->area, newText);
}

/**
 * A rendererbe rajzolja a megadott gombot.
 * @param renderer A renderer.
 * @param button A gomb.
 * @param offset A gomb területéhez képest mennyivel legyen eltolva a gomb helye.
 */
void draw_button(SDL_Renderer *renderer, Button *button, Vector2s offset) {
    boxRGBA(renderer,
            (short) (offset.x + button->area.x),
            (short) (offset.y + button->area.y),
            (short) (offset.x + button->area.x + button->area.w),
            (short) (offset.y + button->area.y + button->area.h),
            200, 200, 200, 127);

    boxRGBA(renderer,
            (short) (offset.x + button->area.x + 5),
            (short) (offset.y + button->area.y + 5),
            (short) (offset.x + button->area.x + button->area.w - 5),
            (short) (offset.y + button->area.y + button->area.h - 5),
            80, 80, 80, 255);
}

/**
 * Megkeresi az első gombot, ami a megadott képernyőkoordinátákon van.
 * @param menu A menü.
 * @param point A képernyőkoordináták.
 * @return Az első megtalált gomb, vagy NULL, ha nincs találat.
 */
Button *find_button(Menu *menu, SDL_Point *point) {
    for (int i = 0; i < menu->buttonCount; ++i) {
        if (inside_rect(&menu->buttons[i]->area, point, &menu->area))
            return menu->buttons[i];
    }
    return NULL;
}

/**
 * Felszabadítja a megadott gombot.
 * @param button A gomb.
 */
void free_button(Button *button) {
    free(button);
}