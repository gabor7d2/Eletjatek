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
 * Átméretezi a megadott játékteret.
 * @param field A játéktér.
 * @param newSize Az új méret (cellák mennyisége mindkét irányban).
 */
void resize_field(GameField *field, Vector2s newSize);

/**
 * Felszabadítja a memóriából a megadott játékteret.
 * @param field A játéktér.
 */
void free_field(GameField *field);

/**
 * Kiszámítja, hogy a jelenlegi x és y kurzor pozíción melyik cella van,
 * és megváltoztatja annak állapotát a megadott állapotra.
 * @param game A játék példány.
 */
void change_cell(Game *game);

/**
 * A játéktér következő iterációját kiszámolja a newCells tömbben,
 * majd megcseréli a cells és newCells tömböket.
 * @param field A játéktér.
 */
void evolve(GameField *field);

#endif //ELETJATEK_GAME_LOGIC_H
