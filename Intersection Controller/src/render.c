#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>

#include "../header/render.h"
#include "../header/common.h"

#include <windows.h>
#include <stdio.h> 
#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../header/stb_image.h"

// ========== ЗАГРУЗКА ТЕКСТУР ==========

// Загружает PNG-файл, возвращает ID текстуры OpenGL
GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    printf("Loaded texture: %s (%dx%d)\n", filename, width, height);
    return textureID;
}

// ========== ШРИФТЫ ==========

// Создаёт крупный шрифт для заголовка
void buildTitleFont(void) {
    HDC hDC = wglGetCurrentDC();
    HFONT hFont = CreateFont(130, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Beat Mark"));
    HFONT oldFont = (HFONT)SelectObject(hDC, hFont);
    fontBaseTitle = glGenLists(256);
    wglUseFontBitmaps(hDC, 0, 256, fontBaseTitle);
    SelectObject(hDC, oldFont);
    DeleteObject(hFont);
}

// Создаёт обычный шрифт (30pt)
void buildBaseFont(void) {
    HDC hDC = wglGetCurrentDC();
    HFONT hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
    HFONT oldFont = (HFONT)SelectObject(hDC, hFont);
    fontBase = glGenLists(256);
    wglUseFontBitmaps(hDC, 0, 256, fontBase);
    SelectObject(hDC, oldFont);
    DeleteObject(hFont);
}

// Создаёт увеличенный шрифт (40pt) для пунктов меню при наведении
void buildBaseFontHov(void) {
    HDC hDC = wglGetCurrentDC();
    HFONT hFont = CreateFont(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));
    HFONT oldFont = (HFONT)SelectObject(hDC, hFont);
    fontBaseHov = glGenLists(256);
    wglUseFontBitmaps(hDC, 0, 256, fontBaseHov);
    SelectObject(hDC, oldFont);
    DeleteObject(hFont);
}

// Выводит текст в заданных экранных координатах
void drawText(GLuint fontBase, float x, float y, const char* text) {
    if (!text) return;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    glListBase(fontBase);
    glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
}

// ========== UI: КНОПКИ ==========

// Рисует одну кнопку (обычную или с подсветкой при наведении)
static void drawButton(const Button* btn, int index, int isHovered) {
    float scale = buttonScale[index];
    float newW = btn->width * scale;
    float newH = btn->height * scale;
    float offsetX = (btn->width - newW) / 2.0f;
    float offsetY = (btn->height - newH) / 2.0f;

    GLuint currentTex = isHovered ? uiButtonHoverTex : uiButtonTex;

    // Фон (текстура)
    if (currentTex != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, currentTex);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);   // возвращаем полную яркость

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(btn->x + offsetX, btn->y + offsetY);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(btn->x + offsetX + newW, btn->y + offsetY);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(btn->x + offsetX + newW, btn->y + offsetY + newH);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(btn->x + offsetX, btn->y + offsetY + newH);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else {
        // Запасной вариант – тёмный прямоугольник
        glColor3f(0.18f, 0.18f, 0.18f);
        glBegin(GL_QUADS);
        glVertex2f(btn->x + offsetX, btn->y + offsetY);
        glVertex2f(btn->x + offsetX + newW, btn->y + offsetY);
        glVertex2f(btn->x + offsetX + newW, btn->y + offsetY + newH);
        glVertex2f(btn->x + offsetX, btn->y + offsetY + newH);
        glEnd();
    }

    // Текст (разная ширина букв для первой кнопки)
    float charWidth = 14.5f;
    if (index == 0) charWidth = 12.5f;

    float textWidth = strlen(btn->text) * charWidth;
    float textX = btn->x + offsetX + (newW - textWidth) / 2.0f;
    float textY = btn->y + offsetY + newH * 0.5f + 11.0f;

    if (isHovered) glColor3f(1.0f, 0.92f, 0.70f);
    else           glColor3f(0.95f, 0.95f, 0.95f);

    drawText(fontBase, textX, textY, btn->text);
}

// Обновляет анимацию масштабирования кнопок при наведении
static void updateButtons(void) {
    if (currentState == STATE_MENU) {
        for (int i = 0; i < 5; i++) {
            int hovered = (mouseX > buttons[i].x && mouseX < buttons[i].x + buttons[i].width &&
                           mouseY > buttons[i].y && mouseY < buttons[i].y + buttons[i].height);
            if (hovered) {
                buttonScale[i] += 0.09f;
                if (buttonScale[i] > 1.16f) buttonScale[i] = 1.16f;
            } else {
                buttonScale[i] -= 0.085f;
                if (buttonScale[i] < 1.0f) buttonScale[i] = 1.0f;
            }
        }
    } else if (currentState == STATE_LEVEL_SELECT) {
        for (int i = 0; i < 4; i++) {
            int hovered = (mouseX > levelButtons[i].x && mouseX < levelButtons[i].x + levelButtons[i].width &&
                           mouseY > levelButtons[i].y && mouseY < levelButtons[i].y + levelButtons[i].height);
            if (hovered) {
                buttonScale[i] += 0.09f;
                if (buttonScale[i] > 1.19f) buttonScale[i] = 1.19f;
            } else {
                buttonScale[i] -= 0.085f;
                if (buttonScale[i] < 1.0f) buttonScale[i] = 1.0f;
            }
        }
    }
}

// ========== ОТЛАДОЧНАЯ СЕТКА ==========

// Рисует серую сетку с шагом 40 пикселей
static void drawDebugGrid(void) {
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    for (int x = 0; x <= 1280; x += GRID_SIZE) {
        glVertex2f((float)x, 0.0f);
        glVertex2f((float)x, 720.0f);
    }
    for (int y = 0; y <= 720; y += GRID_SIZE) {
        glVertex2f(0.0f, (float)y);
        glVertex2f(1280.0f, (float)y);
    }
    glEnd();
}

// ========== ОТРИСОВКА КАРТЫ ==========

// Рисует закрашенный круг (для светофоров)
void drawFilledCircle(float cx, float cy, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)segments;
        glVertex2f(cx + radius * cosf(angle), cy + radius * sinf(angle));
    }
    glEnd();
}

// Рисует одну клетку карты (трава, дорога, перекрёсток, светофор, спавнер)
static void drawTile(int x, int y, int type) {
    float fx = (float)GX(x);
    float fy = (float)GY(y);

    // Сетку убираем, оставляем только для sandbox
    int offset = (currentLevel == 0) ? 1 : 0;

    // Цвет фона клетки
    switch (type) {
    case TILE_GRASS:                   glColor3f(0.28f, 0.46f, 0.28f); break;
    case TILE_ROAD_RIGHT:
    case TILE_ROAD_LEFT:
    case TILE_ROAD_UP:
    case TILE_ROAD_DOWN:
    case TILE_INTERSECT:               glColor3f(0.2f, 0.2f, 0.2f); break;
    case TILE_TRAFFIC_LIGHT_GREEN:     glColor3f(0.3f, 0.4f, 0.3f); break;
    case TILE_TRAFFIC_LIGHT_YELLOW:    glColor3f(0.4f, 0.4f, 0.2f); break;
    case TILE_TRAFFIC_LIGHT_RED:       glColor3f(0.4f, 0.3f, 0.3f); break;
    case TILE_SPAWN:                   glColor3f(0.1f, 0.1f, 0.1f); break;
    case TILE_HOUSE:
    case TILE_TREE:
    case TILE_SIDEWALK:                glColor3f(0.55f, 0.55f, 0.55f); break;
    default:                           glColor3f(0.0f, 0.0f, 0.0f);
    }

    glBegin(GL_QUADS);
    glVertex2f(fx + offset, fy + offset);
    glVertex2f(fx + GRID_SIZE - offset, fy + offset);
    glVertex2f(fx + GRID_SIZE - offset, fy + GRID_SIZE - offset);
    glVertex2f(fx + offset, fy + GRID_SIZE - offset);
    glEnd();

    // отрисовка текстур
    if (type == TILE_HOUSE || type == TILE_TREE) {
        GLuint currentTex = (type == TILE_HOUSE) ? houseTex : treeTex;

        float multiplier = (type == TILE_HOUSE) ? 10.5f : 3.0f;
        float drawSize = GRID_SIZE * multiplier; // Новая ширина и высота

        float hOffset = (drawSize - GRID_SIZE) / 2.0f;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, currentTex);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Сброс цвета

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(fx - hOffset, fy - (drawSize - GRID_SIZE));

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(fx + GRID_SIZE + hOffset, fy - (drawSize - GRID_SIZE));

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(fx + GRID_SIZE + hOffset, fy + GRID_SIZE);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(fx - hOffset, fy + GRID_SIZE);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    // Стрелки направления на дорогах
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    if (type == TILE_ROAD_RIGHT) {
        glVertex2f(fx + 10, fy + 20); glVertex2f(fx + 30, fy + 20);
        glVertex2f(fx + 30, fy + 20); glVertex2f(fx + 25, fy + 15);
        glVertex2f(fx + 30, fy + 20); glVertex2f(fx + 25, fy + 25);
    } else if (type == TILE_ROAD_LEFT) {
        glVertex2f(fx + 30, fy + 20); glVertex2f(fx + 10, fy + 20);
        glVertex2f(fx + 10, fy + 20); glVertex2f(fx + 15, fy + 15);
        glVertex2f(fx + 10, fy + 20); glVertex2f(fx + 15, fy + 25);
    } else if (type == TILE_ROAD_UP) {
        glVertex2f(fx + 20, fy + 30); glVertex2f(fx + 20, fy + 10);
        glVertex2f(fx + 20, fy + 10); glVertex2f(fx + 15, fy + 15);
        glVertex2f(fx + 20, fy + 10); glVertex2f(fx + 25, fy + 15);
    } else if (type == TILE_ROAD_DOWN) {
        glVertex2f(fx + 20, fy + 10); glVertex2f(fx + 20, fy + 30);
        glVertex2f(fx + 20, fy + 30); glVertex2f(fx + 15, fy + 25);
        glVertex2f(fx + 20, fy + 30); glVertex2f(fx + 25, fy + 25);
    } else if (type == TILE_INTERSECT) {
        glVertex2f(fx + 15, fy + 20); glVertex2f(fx + 25, fy + 20);
        glVertex2f(fx + 20, fy + 15); glVertex2f(fx + 20, fy + 25);
    }
    glEnd();

    // Светофор (зелёный/жёлтый/красный круг с ободком)
    if (type == TILE_TRAFFIC_LIGHT_GREEN || type == TILE_TRAFFIC_LIGHT_YELLOW || type == TILE_TRAFFIC_LIGHT_RED) {
        float centerX = fx + 20.0f;
        float centerY = fy + 20.0f;
        float lightRadius = 10.0f;
        float rimRadius = lightRadius + 1.5f;

        glColor3f(0.0f, 0.0f, 0.0f);
        drawFilledCircle(centerX, centerY, rimRadius, 30);

        if (type == TILE_TRAFFIC_LIGHT_GREEN)       glColor3f(0.0f, 1.0f, 0.0f);
        else if (type == TILE_TRAFFIC_LIGHT_YELLOW) glColor3f(1.0f, 1.0f, 0.0f);
        else                                        glColor3f(1.0f, 0.0f, 0.0f);

        drawFilledCircle(centerX, centerY, lightRadius, 30);
    }

    // Рамка спавнера
    if (type == TILE_SPAWN) {
        glColor3f(0.0f, 1.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(fx + 5, fy + 5); glVertex2f(fx + 35, fy + 5);
        glVertex2f(fx + 35, fy + 35); glVertex2f(fx + 5, fy + 35);
        glEnd();
    }
}

// ========== ОТРИСОВКА МАШИН ==========

// Рисует машину с текстурой и поворотом в зависимости от направления
void drawVehicle(Vehicle* v) {
    if (!v->active) return;

    glPushMatrix();
    glTranslatef(v->x, v->y, 0.0f);

    float angle = 0.0f;
    if (v->dirX == 1)  angle = 0.0f;
    if (v->dirX == -1) angle = 180.0f;
    if (v->dirY == 1)  angle = 90.0f;
    if (v->dirY == -1) angle = -90.0f;

    glRotatef(angle - 90.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, carTex);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    float length = 55.0f;
    float width = 55.0f;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-length / 2, -width / 2);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(length / 2, -width / 2);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(length / 2, width / 2);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-length / 2, width / 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPopMatrix();
}

// ========== СВЕТОФОРЫ ==========

// Переключает жёлтый сигнал в зелёный или красный по истечении YELLOW_DURATION
void updateTrafficLights(void) {
    double now = glfwGetTime();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (gameMap[y][x] == TILE_TRAFFIC_LIGHT_YELLOW) {
                if (now - trafficLightTimer[y][x] >= YELLOW_DURATION) {
                    if (yellowToGreen[y][x]) {
                        gameMap[y][x] = TILE_TRAFFIC_LIGHT_GREEN;
                    } else {
                        gameMap[y][x] = TILE_TRAFFIC_LIGHT_RED;
                    }
                    trafficLightTimer[y][x] = 0.0;
                    yellowToGreen[y][x] = false;
                }
            }
        }
    }
}

// ========== ГЛАВНЫЙ РЕНДЕР ==========

void render(void) {
    glClearColor(0.08f, 0.15f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_TEXTURE_2D);

    if (showDebugGrid) drawDebugGrid();

    updateButtons();

    if (currentState == STATE_MENU) {
        // Фоновая клякса
        if (titlePlateTex != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, titlePlateTex);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            float pX = GX(1.0f);
            float pY = GY(-2.5f);
            float pW = GX(30.0f);
            float pH = GY(15.0f);

            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(pX, pY);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(pX + pW, pY);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(pX + pW, pY + pH);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(pX, pY + pH);
            glEnd();

            glDisable(GL_TEXTURE_2D);
        }

        // Заголовок текстом
        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(fontBaseTitle, GX(5.6f), GY(6.2f), "TRAFFIC SIMULATOR");

        // Кнопки меню
        for (int i = 0; i < 5; i++) {
            int hovered = (mouseX > buttons[i].x && mouseX < buttons[i].x + buttons[i].width &&
                           mouseY > buttons[i].y && mouseY < buttons[i].y + buttons[i].height);
            drawButton(&buttons[i], i, hovered);
        }
    } else if (currentState == STATE_LEVEL_SELECT) {
        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(fontBaseTitle, GX(2.8f), GY(3), "SELECT DIFFICULTY LEVEL");
        for (int i = 0; i < 4; i++) {
            int hovered = (mouseX > levelButtons[i].x && mouseX < levelButtons[i].x + levelButtons[i].width &&
                           mouseY > levelButtons[i].y && mouseY < levelButtons[i].y + levelButtons[i].height);
            drawButton(&levelButtons[i], i, hovered);
        }
    } else if (currentState == STATE_SIMULATION) {
        // Карта
        for (int y = 0; y < MAP_HEIGHT; y++)
            for (int x = 0; x < MAP_WIDTH; x++)
                drawTile(x, y, gameMap[y][x]);

        // Машины
        for (int i = 0; i < MAX_VEHICLES; i++)
            if (vehicles[i].active) drawVehicle(&vehicles[i]);

        // HUD
        if (isEditMode) {
            glColor3f(1.0f, 0.5f, 0.0f);
            drawText(fontBase, 30, 30, "MODE: MAP EDITOR");
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(fontBase, 30, 70, "PRESS [SPACE] TO START SIMULATION");
        } else {
            glColor3f(0.0f, 1.0f, 0.0f);
            drawText(fontBase, 30, 30, "MODE: SIMULATION");
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(fontBase, 30, 70, "CLICK TRAFFIC LIGHTS TO TOGGLE");
        }

        char levelText[50];
        sprintf(levelText, "CURRENT LEVEL: %d", currentLevel);
        drawText(fontBase, 30, 110, levelText);

        // Текущая кисть редактора
        char brushText[30];
        const char* typeName = "Unknown";
        switch (currentBrush) {
        case TILE_ROAD_RIGHT: typeName = "Road Right >"; break;
        case TILE_ROAD_LEFT:  typeName = "Road Left <"; break;
        case TILE_ROAD_UP:    typeName = "Road Up /\\"; break;
        case TILE_ROAD_DOWN:  typeName = "Road Down \\/"; break;
        case TILE_TRAFFIC_LIGHT_GREEN: typeName = "Traffic Light"; break;
        case TILE_INTERSECT:  typeName = "Intersection"; break;
        case TILE_SPAWN:      typeName = "Spawner"; break;
        }
        sprintf(brushText, "BRUSH: %s", typeName);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(fontBase, 30, 150, brushText);
    }
    else if (currentState == STATE_HELP) {
        // Фоновая клякса
        if (helpBgTex != 0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, helpBgTex);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            // Растягиваем на всё окно
            float bgX = GX(-3);
            float bgY = GY(-4);
            float bgW = GX(38);  // 1280px
            float bgH = GY(25);  // 720px

            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(bgX, bgY);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(bgX + bgW, bgY);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(bgX + bgW, bgY + bgH);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(bgX, bgY + bgH);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
        }

        float startY = GY(4);
        float lineHeight = GY(1.2f);

        // Заголовок
        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(fontBaseHov, GX(11), GY(2), "HOW TO PLAY");

        // Редактор
        glColor3f(1.0f, 0.7f, 0.3f);
        drawText(fontBaseHov, GX(4), startY, "EDITOR MODE:");

        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(fontBase, GX(4), startY + lineHeight, "1-4 - Select road direction (right/left/up/down)");
        drawText(fontBase, GX(4), startY + lineHeight * 2, "5 - Select intersection tile");
        drawText(fontBase, GX(4), startY + lineHeight * 3, "6 - Select traffic light");
        drawText(fontBase, GX(4), startY + lineHeight * 4, "7 - Select spawn point");
        drawText(fontBase, GX(4), startY + lineHeight * 5, "LMB - Place selected tile");
        drawText(fontBase, GX(4), startY + lineHeight * 6, "RMB - Erase tile (place grass)");
        drawText(fontBase, GX(4), startY + lineHeight * 7, "SPACE - Switch to Simulation mode");
        drawText(fontBase, GX(4), startY + lineHeight * 8, "F8 - Toggle debug grid");

        // Симуляция
        float simY = startY + lineHeight * 10;
        glColor3f(1.0f, 0.7f, 0.3f);
        drawText(fontBaseHov, GX(4), simY, "SIMULATION MODE:");

        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(fontBase, GX(4), simY + lineHeight, "LMB on traffic light - Toggle signal");
        drawText(fontBase, GX(4), simY + lineHeight * 2, "SPACE - Switch to Editor mode");
        drawText(fontBase, GX(4), simY + lineHeight * 3, "F8 - Toggle debug grid");

        // Общее
        float generalY = simY + lineHeight * 5;
        glColor3f(1.0f, 0.7f, 0.3f);
        drawText(fontBaseHov, GX(4), generalY, "GENERAL:");

        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(fontBase, GX(4), generalY + lineHeight, "ESC - Return to main menu");

        glDisable(GL_TEXTURE_2D);   // выключаем текстуру
        glEnable(GL_BLEND);         // включаем блендинг обратно (если был выключен)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // сбрасываем цвет
    }
}