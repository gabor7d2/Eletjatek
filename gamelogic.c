#include "debugmalloc.h"
#include "gamelogic.h"

/**
 * A játéktér összes celláját halottra állítja.
 * @param field A játéktér.
 */
void clear_cells(GameField *field) {
    if (field == NULL) return;
    for (int x = 0; x < field->sizeX; ++x) {
        for (int y = 0; y < field->sizeY; ++y) {
            field->cells[y][x] = DEAD;
        }
    }
}

/**
 * Létrehoz egy megadott méretű játékteret.
 * @param sizeX A játéktér hány cella széles legyen.
 * @param sizeY A játéktér hány cella magas legyen.
 * @return A létrehozott játéktér példány pointere, a hívó kötelessége felszabadítani a free_field() függvény hívásával.
 */
GameField* create_field(short sizeX, short sizeY) {
    GameField* field = malloc(sizeof(GameField));
    field->sizeX = sizeX;
    field->sizeY = sizeY;

    if (sizeX < 0 || sizeY < 0) {
        field->cells = NULL;
        return field;
    }

    // Sor pointerek foglalása
    CellState **cells = (CellState**) malloc(sizeY * sizeof(CellState*));

    // Értékek pointerének foglalása (kilapított sor)
    cells[0] = (CellState*) malloc(sizeX * sizeY * sizeof(CellState));

    // Minden sor pointerének beállítása a kilapított sorban megfelelőre
    for (int i = 1; i < sizeY; ++i) {
        cells[i] = cells[0] + i * sizeX;
    }

    field->cells = cells;
    // Feltöltés alapállapottal
    clear_cells(field);
    return field;
}

/**
 * Felszabadítja a memóriából a megadott játékteret.
 * @param field A játéktér.
 */
void free_field(GameField *field) {
    if (field == NULL || field->cells == NULL || field->cells[0] == NULL) return;
    free(field->cells[0]);
    free(field->cells);
    free(field);
}

void change_cell(GameField *field, GridParams *params, int x, int y, CellState state) {
    if (field == NULL || params == NULL) return;

    x -= params->padding;
    y -= params->padding;

    int cellX = x / params->cellWidth;
    int cellY = y / params->cellHeight;

    if (cellX >= field->sizeX || cellY >= field->sizeY) return;

    field->cells[cellY][cellX] = state;
}