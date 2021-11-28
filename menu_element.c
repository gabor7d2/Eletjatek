#include "menu_element.h"
#include "menu_text.h"
#include "sdl_utils.h"

MenuElement *create_menu_element(SDL_Renderer *renderer, SDL_Rect area, MenuAction action, MenuElementType type,
                           char *text, TTF_Font *textFont, Uint32 textColor, MenuElementColors colors) {
    MenuElement *btn = (MenuElement *) malloc(sizeof(MenuElement));
    btn->area = area;
    btn->action = action;
    btn->type = type;
    btn->colors = colors;
    btn->text = create_text(renderer, area, text, textFont, textColor);
    btn->interactAlpha = 0;
    btn->clicked = false;
    return btn;
}

/**
 * Létrehoz egy dinamikusan foglalt gombot a megadott paraméterekkel.
 * @param renderer A renderer.
 * @param area A gomb helye a menühöz relatívan.
 * @param action A gomb milyen műveletet végez kattintáskor.
 * @param text A gomb szövege.
 * @param textFont A gomb szövegének betűtípusa.
 * @param textColor A gomb szövegének színe.
 * @param colors A gomb színei
 * @return A gombra mutató pointer, a hívó kötelessége felszabadítani a free_menu_element() függvény hívásával.
 */
MenuElement *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action,
                           char *text, TTF_Font *textFont, Uint32 textColor, MenuElementColors colors) {
    return create_menu_element(renderer, area, action, BUTTON, text, textFont, textColor, colors);
}

/**
 * Létrehoz egy dinamikusan foglalt szövegmezőt a megadott paraméterekkel.
 * @param renderer A renderer.
 * @param area A szövegmező helye a menühöz relatívan.
 * @param action A szövegmező milyen műveletet végez kattintáskor.
 * @param text A szövegmező szövege.
 * @param textFont A szövegmező szövegének betűtípusa.
 * @param textColor A szövegmező szövegének színe.
 * @param colors A szövegmező színei
 * @return A szövegmezőre mutató pointer, a hívó kötelessége felszabadítani a free_menu_element() függvény hívásával.
 */
MenuElement *create_text_field(SDL_Renderer *renderer, SDL_Rect area, MenuAction action,
                           char *text, TTF_Font *textFont, Uint32 textColor, MenuElementColors colors) {
    return create_menu_element(renderer, area, action, TEXTFIELD, text, textFont, textColor, colors);
}

/**
 * Szerkeszti egy menüelem szövegét. A betűtípus, a betűméret és a szöveg színe nem változik.
 * @param renderer A renderer.
 * @param element A szerkesztendő menüelem.
 * @param newText Az új szöveg.
 */
void edit_element_text(SDL_Renderer *renderer, MenuElement *element, char *newText) {
    edit_text(renderer, element->text, element->area, newText);
}

/**
 * A rendererbe rajzolja a megadott menüelemet.
 * @param renderer A renderer.
 * @param element A menüelem.
 * @param offset A menüelem területéhez képest mennyivel legyen eltolva a menüelem helye.
 */
void draw_element(SDL_Renderer *renderer, MenuElement *element, Vector2s offset) {
    SDL_Rect innerArea = {element->area.x + 4, element->area.y + 4, element->area.w - 8, element->area.h - 8};
    fill_rect_offset(renderer, &element->area, element->colors.edgeColor, offset);
    fill_rect_offset(renderer, &innerArea, element->colors.normalColor, offset);

    SDL_Color interactColor = element->colors.interactColor;
    interactColor.a = element->clicked ? 200 : element->interactAlpha;

    fill_rect_offset(renderer, &innerArea, interactColor, offset);
}

/**
 * Felszabadítja a megadott menüelemet.
 * @param element A menüelem.
 */
void free_element(MenuElement *element) {
    free(element);
}