#ifndef ELETJATEK_GAMELOGIC_H
#define ELETJATEK_GAMELOGIC_H

#include "typedefs.h"

void clear_cells(GameField *field);

GameField* create_field(short sizeX, short sizeY);

void free_field(GameField *field);

void change_cell(GameField *field, GridParams *params, SDL_Point pos, CellState state);

void evolve(GameField *field);

#endif //ELETJATEK_GAMELOGIC_H
