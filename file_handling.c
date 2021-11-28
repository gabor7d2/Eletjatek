#include "file_handling.h"
#include "game_logic.h"

bool import_game(char *filePath, GameField *field) {
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) return false;

    // Játéktér törlése
    clear_cells(field);

    // Élő cellák koordinátáinak beolvasása, ameddig van adat
    int x, y;
    while (fscanf(fp, "%d:%d", &x, &y) == 2) {
        if (x >= field->size.x || y >= field->size.y) continue;
        field->cells[y][x] = LIVE;
    }

    fclose(fp);
    return true;
}

bool export_game(char *filePath, GameField *field) {
    FILE *fp = fopen(filePath, "w");
    if (fp == NULL) return false;

    // Élő cellák koordinátáinak kiírása
    for (int x = 0; x < field->size.x; ++x) {
        for (int y = 0; y < field->size.y; ++y) {
            if (field->cells[y][x])
                fprintf(fp, "%d:%d ", x, y);
        }
    }

    fclose(fp);
    return true;
}
