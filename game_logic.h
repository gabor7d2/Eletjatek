#ifndef ELETJATEK_GAME_LOGIC_H
#define ELETJATEK_GAME_LOGIC_H

#include "typedefs.h"

/**
 * A játéktér összes celláját halottra állítja.
 * @param field A játéktér.
 */
void clear_cells(GameField *field);

/**
 * Létrehoz egy megadott méretű játékteret.
 * @param size A játéktér hány cella széles és magas legyen.
 * @return A létrehozott játéktér példány pointere, a hívó kötelessége felszabadítani a free_field() függvény hívásával.
 */
GameField *create_field(Vector2s size);

/**
 * Felszabadítja a memóriából a megadott játékteret.
 * @param field A játéktér.
 */
void free_field(GameField *field);

/**
 * Kiszámítja, hogy a megadott x és y képernyőkoordinátákon melyik cella van,
 * és megváltoztatja annak állapotát a megadott állapotra.
 * @param field A játéktér.
 * @param params A grid paraméterek.
 * @param pos A változtatni kívánt cella képernyőkoordinátái.
 * @param state A változtatni kívánt cella új állapota.
 */
void change_cell(GameField *field, GridParams *params, SDL_Point pos, CellState state);

/**
 * A játéktér következő iterációját kiszámolja a newCells tömbben,
 * majd megcseréli a cells és newCells tömböket.
 * @param field A játéktér.
 */
void evolve(GameField *field);

#endif //ELETJATEK_GAME_LOGIC_H
