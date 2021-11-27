#include "menu.h"
#include "display.h"

/**
 * Létrehoz egy fontot a megadott betűtípus elérési út és betűméret alapján.
 * Ezen függvény hívása előtt a TTF_Init() mindenképpen történjen meg!
 * @return A létrehozott font, a hívó kötelessége felszabadítani a TTF_CloseFont(); függvény hívásával.
 */
TTF_Font *create_font(char *fontPath, int fontSize) {
    TTF_Font *font = TTF_OpenFont(fontPath, fontSize);
    if (font == NULL) {
        SDL_Log("Couldn't open font! %s\n", TTF_GetError());
        exit(3);
    }
    return font;
}

/**
 * Létrehoz egy Menu struct példányt a megfelelő kezdőadatokkal.
 * @param menuArea A menü területe az ablakon belül.
 * @param bgColor A menü háttérszíne.
 * @return A létrehozott struct példány pointere, a hívó kötelessége felszabadítani a free_menu() függvény hívásával.
 */
Menu *create_menu(SDL_Rect menuArea, Uint32 bgColor) {
    Menu *menu = (Menu *) malloc(sizeof(Menu));
    menu->area = menuArea;
    menu->bgColor = create_color(bgColor);
    menu->buttonCount = 0;
    menu->buttons = NULL;
    menu->textFieldCount = 0;
    menu->textFields = NULL;
    return menu;
}

/**
 * Felszabadítja a megadott menüt, és az összes benne levő gombot és szövegdobozt.
 * @param menu A menü.
 */
void free_menu(Menu *menu) {
    for (int i = 0; i < menu->buttonCount; ++i) {
        free_button(menu->buttons[i]);
    }

    // TODO free textfields

    free(menu->bgColor);
    free(menu->buttons);
    free(menu->textFields);
    free(menu);
}

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
Button *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action, char *text, TTF_Font *textFont, SDL_Color *textColor) {
    Button *btn = (Button *) malloc(sizeof(Button));
    btn->area = area;
    btn->action = action;

    SDL_Surface *surface = TTF_RenderUTF8_Blended(textFont, text, *textColor);
    SDL_Texture *surface_texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect txtArea = {.x = area.x + area.w / 2 - surface->w / 2, .y = area.y + area.h / 2 - surface->h / 2, .w = surface->w, .h = surface->h};
    Text txt = {.area = txtArea, .texture = surface_texture, .text = text, .textFont = textFont, .textColor = textColor};
    btn->text = txt;

    SDL_FreeSurface(surface);
    return btn;
}

/**
 * A rendererbe rajzolja a megadott gombot.
 * @param renderer A renderer.
 * @param button A gomb.
 * @param offset A gomb területéhez képest mennyivel legyen eltolva a gomb helye.
 */
void draw_button(SDL_Renderer *renderer, Button *button, Vector2s *offset) {
    boxRGBA(renderer,
            (short) (offset->x + button->area.x),
            (short) (offset->y + button->area.y),
            (short) (offset->x + button->area.x + button->area.w),
            (short) (offset->y + button->area.y + button->area.h),
            200, 200, 200, 127);

    boxRGBA(renderer,
            (short) (offset->x + button->area.x + 5),
            (short) (offset->y + button->area.y + 5),
            (short) (offset->x + button->area.x + button->area.w - 5),
            (short) (offset->y + button->area.y + button->area.h - 5),
            80, 80, 80, 255);

    SDL_Rect dst = {.x = offset->x + button->text.area.x, .y = offset->y + button->text.area.y, .w = button->text.area.w, .h = button->text.area.h};
    SDL_RenderCopy(renderer, button->text.texture, NULL, &dst);
}

/**
 * Hozzáadja a megadott gombot a menühöz.
 * @param menu A menü.
 * @param button A gomb.
 */
void add_button(Menu *menu, Button *button) {
    menu->buttons = (Button **) realloc(menu->buttons, sizeof(Button *) * (menu->buttonCount + 1));
    menu->buttons[menu->buttonCount++] = button;
}

/**
 * Megmondja, hogy a 'point' benne van-e az 'offset'-tel eltolt 'rect' területen.
 * @return Igaz, ha benne van, hamis, ha nincs benne.
 */
bool inside_rect(SDL_Rect *rect, SDL_Point *point, SDL_Rect *offset) {
    SDL_Rect offsetedRect = {.x = offset->x + rect->x, .y = offset->y + rect->y, .w = rect->w, .h = rect->h};
    return SDL_PointInRect(point, &offsetedRect);
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
    SDL_DestroyTexture(button->text.texture);
    free(button);
}
