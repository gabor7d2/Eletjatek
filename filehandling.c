#include "debugmalloc.h"
#include "filehandling.h"
#include "gamelogic.h"

/**
 * Importálja a fájlban levő pályaállapotot a megadott játéktérbe.
 * @param filePath A fájl elérési útja.
 * @param field A játéktér, amibe az importálás történik.
 * @return Igaz, ha sikeres volt a fájl megnyitása, egyébként hamis.
 */
bool import_game(char *filePath, GameField *field) {
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) return false;

    // Játéktér törlése
    clear_cells(field);

    // Élő cellák koordinátáinak beolvasása, ameddig van adat
    int x, y;
    while (fscanf(fp, "%d:%d", &x, &y) == 2) {
        if (x >= field->sizeX || y >= field->sizeY) continue;
        field->cells[y][x] = LIVE;
    }

    fclose(fp);
    return true;
}

/**
 * Exportálja a játéktér jelenlegi állapotát.
 * @param filePath A fájl elérési útja.
 * @param field A játéktér, amiből az exportálás történik.
 * @return Igaz, ha sikeres volt a fájl megnyitása, egyébként hamis.
 */
bool export_game(char *filePath, GameField *field) {
    FILE *fp = fopen(filePath, "w");
    if (fp == NULL) return false;

    // Élő cellák koordinátáinak kiírása
    for (int x = 0; x < field->sizeX; ++x) {
        for (int y = 0; y < field->sizeY; ++y) {
            if (field->cells[y][x])
                fprintf(fp, "%d:%d ", x, y);
        }
    }

    fclose(fp);
    return true;
}
