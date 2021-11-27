#ifndef ELETJATEK_GAME_LOGIC_H
#define ELETJATEK_GAME_LOGIC_H

#include "typedefs.h"

void clear_cells(GameField *field);

GameField *create_field(Vector2s size);

void free_field(GameField *field);

void change_cell(GameField *field, GridParams *params, SDL_Point pos, CellState state);

void evolve(GameField *field);

#endif //ELETJATEK_GAME_LOGIC_H
