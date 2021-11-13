#ifndef ELETJATEK_GAMELOGIC_H
#define ELETJATEK_GAMELOGIC_H

#include "typedefs.h"

GameField* create_field(short sizeX, short sizeY);

void free_field(GameField *field);

void change_cell(GameField *field, GridParams *params, int x, int y, CellState state);

#endif //ELETJATEK_GAMELOGIC_H
