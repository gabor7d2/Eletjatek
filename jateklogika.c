#include "debugmalloc.h"
#include "jateklogika.h"

/**
 * Létrehoz egy megadott méretű játékteret.
 * @param sizeX A játéktér hány cella széles legyen.
 * @param sizeY A játéktér hány cella magas legyen.
 * @return A játéktér pointere, a hívó kötelessége felszabadítani a free_field() függvény hívásával.
 */
GameField* create_field(short sizeX, short sizeY) {
    srand(time(0));

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

    // Feltöltés alapállapottal
    for (int x = 0; x < sizeX; ++x) {
        for (int y = 0; y < sizeY; ++y) {
            cells[y][x] = rand() % 2 == 0 ? DEAD : LIVE;
        }
    }
    printf("field created\n");

    field->cells = cells;
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
