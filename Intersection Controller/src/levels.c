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
        gameMap[3][5] = TILE_HOUSE;
        gameMap[3][26] = TILE_HOUSE_BLACK;
        gameMap[16][5] = TILE_HOUSE_BROWN;
        for (int x = 1; x < 10; x++) {
            gameMap[14][x] = TILE_SIDEWALK;
        }
        for (int x = 1; x < 3; x++) {
            for (int y = 15; y < 18; y++) {
                gameMap[y][x] = TILE_SIDEWALK;
            }
        }
        for (int x = 8; x < 10; x++) {
            for (int y = 15; y < 18; y++) {
                gameMap[y][x] = TILE_SIDEWALK;
            }
        }
        for (int y = 11; y < 18; y++) {
            gameMap[y][25] = TILE_SIDEWALK;
        }
        for (int x = 17; x < 32; x++) {
            gameMap[16][x] = TILE_SIDEWALK;
        }

        // сверху
        gameMap[6][0] = TILE_BUSH_RIGHTUP;
        for (int y = 5; y > -1; y--) {
            gameMap[y][0] = TILE_BUSH_VERRIGHT;
        }
        for (int x = 1; x < 3; x++) {
            gameMap[6][x] = TILE_BUSH_HOR;
        }
        for (int x = 9; x < 16; x++) {
            gameMap[6][x] = TILE_BUSH_HOR;
        }
        gameMap[6][16] = TILE_BUSH_RIGHTUP;
        for (int y = 5; y > -1; y--) {
            gameMap[y][16] = TILE_BUSH_VERRIGHT;
        }
        for (int x = 17; x < 24; x++) {
            gameMap[6][x] = TILE_BUSH_HOR;
        }
        for (int x = 26; x < 32; x++) {
            gameMap[6][x] = TILE_BUSH_HOR;
        }
        // слева снизу
        for (int x = 0; x < 12; x++) {
            gameMap[11][x] = TILE_BUSH_HOR;
        }
        gameMap[11][12] = TILE_BUSH_RIGHTDOWN;
        for (int y = 12; y < 18; y++) {
            gameMap[y][12] = TILE_BUSH_VERRIGHT;
        }

        gameMap[4][22] = TILE_TREE_ORANGE;
        gameMap[0][19] = TILE_TREE_GREEN;
        gameMap[1][10] = TILE_TREE_BLACKGREEN;
        gameMap[4][13] = TILE_TREE;
        gameMap[13][21] = TILE_HOUSE_BLACK2;
        gameMap[11][27] = TILE_TREE;
        gameMap[10][1] = TILE_TREE_GREEN;
        gameMap[17][17] = TILE_TREE_ORANGE;
        gameMap[16][1] = TILE_TREE_BLACKGREEN;
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

        // Дома
        gameMap[3][5] = TILE_HOUSE_BROWN;
        gameMap[3][25] = TILE_HOUSE_BLACK;
        gameMap[15][5] = TILE_HOUSE;
        gameMap[15][25] = TILE_HOUSE_BLACK2;

        // Кусты вдоль горизонтального тротуара (сверху)
        for (int x = 0; x < 12; x++) gameMap[6][x] = TILE_BUSH_HOR;
        gameMap[6][12] = TILE_BUSH_RIGHTUP;
        for (int x = 19; x < 32; x++) gameMap[6][x] = TILE_BUSH_HOR;

        // Кусты вдоль горизонтального тротуара (снизу)
        for (int x = 0; x < 12; x++) gameMap[11][x] = TILE_BUSH_HOR;
        gameMap[11][12] = TILE_BUSH_RIGHTDOWN;
        for (int x = 19; x < 32; x++) gameMap[11][x] = TILE_BUSH_HOR;

        // Кусты вдоль вертикального тротуара
        for (int y = 5; y > -1; y--) gameMap[y][12] = TILE_BUSH_VERRIGHT;
        for (int y = 12; y < 18; y++) gameMap[y][12] = TILE_BUSH_VERRIGHT;

        // Деревья (раскиданы группами)
        gameMap[0][1] = TILE_TREE_GREEN;
        gameMap[0][14] = TILE_TREE_ORANGE;
        gameMap[6][13] = TILE_TREE_BLACKGREEN;
        gameMap[13][10] = TILE_TREE_RED;
        gameMap[17][0] = TILE_TREE_RED;
        gameMap[15][30] = TILE_TREE_BLACKGREEN;
        gameMap[4][19] = TILE_TREE;
        gameMap[13][19] = TILE_TREE_ORANGE;
    }
    else if (levelId == 3) { // hard
        // 1. Главная горизонтальная дорога (на всю ширину)
        for (int x = 0; x < MAP_WIDTH; x++) {
            gameMap[9][x] = TILE_ROAD_RIGHT;
            gameMap[8][x] = TILE_ROAD_LEFT;
            gameMap[10][x] = TILE_SIDEWALK;
            gameMap[7][x] = TILE_SIDEWALK;
        }

        // 2. Первая (левая) вертикальная дорога — ПОЛНЫЙ ПЕРЕКРЕСТОК
        for (int y = 0; y < MAP_HEIGHT; y++) {
            gameMap[y][11] = TILE_ROAD_UP;
            gameMap[y][10] = TILE_ROAD_DOWN;
            // Отрисовка боковых тротуаров (только там, где нет горизонтальной дороги)
            if (y < 8 || y > 9) {
                gameMap[y][12] = TILE_SIDEWALK;
                gameMap[y][9] = TILE_SIDEWALK;
            }
        }

        // 3. Вторая (правая) вертикальная дорога — Т-ОБРАЗНЫЙ (только СВЕРХУ)
        // Рисуем дорогу только до уровня y = 7 (выше основной трассы)
        for (int y = 0; y <= 7; y++) {
            gameMap[y][23] = TILE_ROAD_UP;
            gameMap[y][22] = TILE_ROAD_DOWN;
            gameMap[y][24] = TILE_SIDEWALK;
            gameMap[y][21] = TILE_SIDEWALK;
        }
        // "Заглушаем" дорогу снизу тротуаром, чтобы получился Т-образный стык
        gameMap[10][22] = TILE_SIDEWALK;
        gameMap[10][23] = TILE_SIDEWALK;

        // 4. Зоны перекрестков
        // Левый (4-х сторонний)
        gameMap[8][10] = TILE_INTERSECT; gameMap[8][11] = TILE_INTERSECT;
        gameMap[9][10] = TILE_INTERSECT; gameMap[9][11] = TILE_INTERSECT;

        // Правый (Т-образный)
        gameMap[8][22] = TILE_INTERSECT; gameMap[8][23] = TILE_INTERSECT;
        gameMap[9][22] = TILE_INTERSECT; gameMap[9][23] = TILE_INTERSECT;

        // 5. Светофоры
        gameMap[9][9] = TILE_TRAFFIC_LIGHT_GREEN; // Перед левым перекрестком
        gameMap[8][24] = TILE_TRAFFIC_LIGHT_RED;  // Перед правым (на основном пути)

        // 6. Спавнеры (точки появления машин)
        gameMap[9][0] = TILE_SPAWN;      // Въезд слева
        gameMap[8][31] = TILE_SPAWN;     // Въезд справа
        gameMap[17][11] = TILE_SPAWN;    // Въезд снизу (только для левой дороги)
        gameMap[0][22] = TILE_SPAWN;     // Въезд сверху (для обеих дорог)

        gameMap[1][2] = TILE_HOUSE_BROWN;

        // Слева снизу: Обычный дом
        gameMap[15][2] = TILE_HOUSE;

        // Справа снизу
        gameMap[15][17] = TILE_HOUSE_BLACK;
        gameMap[15][28] = TILE_HOUSE_BLACK2;

        // Сверху по центру
        gameMap[1][14] = TILE_TREE_GREEN;
        gameMap[4][18] = TILE_TREE_ORANGE;

        gameMap[6][31] = TILE_TREE_GREEN;

        // Справа сверху
        gameMap[1][28] = TILE_TREE_RED;

        for (int x = 0; x < 7; x++) {
            gameMap[5][x] = TILE_BUSH_HOR;
        }
        gameMap[5][7] = TILE_BUSH_RIGHTUP;
        for (int y = 4; y > -1; y--) {
            gameMap[y][7] = TILE_BUSH_VERRIGHT;
        }
        gameMap[5][8] = TILE_TREE;

        for (int x = 14; x < 22; x++) {
            gameMap[11][x] = TILE_BUSH_HOR;
        }
        gameMap[11][22] = TILE_BUSH_LEFTUP;
        for (int y = 12; y < 18; y++) {
            gameMap[y][22] = TILE_BUSH_VERRIGHT;
        }
        for (int x = 23; x < 32; x++) {
            gameMap[11][x] = TILE_BUSH_HOR;
        }
    }
}