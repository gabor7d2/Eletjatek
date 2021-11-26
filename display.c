#include "debugmalloc.h"
#include "display.h"

/**
 * Létrehoz egy SDL_Color példányt, ami dinamikusan van lefoglalva a memóriában.
 * @return A létrehozott struct példány, a hivó kötelessége felszabadítani.
 */
SDL_Color *create_color(Uint32 color) {
    SDL_Color *c = (SDL_Color*) malloc(sizeof(SDL_Color));
    c->r = (color >> 24) & 0xff;
    c->g = (color >> 16) & 0xff;
    c->b = (color >> 8) & 0xff;
    c->a = color & 0xff;
    return c;
}

/**
 * Inicializálja az SDL-t és létrehozza a renderert és egy ablakot.
 * @param width Ablak szélessége (pixel).
 * @param height Ablak magassága (pixel).
 * @param title Ablak neve.
 * @param pwindow A létrehozott ablak pointere milyen memóriacímre legyen írva.
 * @param prenderer A létrehozott renderer pointere milyen memóriacímre legyen írva.
 */
void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer) {
    // SDL inicializálása és ablak megnyitása
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        SDL_Log("SDL couldn't be initialized: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Couldn't create renderer: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

/**
 * Kitölti a megadott területet a megadott színnel.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param area A terület, amit át szeretnénk színezni.
 * @param bgcolor A színezéshez használt szín.
 */
void fill_rect(SDL_Renderer *renderer, SDL_Rect *area, SDL_Color *bgcolor) {
    if (renderer == NULL || bgcolor == NULL) return;
    boxRGBA(renderer, (short) area->x, (short) area->y, (short) (area->x + area->w - 1), (short) (area->y + area->h - 1), bgcolor->r, bgcolor->g, bgcolor->b, bgcolor->a);
}

/**
 * Létrehoz egy Menu struct példányt a megfelelő kezdőadatokkal.
 * @param menuArea A menü területe az ablakon belül.
 * @param bgColor A menü háttérszíne.
 * @return A létrehozott struct példány pointere, a hívó kötelessége felszabadítani a free_menu() függvény hívásával.
 */
Menu *create_menu(SDL_Rect *menuArea, Uint32 bgColor) {
    Menu *menu = (Menu*) malloc(sizeof(Menu));
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
 * Létrehoz egy dinamikusan foglalt gombot a megadott paraméterekkel.
 * @param renderer A renderer.
 * @param area A gomb helye a menühöz relatívan.
 * @param action A gomb milyen műveletet végez kattintáskor.
 * @param text A gomb szövege.
 * @param textFont A gomb szövegének betűtípusa.
 * @param textColor A gomb szövegének színe.
 * @return A gombra mutató pointer, a hívó kötelessége felszabadítani a free_button() függvény hívásával.
 */
Button *create_button(SDL_Renderer *renderer, SDL_Rect area, MenuAction action, char *text, TTF_Font *textFont, SDL_Color textColor) {
    Button *btn = (Button*) malloc(sizeof(Button));
    btn->area = area;
    btn->action = action;

    SDL_Surface *surface = TTF_RenderUTF8_Blended(textFont, text, textColor);
    SDL_Texture *surface_texture = SDL_CreateTextureFromSurface(renderer, surface);
    Text txt = { .w = surface->w, .h = surface->h, .texture = surface_texture };
    btn->text = txt;

    SDL_FreeSurface(surface);
    return btn;
}

void draw_button(SDL_Renderer *renderer, Button *button, Vector2s *offset) {
    boxRGBA(renderer, offset->x + button->area.x, offset->y + button->area.y, offset->x + button->area.x + button->area.w, offset->y + button->area.y + button->area.h,
    200, 200, 200, 127);
    boxRGBA(renderer, offset->x + button->area.x + 5, offset->y + button->area.y + 5, offset->x + button->area.x + button->area.w - 5, offset->y + button->area.y + button->area.h - 5,
    80, 80, 80, 255);
    SDL_Rect dst = { .x = offset->x + button->area.x + button->area.w / 2 - button->text.w / 2, .y = offset->y + button->area.y + button->area.h / 2 - button->text.h / 2, .w = button->text.w, .h = button->text.h  };
    SDL_RenderCopy(renderer, button->text.texture, NULL, &dst);
}

void add_button(Menu *menu, Button *button) {
    menu->buttons = (Button**) realloc(menu->buttons, sizeof(Button*) * (menu->buttonCount + 1));
    menu->buttons[menu->buttonCount++] = button;
}

bool inside_rect(SDL_Rect *rect, SDL_Point *point, SDL_Rect *offset) {
    SDL_Rect offsetedRect = { .x = offset->x + rect->x, .y = offset->y + rect->y, .w = rect->w, .h = rect->h };
    return SDL_PointInRect(point, &offsetedRect);
}

Button *find_button(Menu *menu, SDL_Point *point) {
    for (int i = 0; i < menu->buttonCount; ++i) {
        if (inside_rect(&menu->buttons[i]->area, point, menu->area))
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
