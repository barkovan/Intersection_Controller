#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include "common.h"

// Шрифты
void buildTitleFont(void);
void buildBaseFont(void);
void buildBaseFontHov(void);

// Отрисовка
void drawText(GLuint fontBase, float x, float y, const char* text);
void render(void);

// Логика машин
void updateVehicles(float dt);
void spawnLogic(float dt);

void updateTrafficLights(void);

#endif