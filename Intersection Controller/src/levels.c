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
            gameMap[10][x] = TILE_SIDEWALK;
            gameMap[8][x] = TILE_ROAD_LEFT;
            gameMap[7][x] = TILE_SIDEWALK;
        }

        // Вертикальная примыкающая дорога (снизу вверх и сверху вниз)
        for (int y = 10; y < MAP_HEIGHT; y++) {
            gameMap[y][16] = TILE_ROAD_UP;
            gameMap[y][17] = TILE_SIDEWALK;
            gameMap[y][15] = TILE_ROAD_DOWN;
            gameMap[y][14] = TILE_SIDEWALK;
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

        // текстурки
        gameMap[8][5] = TILE_HOUSE;
    }
    else if (levelId == 2) { //medium
        // Создаем одну горизонтальную дорогу
        for (int x = 0; x < MAP_WIDTH; x++) {
            gameMap[8][x] = TILE_ROAD_LEFT;
            gameMap[9][x] = TILE_ROAD_RIGHT;
            gameMap[7][x] = TILE_SIDEWALK;
            gameMap[10][x] = TILE_SIDEWALK;
        }

        // Создаем одну вертикальную дорогу
        for (int y = 0; y < MAP_HEIGHT; y++) {
            gameMap[y][15] = TILE_ROAD_DOWN;
            gameMap[y][16] = TILE_ROAD_UP;
            if (gameMap[y][14] == TILE_GRASS) gameMap[y][14] = TILE_SIDEWALK;
            if (gameMap[y][17] == TILE_GRASS) gameMap[y][17] = TILE_SIDEWALK;
        }

        // Центральный перекресток
        gameMap[8][15] = TILE_INTERSECT; gameMap[8][16] = TILE_INTERSECT;
        gameMap[9][15] = TILE_INTERSECT; gameMap[9][16] = TILE_INTERSECT;

        // Два светофора
        gameMap[9][14] = TILE_TRAFFIC_LIGHT_GREEN;
        gameMap[10][16] = TILE_TRAFFIC_LIGHT_RED;

        // Три спавнера
        gameMap[9][0] = TILE_SPAWN; // Машины слева направо
        gameMap[17][16] = TILE_SPAWN; // Машины снизу вверх
        gameMap[0][15] = TILE_SPAWN; // Машины сверху вниз
    }
    else if (levelId == 3) { // hard
        // Главная горизонтальная дорога
        for (int x = 0; x < MAP_WIDTH; x++) {
            gameMap[9][x] = TILE_ROAD_RIGHT;
            gameMap[8][x] = TILE_ROAD_LEFT;
            gameMap[10][x] = TILE_SIDEWALK;
            gameMap[7][x] = TILE_SIDEWALK;
        }

        // Две пересекающие вертикальные дороги
        for (int y = 0; y < MAP_HEIGHT; y++) {
            // Первая (левая) вертикальная дорога
            gameMap[y][11] = TILE_ROAD_UP;
            gameMap[y][10] = TILE_ROAD_DOWN;
            if (gameMap[y][12] != TILE_ROAD_RIGHT && gameMap[y][12] != TILE_ROAD_LEFT) gameMap[y][12] = TILE_SIDEWALK;
            if (gameMap[y][9] != TILE_ROAD_RIGHT && gameMap[y][9] != TILE_ROAD_LEFT) gameMap[y][9] = TILE_SIDEWALK;

            // Вторая (правая) вертикальная дорога
            gameMap[y][23] = TILE_ROAD_UP;
            gameMap[y][22] = TILE_ROAD_DOWN;
            if (gameMap[y][24] != TILE_ROAD_RIGHT && gameMap[y][24] != TILE_ROAD_LEFT) gameMap[y][24] = TILE_SIDEWALK;
            if (gameMap[y][21] != TILE_ROAD_RIGHT && gameMap[y][21] != TILE_ROAD_LEFT) gameMap[y][21] = TILE_SIDEWALK;
        }

        // Зоны перекрестков
        // Левый перекресток
        gameMap[8][10] = TILE_INTERSECT; gameMap[8][11] = TILE_INTERSECT;
        gameMap[9][10] = TILE_INTERSECT; gameMap[9][11] = TILE_INTERSECT;
        // Правый перекресток
        gameMap[8][22] = TILE_INTERSECT; gameMap[8][23] = TILE_INTERSECT;
        gameMap[9][22] = TILE_INTERSECT; gameMap[9][23] = TILE_INTERSECT;

        // Два светофора
        gameMap[9][9] = TILE_TRAFFIC_LIGHT_GREEN; // Перед левым перекрестком (движение направо)
        gameMap[8][24] = TILE_TRAFFIC_LIGHT_RED;   // Перед правым перекрестком (движение налево)

        // Четыре спавнера
        gameMap[9][0] = TILE_SPAWN; // Левый край
        gameMap[8][31] = TILE_SPAWN; // Правый край
        gameMap[17][11] = TILE_SPAWN; // Низ 1-й дороги
        gameMap[0][22] = TILE_SPAWN; // Верх 2-й дороги
    }
}