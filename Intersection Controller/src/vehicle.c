#include "../header/vehicle.h"
#include <math.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

// Глобальный указатель на голову списка
Vehicle* vehicleList = NULL;

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ СПИСКА ---

// Добавление новой машины в начало списка
Vehicle* createVehicle(float x, float y) {
    Vehicle* newV = (Vehicle*)malloc(sizeof(Vehicle));
    if (!newV) return NULL;

    newV->x = x;
    newV->y = y;
    newV->speed = 0.0f;
    newV->dirX = 0;
    newV->dirY = 0;
    newV->canTurn = 1;

    // Вставка в начало двусвязного списка
    newV->prev = NULL;
    newV->next = vehicleList;
    if (vehicleList != NULL) {
        vehicleList->prev = newV;
    }
    vehicleList = newV;

    return newV;
}

// Удаление машины из памяти
void deleteVehicle(Vehicle* v) {
    if (!v) return;

    if (v->prev) v->prev->next = v->next;
    if (v->next) v->next->prev = v->prev;

    if (v == vehicleList) {
        vehicleList = v->next;
    }

    free(v);
}

// --- ЛОГИКА ---
bool isDirectionSafe(int gx, int gy, int dx, int dy);
void updateVehicles(float dt);

bool isDirectionSafe(int gx, int gy, int dx, int dy) {
    int cx = gx + dx;
    int cy = gy + dy;

    while (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
        int tile = gameMap[cy][cx];
        if (tile == TILE_INTERSECT || tile == TILE_TRAFFIC_LIGHT_GREEN) {
            cx += dx; cy += dy; continue;
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

void updateVehicles(float dt) {
    float carLength = 55.0f;
    float halfCarLength = carLength / 2;

    Vehicle* v = vehicleList;
    while (v != NULL) {
        // Запоминаем следующую машину сразу, т.к. текущую можем удалить
        Vehicle* nextVehicle = v->next;

        int currGx = (int)(v->x / 40.0f);
        int currGy = (int)(v->y / 40.0f);

        // поиск светофора
        int mustStopForRed = 0;
        float distToStopLine = 10000.0f;
        int lookAheadCells = 5;

        for (int step = 1; step <= lookAheadCells; step++) {
            int checkGx = currGx + v->dirX * step;
            int checkGy = currGy + v->dirY * step;

            if (checkGx >= 0 && checkGx < MAP_WIDTH && checkGy >= 0 && checkGy < MAP_HEIGHT) {
                int tile = gameMap[checkGy][checkGx];
                if (tile == TILE_TRAFFIC_LIGHT_RED || tile == TILE_TRAFFIC_LIGHT_YELLOW) {
                    mustStopForRed = 1;
                    float stopLine = 0.0f;
                    float dist = 0.0f;

                    if (v->dirX > 0) { stopLine = checkGx * 40.0f; dist = stopLine - (v->x + halfCarLength); }
                    else if (v->dirX < 0) { stopLine = (checkGx + 1) * 40.0f; dist = (v->x - halfCarLength) - stopLine; }
                    else if (v->dirY > 0) { stopLine = checkGy * 40.0f; dist = stopLine - (v->y + halfCarLength); }
                    else if (v->dirY < 0) { stopLine = (checkGy + 1) * 40.0f; dist = (v->y - halfCarLength) - stopLine; }

                    if (dist > -5.0f && dist < distToStopLine) distToStopLine = (dist > 0.0f) ? dist : 0.0f;
                    break;
                }
            }
        }

        // Расстояние до ближайших машин
        float distToCar = 10000.0f;
        Vehicle* other = vehicleList;
        while (other != NULL) {
            if (v == other) { other = other->next; continue; }

            if (v->dirX != other->dirX || v->dirY != other->dirY) { other = other->next; continue; }

            float dx = other->x - v->x;
            float dy = other->y - v->y;

            if ((v->dirX > 0 && dx > 0 && fabsf(dy) <


                15.0f) ||
                (v->dirX < 0 && dx < 0 && fabsf(dy) < 15.0f) ||
                (v->dirY > 0 && dy > 0 && fabsf(dx) < 15.0f) ||
                (v->dirY < 0 && dy < 0 && fabsf(dx) < 15.0f)) {
                float dist = sqrtf(dx * dx + dy * dy) - carLength;
                if (dist < distToCar) distToCar = (dist > 0.0f) ? dist : 0.0f;
            }
            other = other->next;
        }

        // Скорость и ускорение
        float desiredSpeed = VEHICLE_MAX_SPEED;
        float obstacleDist = distToCar;
        if (mustStopForRed && distToStopLine < obstacleDist) obstacleDist = distToStopLine;

        if (obstacleDist < 200.0f) {
            float maxSafe = sqrtf(VEHICLE_DECELERATION * obstacleDist);
            if (maxSafe < desiredSpeed) desiredSpeed = maxSafe;
        }
        if (obstacleDist <= 1.0f) desiredSpeed = 0.0f;

        if (v->speed < desiredSpeed) {
            v->speed += VEHICLE_ACCELERATION * dt;
            if (v->speed > desiredSpeed) v->speed = desiredSpeed;
        }
        else if (v->speed > desiredSpeed) {
            v->speed -= VEHICLE_DECELERATION * dt;
            if (v->speed < desiredSpeed) v->speed = desiredSpeed;
        }

        //  Движение и удаление
        float move = v->speed * dt;
        float prevX = v->x, prevY = v->y;
        v->x += v->dirX * move;
        v->y += v->dirY * move;

        int gx = (int)(v->x / 40.0f);
        int gy = (int)(v->y / 40.0f);

        if (gx < 0 || gx >= MAP_WIDTH || gy < 0 || gy >= MAP_HEIGHT || gameMap[gy][gx] == TILE_GRASS) {
            deleteVehicle(v);
            carsPassedCount++;
            v = nextVehicle;
            continue;
        }

        // Аварии
        Vehicle* crashOther = vehicleList;
        int crashed = 0;
        while (crashOther != NULL) {
            if (v == crashOther) { crashOther = crashOther->next; continue; }
            float dx = crashOther->x - v->x;
            float dy = crashOther->y - v->y;
            float centerDist = sqrtf(dx * dx + dy * dy);

            if (centerDist < carLength * 0.70f) {
                // сохраняем следующий за crashOther, чтобы не потерять nextVehicle
                Vehicle* otherNext = crashOther->next;
                deleteVehicle(crashOther);

                // если nextVehicle указывал на crashOther, сдвигаемся дальше
                if (nextVehicle == crashOther)
                    nextVehicle = otherNext;

                deleteVehicle(v);
                crashed = 1;
                break;
            }
            crashOther = crashOther->next;
        }
        if (crashed) {
            if (currentLevel != 0) lives--;
            v = nextVehicle;
            continue;
        }

        // Повороты и центрирование
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
            v->x = centerX; v->y = centerY;
            if (tile == TILE_ROAD_RIGHT) { v->dirX = 1; v->dirY = 0; }
            else if (tile == TILE_ROAD_LEFT) { v->dirX = -1; v->dirY = 0; }
            else if (tile == TILE_ROAD_UP) { v->dirX = 0; v->dirY = -1; }
            else if (tile == TILE_ROAD_DOWN) { v->dirX = 0; v->dirY = 1; }
            else if ((tile == TILE_INTERSECT || tile == TILE_SPAWN) && v->canTurn) {

                int possibleDirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
                int validIndices[4];
                int validCount = 0;
                int straightIndex = -1;

                for (int d = 0; d < 4; d++) {
                    int vx = possibleDirs[d][0];
                    int vy = possibleDirs[d][1];

                    // Запрещаем разворот назад (нельзя ехать в противоположную сторону)
                    if (vx == -v->dirX && vy == -v->dirY && (v->dirX != 0 || v->dirY != 0)) continue;

                    // Проверяем, безопасна ли клетка (не встречка ли там)
                    if (isDirectionSafe(gx, gy, vx, vy)) {
                        validIndices[validCount] = d;
                        // Проверяем, является ли это направление движением "прямо"
                        if (vx == v->dirX && vy == v->dirY) {
                            straightIndex = validCount;
                        }
                        validCount++;
                    }
                }

                if (validCount > 0) {
                    int choiceIndex = -1;

                    // Приоритет: если можно ехать прямо, едем прямо (чтобы машины не метались)
                    if (straightIndex != -1) {
                        choiceIndex = validIndices[straightIndex];
                    }
                    else {
                        // Если прямо нельзя (тупик или поворот), выбираем рандом из доступных
                        choiceIndex = validIndices[rand() % validCount];
                    }

                    float newDirX = (float)possibleDirs[choiceIndex][0];
                    float newDirY = (float)possibleDirs[choiceIndex][1];

                    // Если направление изменилось, запрещаем поворачивать еще раз на этом же тайле
                    if (newDirX != (float)v->dirX || newDirY != (float)v->dirY) {
                        v->canTurn = 0;
                    }

                    v->dirX = (int)newDirX;
                    v->dirY = (int)newDirY;
                }
            }
            v->x += v->dirX * 0.1f; v->y += v->dirY * 0.1f;
        }

        v = nextVehicle; // Переходим к следующей машине в списке
    }
}

bool isSpawnAreaClear(float sx, float sy) {
    float safeDistSq = 65.0f * 65.0f;
    Vehicle* curr =


        vehicleList;
    while (curr != NULL) {
        float dx = curr->x - sx;
        float dy = curr->y - sy;
        if ((dx * dx + dy * dy) < safeDistSq) return false;
        curr = curr->next;
    }
    return true;
}

void spawnLogic(float dt) {
    if (isPauseMode) return;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (gameMap[y][x] == TILE_SPAWN) {
                spawnTimers[y][x] += dt;
                float spawnInterval = (currentLevel == 0) ? 3.0f : VEHICLE_SPAWN_INTERVAL;
                if (spawnTimers[y][x] >= spawnInterval) {
                    float targetX = (float)x * 40.0f + 20.0f;
                    float targetY = (float)y * 40.0f + 20.0f;
                    if (isSpawnAreaClear(targetX, targetY)) {
                        createVehicle(targetX, targetY);
                        if (currentLevel == 0) {
                            spawnTimers[y][x] = 0.0f;
                        }
                        else {
                            spawnTimers[y][x] = 2.0f + (float)rand() / (float)RAND_MAX * (VEHICLE_SPAWN_INTERVAL - 2.0f);
                        }
                    }
                }
            }
        }
    }
}

void clearAllVehicles(void) {
    Vehicle* curr = vehicleList;
    while (curr != NULL) {
        Vehicle* next = curr->next;
        free(curr);
        curr = next;
    }
    vehicleList = NULL;
}
