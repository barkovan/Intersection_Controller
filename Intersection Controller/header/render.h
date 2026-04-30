#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include "common.h"

// Загрузка PNG-файла
GLuint loadTexture(const char* filename);

// Шрифты
void buildTitleFont(void);
void buildBaseFont(void);
void buildBaseFontHov(void);

// Отрисовка
void drawText(GLuint fontBase, float x, float y, const char* text);
void render(void);
void drawVehicle(Vehicle* v);

void updateTrafficLights(void);

#endif