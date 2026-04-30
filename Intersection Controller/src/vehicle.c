#include "../header/vehicle.h"

#include <math.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>


// ========== ФИЗИКА И ЛОГИКА МАШИН ==========

// Проверяет, можно ли ехать из клетки (gx,gy) в направлении (dx,dy) – чтобы не выехать на встречку
bool isDirectionSafe(int gx, int gy, int dx, int dy) {
    int cx = gx + dx;
    int cy = gy + dy;

    while (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
        int tile = gameMap[cy][cx];

        if (tile == TILE_INTERSECT || tile == TILE_TRAFFIC_LIGHT_GREEN) {
            cx += dx;
            cy += dy;
            continue;
        }

        if (dx == 1 && tile == TILE_ROAD_RIGHT) return true;
        if (dx == -1 && tile == TILE_ROAD_LEFT)  return true;
        if (dy == -1 && tile == TILE_ROAD_UP)    return true;
        if (dy == 1 && tile == TILE_ROAD_DOWN)  return true;
        if (tile == TILE_SPAWN)                  return true;
        if (tile == TILE_TRAFFIC_LIGHT_GREEN)    return true;

        return false;
    }
    return false;
}

// Обновляет положение и скорость всех машин
void updateVehicles(float dt) {
    // Настраиваем габариты машины (в пикселях)
    float carLength = 60.0f;     // Полная длина с запасом (для расчёта ДТП и дистанции)
    float halfCarLength = carLength / 2; // От центра до бампера (чтобы капот не торчал на перекрестке)

    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (!vehicles[i].active) continue;
        Vehicle* v = &vehicles[i];

        int currGx = (int)(v->x / 40.0f);
        int currGy = (int)(v->y / 40.0f);

        // 1. ПОИСК СВЕТОФОРА (Смотрим на несколько клеток вперед)
        int mustStopForRed = 0;
        float distToStopLine = 10000.0f;
        int lookAheadCells = 5; // Просматриваем до 5 клеток (200 пикселей) вперед

        for (int step = 1; step <= lookAheadCells; step++) {
            int checkGx = currGx + v->dirX * step;
            int checkGy = currGy + v->dirY * step;

            if (checkGx >= 0 && checkGx < MAP_WIDTH && checkGy >= 0 && checkGy < MAP_HEIGHT) {
                int tile = gameMap[checkGy][checkGx];
                if (tile == TILE_TRAFFIC_LIGHT_RED || tile == TILE_TRAFFIC_LIGHT_YELLOW) {
                    mustStopForRed = 1;

                    // Считаем дистанцию до края клетки светофора минус длина капота
                    float stopLine = 0.0f;
                    float dist = 0.0f;

                    if (v->dirX > 0) {
                        stopLine = checkGx * 40.0f; // Левый край клетки
                        dist = stopLine - (v->x + halfCarLength);
                    }
                    else if (v->dirX < 0) {
                        stopLine = (checkGx + 1) * 40.0f; // Правый край клетки
                        dist = (v->x - halfCarLength) - stopLine;
                    }
                    else if (v->dirY > 0) {
                        stopLine = checkGy * 40.0f; // Верхний край клетки
                        dist = stopLine - (v->y + halfCarLength);
                    }
                    else if (v->dirY < 0) {
                        stopLine = (checkGy + 1) * 40.0f; // Нижний край клетки
                        dist = (v->y - halfCarLength) - stopLine;
                    }

                    // Если чуть-чуть переехали стоп-линию (до 5 пикселей) - стоим на месте
                    if (dist > -5.0f && dist < distToStopLine) {
                        distToStopLine = (dist > 0.0f) ? dist : 0.0f;
                    }
                    break; // Нашли ближайший светофор, дальше не смотрим
                }
            }
        }

        // 2. РАССТОЯНИЕ ДО БЛИЖАЙШЕЙ МАШИНЫ ВПЕРЕДИ
        float distToCar = 10000.0f;
        for (int j = 0; j < MAX_VEHICLES; j++) {
            if (i == j || !vehicles[j].active) continue;
            Vehicle* other = &vehicles[j];
            float dx = other->x - v->x;
            float dy = other->y - v->y;

            // Проверяем, что машина едет в том же ряду перед нами
            if ((v->dirX > 0 && dx > 0 && fabsf(dy) < 15.0f) ||
                (v->dirX < 0 && dx < 0 && fabsf(dy) < 15.0f) ||
                (v->dirY > 0 && dy > 0 && fabsf(dx) < 15.0f) ||
                (v->dirY < 0 && dy < 0 && fabsf(dx) < 15.0f)) {

                // Вычитаем длину машины и оставляем зазор 5 пикселей
                float dist = sqrtf(dx * dx + dy * dy) - carLength - 5.0f;
                if (dist < distToCar) distToCar = (dist > 0.0f) ? dist : 0.0f;
            }
        }

        // 3. ЖЕЛАЕМАЯ СКОРОСТЬ
        float desiredSpeed = VEHICLE_MAX_SPEED;
        float obstacleDist = distToCar;
        if (mustStopForRed && distToStopLine < obstacleDist) {
            obstacleDist = distToStopLine;
        }

        // Плавное торможение, если препятствие близко
        if (obstacleDist < 200.0f) {
            float maxSafe = sqrtf(2.0f * VEHICLE_DECELERATION * obstacleDist);
            if (maxSafe < desiredSpeed) desiredSpeed = maxSafe;
        }
        if (obstacleDist <= 1.0f) desiredSpeed = 0.0f;

        // 4. ПЛАВНОЕ УСКОРЕНИЕ/ЗАМЕДЛЕНИЕ
        if (v->speed < desiredSpeed) {
            v->speed += VEHICLE_ACCELERATION * dt;
            if (v->speed > desiredSpeed) v->speed = desiredSpeed;
        }
        else if (v->speed > desiredSpeed) {
            v->speed -= VEHICLE_DECELERATION * dt;
            if (v->speed < desiredSpeed) v->speed = desiredSpeed;
        }

        // 5. АВАРИИ
        for (int j = 0; j < MAX_VEHICLES; j++) {
            if (i == j || !vehicles[j].active) continue;
            Vehicle* other = &vehicles[j];

            float dx = other->x - v->x;
            float dy = other->y - v->y;
            float centerDist = sqrtf(dx * dx + dy * dy);

            if (centerDist < carLength * 0.6f) {
                if (v->speed > 10.0f || other->speed > 10.0f) {
                    other->active = 0;
                    v->active = 0;
                    break;
                }
            }
        }

        // 6. ДВИЖЕНИЕ
        float move = v->speed * dt;
        float prevX = v->x, prevY = v->y;
        v->x += v->dirX * move;
        v->y += v->dirY * move;

        // Выезд за границы или на траву – удаление
        int gx = (int)(v->x / 40.0f);
        int gy = (int)(v->y / 40.0f);
        if (gx < 0 || gx >= MAP_WIDTH || gy < 0 || gy >= MAP_HEIGHT) {
            v->active = 0;
            continue;
        }
        if (gameMap[gy][gx] == TILE_GRASS) {
            v->active = 0;
            continue;
        }

        // 7. ЛОГИКА ЦЕНТРИРОВАНИЯ И ПОВОРОТОВ
        int tile = gameMap[gy][gx];
        float centerX = (float)gx * 40.0f + 20.0f;
        float centerY = (float)gy * 40.0f + 20.0f;

        if (tile != TILE_INTERSECT) v->canTurn = 1;

        int crossedCenter = 0;
        if (v->dirX == 0 && v->dirY == 0) crossedCenter = 1;
        else if (v->dirX > 0 && prevX <= centerX && v->x >= centerX) crossedCenter = 1;
        else if (v->dirX < 0 && prevX >= centerX && v->x <= centerX) crossedCenter = 1;
        else if (v->dirY > 0 && prevY <= centerY && v->y >= centerY) crossedCenter = 1;
        else if (v->dirY < 0 && prevY >= centerY && v->y <= centerY) crossedCenter = 1;

        if (crossedCenter) {
            v->x = centerX;
            v->y = centerY;

            if (tile == TILE_ROAD_RIGHT) { v->dirX = 1;  v->dirY = 0; }
            else if (tile == TILE_ROAD_LEFT) { v->dirX = -1; v->dirY = 0; }
            else if (tile == TILE_ROAD_UP) { v->dirX = 0;  v->dirY = -1; }
            else if (tile == TILE_ROAD_DOWN) { v->dirX = 0;  v->dirY = 1; }
            else if (tile == TILE_INTERSECT) {
                if (v->canTurn) {
                    int possibleDirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
                    int validIndices[4];
                    int validCount = 0;
                    for (int d = 0; d < 4; d++) {
                        int vx = possibleDirs[d][0];
                        int vy = possibleDirs[d][1];
                        if (vx == -v->dirX && vy == -v->dirY && (v->dirX != 0 || v->dirY != 0)) continue;
                        if (isDirectionSafe(gx, gy, vx, vy)) {
                            validIndices[validCount++] = d;
                        }
                    }
                    if (validCount > 0) {
                        int choice = validIndices[rand() % validCount];
                        float newDirX = (float)possibleDirs[choice][0];
                        float newDirY = (float)possibleDirs[choice][1];
                        if (newDirX != v->dirX || newDirY != v->dirY) v->canTurn = 0;
                        v->dirX = (int)newDirX;
                        v->dirY = (int)newDirY;
                    }
                }
            }
            else if (tile == TILE_SPAWN) {
                int possibleDirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
                int validIndices[4];
                int validCount = 0;
                for (int d = 0; d < 4; d++) {
                    if (isDirectionSafe(gx, gy, possibleDirs[d][0], possibleDirs[d][1]))
                        validIndices[validCount++] = d;
                }
                if (validCount > 0) {
                    int choice = validIndices[rand() % validCount];
                    v->dirX = possibleDirs[choice][0];
                    v->dirY = possibleDirs[choice][1];
                    v->canTurn = 1;
                }
            }

            v->x += v->dirX * 0.1f;
            v->y += v->dirY * 0.1f;
        }
    }
}

// Проверяет, нет ли активных машин слишком близко к точке (sx, sy)
bool isSpawnAreaClear(float sx, float sy) {
    float safeDist = 65.0f;
    float safeDistSq = safeDist * safeDist; // Квадрат для ускорения вычислений

    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (!vehicles[i].active) continue;

        float dx = vehicles[i].x - sx;
        float dy = vehicles[i].y - sy;

        // Если хоть одна машина ближе безопасного радиуса — спавнить нельзя
        if ((dx * dx + dy * dy) < safeDistSq) {
            return false;
        }
    }
    return true;
}

// Появление новых машин на спавнерах раз в VEHICLE_SPAWN_INTERVAL
void spawnLogic(float dt) {
    if (isEditMode) return;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (gameMap[y][x] == TILE_SPAWN) {

                spawnTimers[y][x] += dt;

                if (spawnTimers[y][x] >= VEHICLE_SPAWN_INTERVAL) {

                    float targetX = (float)x * 40.0f + 20.0f;
                    float targetY = (float)y * 40.0f + 20.0f;

                    if (isSpawnAreaClear(targetX, targetY)) {
                        for (int i = 0; i < MAX_VEHICLES; i++) {
                            if (!vehicles[i].active) {
                                vehicles[i].active = 1;
                                vehicles[i].x = targetX;
                                vehicles[i].y = targetY;
                                vehicles[i].speed = 0.0f;
                                vehicles[i].dirX = 0;
                                vehicles[i].dirY = 0;
                                vehicles[i].canTurn = 1;

                                spawnTimers[y][x] = 0.0f;

                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}