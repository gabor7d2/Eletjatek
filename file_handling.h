#ifndef ELETJATEK_FILE_HANDLING_H
#define ELETJATEK_FILE_HANDLING_H

#include "typedefs.h"

bool import_game(char *fileName, GameField *field);

bool export_game(char *fileName, GameField *field);

#endif //ELETJATEK_FILE_HANDLING_H
