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
 * Dinamikusan létrehoz egy 2D-s CellState tömböt.
 * @param sizeX A tömb vízszintes mérete.
 * @param sizeY A tömb függőleges mérete.
 * @return A létrehozott tömb pointere.
 */
CellState **create_2D_array(short sizeX, short sizeY) {
    if (sizeX < 0 || sizeY < 0) {
        return NULL;
    }

    // Sor pointerek foglalása
    CellState **cells = (CellState**) malloc(sizeY * sizeof(CellState*));

    // Értékek pointerének foglalása (kilapított sor)
    cells[0] = (CellState*) malloc(sizeX * sizeY * sizeof(CellState));

    // Minden sor pointerének beállítása a kilapított sorban megfelelőre
    for (int i = 1; i < sizeY; ++i) {
        cells[i] = cells[0] + i * sizeX;
    }

    return cells;
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
    field->cells = create_2D_array(sizeX, sizeY);
    field->newCells = create_2D_array(sizeX, sizeY);

    // Feltöltés alapállapottal
    clear_cells(field);
    return field;
}

/**
 * Felszabadítja a memóriából a megadott játékteret.
 * @param field A játéktér.
 */
void free_field(GameField *field) {
    if (field == NULL || field->cells == NULL || field->cells[0] == NULL ||
    field->newCells == NULL || field->newCells[0] == NULL) return;
    free(field->cells[0]);
    free(field->cells);
    free(field->newCells[0]);
    free(field->newCells);
    free(field);
}

/**
 * Kiszámítja, hogy a megadott x és y képernyőkoordinátákon melyik cella van,
 * és megváltoztatja annak állapotát a megadott állapotra.
 * @param field A játéktér.
 * @param params A grid paraméterek.
 * @param pos A változtatni kívánt cella képernyőkoordinátái.
 * @param state A változtatni kívánt cella új állapota.
 */
void change_cell(GameField *field, GridParams *params, SDL_Point pos, CellState state) {
    if (field == NULL || params == NULL) return;

    // Négyzetrácson kívüli kattintás
    if (pos.x < params->padding.x || pos.y < params->padding.y) return;

    pos.x -= params->padding.x;
    pos.y -= params->padding.y;

    int cellX = pos.x / params->cellSize.x;
    int cellY = pos.y / params->cellSize.y;

    // Négyzetrácson kívüli kattintás
    if (cellX >= field->sizeX || cellY >= field->sizeY) return;

    field->cells[cellY][cellX] = state;
}

/**
 * A játéktér egy cellájának lekérdezése átfordulással,
 * vagyis ha -1-et kérdezünk, akkor átugrik a pálya másik végére
 * és az lesz a lekérdezett cella, másik irányú túlindexelésnél szintúgy.
 * @param field A játéktér.
 * @param x A kérdezett cella x indexe. Lehet -1 vagy játéktér mérete értékű is.
 * @param y A kérdezett cella y indexe. Lehet -1 vagy játéktér mérete értékű is.
 * @return A lekérdezett cellaérték.
 */
CellState get_cell(GameField *field, int x, int y) {
    if (x == -1) x = field->sizeX - 1;
    if (y == -1) y = field->sizeY - 1;
    if (x == field->sizeX) x = 0;
    if (y == field->sizeY) y = 0;
    return field->cells[y][x];
}

/**
 * Megszámolja, a megadott indexű cellának mennyi élő szomszédja van a 8-ból.
 * @param field A játéktér.
 * @param x A kérdezett cella x indexe.
 * @param y A kérdezett cella y indexe.
 * @return Élő szomszédok száma. (min 0, max 8)
 */
int get_neighbor_count(GameField *field, int x, int y) {
    int count = 0;
    if (get_cell(field, x - 1, y - 1)) count++;
    if (get_cell(field, x - 1, y)) count++;
    if (get_cell(field, x - 1, y + 1)) count++;
    if (get_cell(field, x, y - 1)) count++;
    if (get_cell(field, x, y + 1)) count++;
    if (get_cell(field, x + 1, y - 1)) count++;
    if (get_cell(field, x + 1, y)) count++;
    if (get_cell(field, x + 1, y + 1)) count++;
    return count;
}

/**
 * A játéktér következő iterációját kiszámolja a newCells tömbben,
 * majd megcseréli a cells és newCells tömböket.
 * @param field A játéktér.
 */
void evolve(GameField *field) {
    if (field == NULL) return;

    for (int x = 0; x < field->sizeX; ++x) {
        for (int y = 0; y < field->sizeY; ++y) {
            int live_neighbors = get_neighbor_count(field, x, y);
            if ((live_neighbors == 2 && field->cells[y][x] == LIVE) || live_neighbors == 3)
                field->newCells[y][x] = LIVE;
            else
                field->newCells[y][x] = DEAD;
        }
    }
    CellState **old = field->cells;
    field->cells = field->newCells;
    field->newCells = old;
}