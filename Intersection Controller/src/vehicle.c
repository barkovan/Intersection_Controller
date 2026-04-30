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
    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (!vehicles[i].active) continue;
        Vehicle* v = &vehicles[i];

        int currGx = (int)(v->x / 40.0f);
        int currGy = (int)(v->y / 40.0f);
        int nextGx = currGx + v->dirX;
        int nextGy = currGy + v->dirY;

        // Нужно ли останавливаться перед красным или жёлтым светофором
        int mustStopForRed = 0;
        if (nextGx >= 0 && nextGx < MAP_WIDTH && nextGy >= 0 && nextGy < MAP_HEIGHT) {
            if (gameMap[nextGy][nextGx] == TILE_TRAFFIC_LIGHT_RED ||
                gameMap[nextGy][nextGx] == TILE_TRAFFIC_LIGHT_YELLOW) {
                mustStopForRed = 1;
            }
        }

        // Расстояние до центра текущей клетки (точка остановки)
        float stopX = currGx * 40.0f + 20.0f;
        float stopY = currGy * 40.0f + 20.0f;
        float distToStop = 0.0f;
        if (v->dirX > 0)      distToStop = stopX - v->x;
        else if (v->dirX < 0) distToStop = v->x - stopX;
        else if (v->dirY > 0) distToStop = stopY - v->y;
        else if (v->dirY < 0) distToStop = v->y - stopY;

        // Расстояние до ближайшей машины впереди
        float distToCar = 10000.0f;
        for (int j = 0; j < MAX_VEHICLES; j++) {
            if (i == j || !vehicles[j].active) continue;
            Vehicle* other = &vehicles[j];
            float dx = other->x - v->x;
            float dy = other->y - v->y;
            if ((v->dirX > 0 && dx > 0 && fabsf(dy) < 15.0f) ||
                (v->dirX < 0 && dx < 0 && fabsf(dy) < 15.0f) ||
                (v->dirY > 0 && dy > 0 && fabsf(dx) < 15.0f) ||
                (v->dirY < 0 && dy < 0 && fabsf(dx) < 15.0f)) {
                float dist = sqrtf(dx * dx + dy * dy) - 16.0f;
                if (dist < distToCar) distToCar = dist;
            }
        }

        // Желаемая скорость с учётом препятствий
        float desiredSpeed = VEHICLE_MAX_SPEED;
        float obstacleDist = distToCar;
        if (mustStopForRed && distToStop < obstacleDist) obstacleDist = distToStop;

        if (obstacleDist < 1000.0f) {
            float maxSafe = sqrtf(2.0f * VEHICLE_DECELERATION * obstacleDist);
            if (maxSafe < desiredSpeed) desiredSpeed = maxSafe;
        }
        if (obstacleDist <= 2.0f) desiredSpeed = 0.0f;

        // Плавное ускорение/замедление
        if (v->speed < desiredSpeed) {
            v->speed += VEHICLE_ACCELERATION * dt;
            if (v->speed > desiredSpeed) v->speed = desiredSpeed;
        }
        else if (v->speed > desiredSpeed) {
            v->speed -= VEHICLE_DECELERATION * dt;
            if (v->speed < desiredSpeed) v->speed = desiredSpeed;
        }

        // Столкновение (авария)
        if (distToCar < 2.0f && v->speed > 10.0f) {
            for (int j = 0; j < MAX_VEHICLES; j++) {
                if (i == j || !vehicles[j].active) continue;
                Vehicle* other = &vehicles[j];
                float dx = other->x - v->x;
                float dy = other->y - v->y;
                if ((v->dirX > 0 && dx > 0 && fabsf(dy) < 15.0f) ||
                    (v->dirX < 0 && dx < 0 && fabsf(dy) < 15.0f) ||
                    (v->dirY > 0 && dy > 0 && fabsf(dx) < 15.0f) ||
                    (v->dirY < 0 && dy < 0 && fabsf(dx) < 15.0f)) {
                    other->active = 0;
                    break;
                }
            }
            v->active = 0;
            continue;
        }

        // Движение
        float move = v->speed * dt;
        float prevX = v->x, prevY = v->y;
        if (!(mustStopForRed && fabs(distToStop) < 1.0f && v->speed < 1.0f)) {
            v->x += v->dirX * move;
            v->y += v->dirY * move;
        }

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

        int tile = gameMap[gy][gx];
        float centerX = (float)gx * 40.0f + 20.0f;
        float centerY = (float)gy * 40.0f + 20.0f;

        if (tile != TILE_INTERSECT) v->canTurn = 1;

        // Проверка пересечения центра клетки
        int crossedCenter = 0;
        if (v->dirX == 0 && v->dirY == 0) {
            crossedCenter = 1;
        }
        else if (v->dirX > 0 && prevX <= centerX && v->x >= centerX) {
            crossedCenter = 1;
        }
        else if (v->dirX < 0 && prevX >= centerX && v->x <= centerX) {
            crossedCenter = 1;
        }
        else if (v->dirY > 0 && prevY <= centerY && v->y >= centerY) {
            crossedCenter = 1;
        }
        else if (v->dirY < 0 && prevY >= centerY && v->y <= centerY) {
            crossedCenter = 1;
        }

        if (crossedCenter) {
            v->x = centerX;
            v->y = centerY;

            // Выбор нового направления в зависимости от типа клетки
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
                        if (vx == -v->dirX && vy == -v->dirY && (v->dirX != 0 || v->dirY != 0))
                            continue;
                        if (isDirectionSafe(gx, gy, vx, vy)) {
                            validIndices[validCount++] = d;
                        }
                    }
                    if (validCount > 0) {
                        int choice = validIndices[rand() % validCount];
                        float newDirX = (float)possibleDirs[choice][0];
                        float newDirY = (float)possibleDirs[choice][1];
                        if (newDirX != v->dirX || newDirY != v->dirY)
                            v->canTurn = 0;
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

            // Небольшой сдвиг, чтобы не застрять на центре
            v->x += v->dirX * 0.1f;
            v->y += v->dirY * 0.1f;
        }
    }
}

// Появление новых машин на спавнерах раз в VEHICLE_SPAWN_INTERVAL
void spawnLogic(float dt) {
    static double lastSpawnTime = 0.0;
    double currentTime = glfwGetTime();

    if (currentTime - lastSpawnTime < VEHICLE_SPAWN_INTERVAL) return;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (gameMap[y][x] == TILE_SPAWN) {
                for (int i = 0; i < MAX_VEHICLES; i++) {
                    if (!vehicles[i].active) {
                        vehicles[i].active = 1;
                        vehicles[i].x = (float)x * 40 + 20;
                        vehicles[i].y = (float)y * 40 + 20;
                        vehicles[i].speed = 0.0f;
                        vehicles[i].dirX = 0;
                        vehicles[i].dirY = 0;
                        vehicles[i].canTurn = 1;
                        lastSpawnTime = currentTime;
                        return;
                    }
                }
            }
        }
    }
}