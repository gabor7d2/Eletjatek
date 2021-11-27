#include "menu_text.h"

Text *create_text(SDL_Renderer *renderer, SDL_Rect area, char *text, TTF_Font *textFont, Uint32 textColor) {
    Text *txt = (Text*) malloc(sizeof(Text));
    txt->textFont = textFont;
    txt->texture = NULL;
    set_color(textColor, &txt->textColor);
    edit_text(renderer, txt, area, text);

    return txt;
}

void edit_text(SDL_Renderer *renderer, Text *text, SDL_Rect area, char *newText) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended(text->textFont, newText, text->textColor);
    SDL_Texture *surface_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect txtArea = {.x = area.x + area.w / 2 - surface->w / 2, .y = area.y + area.h / 2 - surface->h / 2, .w = surface->w, .h = surface->h};
    text->area = txtArea;
    text->text = newText;
    SDL_DestroyTexture(text->texture);
    text->texture = surface_texture;
}

void draw_text(SDL_Renderer *renderer, Text *text, Vector2s offset) {
    SDL_Rect dst = {.x = offset.x + text->area.x, .y = offset.y + text->area.y, .w = text->area.w, .h = text->area.h};
    SDL_RenderCopy(renderer, text->texture, NULL, &dst);
}

void free_text(Text *text) {
    SDL_DestroyTexture(text->texture);
    free(text);
}