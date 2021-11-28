#ifndef ELETJATEK_FILE_HANDLING_H
#define ELETJATEK_FILE_HANDLING_H

#include "typedefs.h"

/**
 * Importálja a fájlban levő pályaállapotot a megadott játéktérbe.
 * @param fileName A fájl elérési útja.
 * @param field A játéktér, amibe az importálás történik.
 * @return Igaz, ha sikeres volt a fájl megnyitása, egyébként hamis.
 */
bool import_game(char *fileName, GameField *field);

/**
 * Exportálja a játéktér jelenlegi állapotát.
 * @param fileName A fájl elérési útja.
 * @param field A játéktér, amiből az exportálás történik.
 * @return Igaz, ha sikeres volt a fájl megnyitása, egyébként hamis.
 */
bool export_game(char *fileName, GameField *field);

#endif //ELETJATEK_FILE_HANDLING_H
