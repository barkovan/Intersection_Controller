#include "../header/levels.h"
#include "../header/common.h"

void loadLevel(int levelId) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            gameMap[y][x] = TILE_GRASS;
        }
    }
    for (int i = 0; i < MAX_VEHICLES; i++) {
        vehicles[i].active = 0;
    }

    // генерация уровней
    if (levelId == 1) { // easy

        // Горизонтальная главная дорога (2 полосы: направо и налево)
        for (int x = 0; x < MAP_WIDTH; x++) {
            gameMap[9][x] = TILE_ROAD_RIGHT;
            gameMap[8][x] = TILE_ROAD_LEFT;
        }

        // Вертикальная примыкающая дорога (снизу вверх и сверху вниз)
        for (int y = 10; y < MAP_HEIGHT; y++) {
            gameMap[y][16] = TILE_ROAD_UP;
            gameMap[y][15] = TILE_ROAD_DOWN;
        }

        // Зона перекрестка
        gameMap[8][15] = TILE_INTERSECT;
        gameMap[8][16] = TILE_INTERSECT;
        gameMap[9][15] = TILE_INTERSECT;
        gameMap[9][16] = TILE_INTERSECT;

        // светофор
        gameMap[10][16] = TILE_TRAFFIC_LIGHT_GREEN; // По умолчанию зелёный

        // спавнеры машин
        gameMap[17][16] = TILE_SPAWN;     // Снизу (едут вверх к перекрестку)
        gameMap[9][0] = TILE_SPAWN;      // Слева (едут направо)
    }
}