#include "../header/common.h"
#include "../header/vehicle.h"
#include <stdio.h>
#include <string.h>

void saveGame(const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return;

    GameSave save;
    memset(&save, 0, sizeof(GameSave));

    // 1. Сохраняем общие параметры
    save.level = currentLevel;
    save.pauseMode = isPauseMode;

    // 2. Копируем карту и таймеры
    memcpy(save.map, gameMap, sizeof(gameMap));
    memcpy(save.trafficTimer, trafficLightTimer, sizeof(trafficLightTimer));
    memcpy(save.yellowToGreenMap, yellowToGreen, sizeof(yellowToGreen));
    memcpy(save.spawnTimersMap, spawnTimers, sizeof(spawnTimers));

    // 3. Сохраняем машины (не более MAX_VEHICLES)
    int count = 0;
    Vehicle* curr = vehicleList; //
    while (curr != NULL && count < MAX_VEHICLES) {
        save.vehicles[count].x = curr->x;
        save.vehicles[count].y = curr->y;
        save.vehicles[count].speed = curr->speed;
        save.vehicles[count].dirX = curr->dirX;
        save.vehicles[count].dirY = curr->dirY;
        save.vehicles[count].canTurn = curr->canTurn;

        count++;
        curr = curr->next;
    }
    save.vehicleCount = count;

    fwrite(&save, sizeof(GameSave), 1, f);
    fclose(f);
}

bool loadGame(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return false;

    GameSave save;
    if (fread(&save, sizeof(GameSave), 1, f) != 1) {
        fclose(f);
        return false;
    }
    fclose(f);

    // 1. Очищаем текущее состояние
    clearAllVehicles();

    // 2. Восстанавливаем общие параметры
    currentLevel = save.level;
    isPauseMode = save.pauseMode;

    // 3. Восстанавливаем карту
    memcpy(gameMap, save.map, sizeof(gameMap));
    memcpy(trafficLightTimer, save.trafficTimer, sizeof(trafficLightTimer));
    memcpy(yellowToGreen, save.yellowToGreenMap, sizeof(yellowToGreen));
    memcpy(spawnTimers, save.spawnTimersMap, sizeof(spawnTimers));

    // 4. ГЛАВНОЕ: Пересоздаем список машин
    for (int i = 0; i < save.vehicleCount; i++) {
        // Создаем новый узел списка
        Vehicle* v = createVehicle(save.vehicles[i].x, save.vehicles[i].y);
        if (v) {
            v->speed = save.vehicles[i].speed;
            v->dirX = save.vehicles[i].dirX;
            v->dirY = save.vehicles[i].dirY;
            v->canTurn = save.vehicles[i].canTurn;
        }
    }

    return true;
}