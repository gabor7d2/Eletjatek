#include "game_logic.h"
#include "math.h"

void clear_cells(GameField *field) {
    if (field == NULL) return;
    for (int x = 0; x < field->size.x; ++x) {
        for (int y = 0; y < field->size.y; ++y) {
            field->cells[y][x] = DEAD;
        }
    }
}

/**
 * Dinamikusan létrehoz egy 2D-s CellState tömböt és minden celláját alapállapotra (DEAD) állítja.
 * @param sizeX A tömb vízszintes mérete.
 * @param sizeY A tömb függőleges mérete.
 * @return A létrehozott tömb pointere.
 */
CellState **create_2D_array(short sizeX, short sizeY) {
    if (sizeX < 0 || sizeY < 0) {
        return NULL;
    }

    // Sor pointerek foglalása
    CellState **cells = (CellState **) malloc(sizeY * sizeof(CellState *));

    // Értékek pointerének foglalása (kilapított sor)
    cells[0] = (CellState *) malloc(sizeX * sizeY * sizeof(CellState));

    // Minden sor pointerének beállítása a kilapított sorban megfelelőre
    for (int i = 1; i < sizeY; ++i) {
        cells[i] = cells[0] + i * sizeX;
    }

    for (int x = 0; x < sizeX; ++x) {
        for (int y = 0; y < sizeY; ++y) {
            cells[y][x] = DEAD;
        }
    }

    return cells;
}

GameField *create_field(Vector2s size) {
    GameField *field = malloc(sizeof(GameField));
    field->cells = NULL;
    field->newCells = NULL;
    resize_field(field, size);
    return field;
}

void resize_field(GameField *field, Vector2s newSize) {
    CellState **newCells = create_2D_array(newSize.x, newSize.y);

    if (field->cells != NULL) {
        // copy previous cell data
        short minX = (short) fmin(field->size.x, newSize.x);
        short minY = (short) fmin(field->size.y, newSize.y);
        for (int x = 0; x < minX; ++x) {
            for (int y = 0; y < minY; ++y) {
                newCells[y][x] = field->cells[y][x];
            }
        }

        free(field->cells[0]);
        free(field->cells);
        free(field->newCells[0]);
        free(field->newCells);
    }

    field->size = newSize;
    field->cells = newCells;
    field->newCells = create_2D_array(newSize.x, newSize.y);
}

void free_field(GameField *field) {
    if (field == NULL || field->cells == NULL || field->cells[0] == NULL ||
        field->newCells == NULL || field->newCells[0] == NULL)
        return;
    free(field->cells[0]);
    free(field->cells);
    free(field->newCells[0]);
    free(field->newCells);
    free(field);
}

void change_cell(Game *game) {
    GameField *field = game->gameField;
    GridParams *params = game->gridParams;
    SDL_Point pos = game->cursorPos;

    if (field == NULL || params == NULL) return;

    // Négyzetrácson kívüli kattintás
    if (pos.x < params->padding.x || pos.y < params->padding.y) return;

    pos.x -= params->padding.x;
    pos.y -= params->padding.y;

    int cellX = (int) (pos.x / params->cellSize.x);
    int cellY = (int) (pos.y / params->cellSize.y);

    // Négyzetrácson kívüli kattintás
    if (cellX >= field->size.x || cellY >= field->size.y) return;

    field->cells[cellY][cellX] = game->drawMode;
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
    if (x == -1) x = field->size.x - 1;
    if (y == -1) y = field->size.y - 1;
    if (x == field->size.x) x = 0;
    if (y == field->size.y) y = 0;
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
    for (int i = x - 1; i <= x + 1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            // Középső cellát nem kell vizsgálni
            if (i == x && j == y) continue;
            if (get_cell(field, i, j)) count++;
        }
    }
    return count;
}

void evolve(GameField *field) {
    if (field == NULL) return;

    for (int x = 0; x < field->size.x; ++x) {
        for (int y = 0; y < field->size.y; ++y) {
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