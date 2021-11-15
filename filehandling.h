#ifndef ELETJATEK_FILEHANDLING_H
#define ELETJATEK_FILEHANDLING_H

#include "typedefs.h"

bool import_game(char *fileName, GameField *field);

bool export_game(char *fileName, GameField *field);

#endif //ELETJATEK_FILEHANDLING_H
