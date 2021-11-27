#include "sdl_utils.h"

/**
 * Inicializálja az SDL-t és létrehoz egy ablakot és annak a rendererjét.
 * @param width Ablak szélessége (pixel).
 * @param height Ablak magassága (pixel).
 * @param title Ablak neve.
 * @param pwindow A létrehozott ablak pointere milyen memóriacímre legyen írva.
 * @param prenderer A létrehozott renderer pointere milyen memóriacímre legyen írva.
 */
void sdl_init(int width, int height, char *title, SDL_Window **pwindow, SDL_Renderer **prenderer) {
    // SDL inicializálása és ablak megnyitása
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        SDL_Log("FATAL: SDL couldn't be initialized: %s", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        SDL_Log("FATAL: Couldn't create window: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("FATAL: Couldn't create renderer: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

/**
 * A megadott 32 bites szám R, G, B és A értékeit beleírja a
 * cím szerint megadott 'color' struct-ba
 * @param value Egy 32 bites szám, melynek bájtjai rendre az R, G, B, A értékeket határozzák meg.
 * @param color SDL_Color struct példányra mutató pointer, aminek az értékei át lesznek állítva.
 */
void set_color(Uint32 value, SDL_Color *color) {
    color->r = (value >> 24) & 0xff;
    color->g = (value >> 16) & 0xff;
    color->b = (value >> 8) & 0xff;
    color->a = value & 0xff;
}

/**
 * Létrehoz egy fontot a megadott betűtípus elérési út és betűméret alapján.
 * Ezen függvény hívása előtt a TTF_Init() mindenképpen történjen meg!
 * @return A létrehozott font, a hívó kötelessége felszabadítani a TTF_CloseFont(); függvény hívásával.
 */
TTF_Font *create_font(char *fontPath, int fontSize) {
    TTF_Font *font = TTF_OpenFont(fontPath, fontSize);
    if (font == NULL) {
        SDL_Log("FATAL: Couldn't open font! %s\n", TTF_GetError());
        exit(3);
    }
    return font;
}

/**
 * Kitölti a megadott területet a megadott színnel.
 * A renderert szükséges renderelésre meghívni a függvény visszatérte után.
 * @param renderer A renderer.
 * @param area A terület, amit át szeretnénk színezni.
 * @param bgcolor A színezéshez használt szín.
 */
void fill_rect(SDL_Renderer *renderer, SDL_Rect *area, SDL_Color color) {
    if (renderer == NULL) return;
    boxRGBA(renderer, (short) area->x, (short) area->y, (short) (area->x + area->w - 1), (short) (area->y + area->h - 1), color.r, color.g, color.b, color.a);
}

/**
 * Megmondja, hogy a 'point' rajta van-e az 'offset'-tel eltolt 'rect' területen.
 * @return Igaz, ha benne van, hamis, ha nincs benne.
 */
bool inside_rect(SDL_Rect *rect, SDL_Point *point, SDL_Rect *offset) {
    SDL_Rect offsetedRect = {.x = offset->x + rect->x, .y = offset->y + rect->y, .w = rect->w, .h = rect->h};
    return SDL_PointInRect(point, &offsetedRect);
}