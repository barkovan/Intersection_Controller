#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>

#include "../header/render.h"
#include "../header/common.h"

#include <windows.h>
#include <stdio.h> 
#include <string.h>
#include <math.h>

void buildTitleFont(void) {
    // Шрифт названия игры
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

void buildBaseFont(void) {
    // Шрифт по умолчанию
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

void buildBaseFontHov(void) {
    // Увеличенный шрифт по умолчанию
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

void drawText(GLuint fontBase, float x, float y, const char* text) {
    // Отрисовка текста
    if (!text) return;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    glListBase(fontBase);
    glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);
}

static void drawButton(const Button* btn, int index, int isHovered) {
    // Отрисовка кнопок
    float scale = buttonScale[index];
    float newW = btn->width * scale;
    float newH = btn->height * scale;
    float offsetX = (btn->width - newW) / 2.0f;
    float offsetY = (btn->height - newH) / 2.0f;

    if (isHovered) glColor3f(0.0f, 0.35f, 0.2f);
    else glColor3f(0.02f, 0.02f, 0.02f);

    glBegin(GL_QUADS);
    glVertex2f(btn->x + offsetX, btn->y + offsetY);
    glVertex2f(btn->x + offsetX + newW, btn->y + offsetY);
    glVertex2f(btn->x + offsetX + newW, btn->y + offsetY + newH);
    glVertex2f(btn->x + offsetX, btn->y + offsetY + newH);
    glEnd();

    if (isHovered) glColor3f(0.0f, 0.9f, 0.4f);
    else glColor3f(0.15f, 0.4f, 0.25f);

    glBegin(GL_LINE_LOOP);
    glVertex2f(btn->x + offsetX, btn->y + offsetY);
    glVertex2f(btn->x + offsetX + newW, btn->y + offsetY);
    glVertex2f(btn->x + offsetX + newW, btn->y + offsetY + newH);
    glVertex2f(btn->x + offsetX, btn->y + offsetY + newH);
    glEnd();

    if (isHovered) glColor3f(0.0f, 1.0f, 0.6f);
    else glColor3f(0.7f, 0.7f, 0.7f);

    if (strcmp(btn->type, "Menu") == 0)
        if (!isHovered)
            drawText(fontBase, btn->x + offsetX + 60, btn->y + offsetY + 40, btn->text);
        else
            drawText(fontBaseHov, btn->x + offsetX + 60, btn->y + offsetY + 45, btn->text);
    else if (strcmp(btn->type, "Level") == 0)
        if (!isHovered)
            drawText(fontBase, btn->x + offsetX + (newW - 15 * strlen(btn->text)) / 2, btn->y + offsetY + (newH / 2), btn->text);
        else
            drawText(fontBaseHov, btn->x + offsetX + (newW - 20 * strlen(btn->text)) / 2, btn->y + offsetY + (newH / 2), btn->text);
}

static void updateButtons(void) {
    // Обработка при наведении курсора на кнопку
    if (currentState == STATE_MENU) {
        for (int i = 0; i < 5; i++) {
            int hovered = (mouseX > buttons[i].x && mouseX < buttons[i].x + buttons[i].width &&
                mouseY > buttons[i].y && mouseY < buttons[i].y + buttons[i].height);
            if (hovered) {
                buttonScale[i] += 0.08f;
                if (buttonScale[i] > 1.12f) buttonScale[i] = 1.12f;
            }
            else {
                buttonScale[i] -= 0.08f;
                if (buttonScale[i] < 1.0f) buttonScale[i] = 1.0f;
            }
        }
    }
    else if (currentState == STATE_LEVEL_SELECT) {
        for (int i = 0; i < 3; i++) {
            int hovered = (mouseX > levelButtons[i].x && mouseX < levelButtons[i].x + levelButtons[i].width &&
                mouseY > levelButtons[i].y && mouseY < levelButtons[i].y + levelButtons[i].height);
            if (hovered) {
                buttonScale[i] += 0.08f;
                if (buttonScale[i] > 1.12f) buttonScale[i] = 1.12f;
            }
            else {
                buttonScale[i] -= 0.08f;
                if (buttonScale[i] < 1.0f) buttonScale[i] = 1.0f;
            }
        }
    }
}

static void drawDebugGrid(void) {
    // Отрисовка дебаг сетки
    glColor3f(0.2f, 0.2f, 0.2f); // Серый

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

void drawFilledCircle(float cx, float cy, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * (float)i / (float)segments;
        glVertex2f(cx + radius * cosf(angle), cy + radius * sinf(angle));
    }
    glEnd();
}

static void drawTile(int x, int y, int type) {
    float fx = (float)GX(x);
    float fy = (float)GY(y);

    switch (type) {
    case TILE_GRASS:     glColor3f(0.1f, 0.4f, 0.1f); break; // Зеленый
    case TILE_ROAD_RIGHT:
    case TILE_ROAD_LEFT:
    case TILE_ROAD_UP:
    case TILE_ROAD_DOWN:
    case TILE_INTERSECT: glColor3f(0.2f, 0.2f, 0.2f); break; // Светло-серый
    case TILE_TRAFFIC_LIGHT_GREEN: glColor3f(0.3f, 0.4f, 0.3f); break;
    case TILE_TRAFFIC_LIGHT_RED: glColor3f(0.4f, 0.3f, 0.3f); break;
    case TILE_SPAWN: glColor3f(0.1f, 0.1f, 0.1f); break; // Темно-серый
    default:             glColor3f(0.0f, 0.0f, 0.0f);
    }

    glBegin(GL_QUADS);
    glVertex2f(fx + 1, fy + 1); // Отступ
    glVertex2f(fx + 39, fy + 1);
    glVertex2f(fx + 39, fy + 39);
    glVertex2f(fx + 1, fy + 39);
    glEnd();

    glColor3f(0.5f, 0.5f, 0.5f); // Отрисовка стрелов движения
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    if (type == TILE_ROAD_RIGHT) {
        glVertex2f(fx + 10, fy + 20); glVertex2f(fx + 30, fy + 20);
        glVertex2f(fx + 30, fy + 20); glVertex2f(fx + 25, fy + 15);
        glVertex2f(fx + 30, fy + 20); glVertex2f(fx + 25, fy + 25);
    }
    else if (type == TILE_ROAD_LEFT) {
        glVertex2f(fx + 30, fy + 20); glVertex2f(fx + 10, fy + 20);
        glVertex2f(fx + 10, fy + 20); glVertex2f(fx + 15, fy + 15);
        glVertex2f(fx + 10, fy + 20); glVertex2f(fx + 15, fy + 25);
    }
    else if (type == TILE_ROAD_UP) {
        glVertex2f(fx + 20, fy + 30); glVertex2f(fx + 20, fy + 10);
        glVertex2f(fx + 20, fy + 10); glVertex2f(fx + 15, fy + 15);
        glVertex2f(fx + 20, fy + 10); glVertex2f(fx + 25, fy + 15);
    }
    else if (type == TILE_ROAD_DOWN) {
        glVertex2f(fx + 20, fy + 10); glVertex2f(fx + 20, fy + 30);
        glVertex2f(fx + 20, fy + 30); glVertex2f(fx + 15, fy + 25);
        glVertex2f(fx + 20, fy + 30); glVertex2f(fx + 25, fy + 25);
    }
    else if (type == TILE_INTERSECT) {
        // Отрисовка креста перекрестка
        glVertex2f(fx + 15, fy + 20); glVertex2f(fx + 25, fy + 20);
        glVertex2f(fx + 20, fy + 15); glVertex2f(fx + 20, fy + 25);
    }
    glEnd();

    if (type == TILE_TRAFFIC_LIGHT_GREEN || type == TILE_TRAFFIC_LIGHT_RED) {
        float centerX = fx + 20.0f;
        float centerY = fy + 20.0f;
        float lightRadius = 10.0f; // Радиус самого света
        float rimRadius = lightRadius + 1.5f; // Радиус обода (на 1.5 пикселя больше)

        // А. Рисуем черный обод (подложку)
        glColor3f(0.0f, 0.0f, 0.0f); // Черный цвет
        drawFilledCircle(centerX, centerY, rimRadius, 30);

        // Б. Рисуем сам свет поверх обода
        if (type == TILE_TRAFFIC_LIGHT_GREEN) {
            glColor3f(0.0f, 1.0f, 0.0f); // Ярко-зеленый
        }
        else {
            glColor3f(1.0f, 0.0f, 0.0f); // Ярко-красный
        }
        drawFilledCircle(centerX, centerY, lightRadius, 30);
    }

    else if (type == TILE_SPAWN) {
        // Отрисовка рамки спавнера
        glColor3f(0.0f, 1.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(fx + 5, fy + 5); glVertex2f(fx + 35, fy + 5);
        glVertex2f(fx + 35, fy + 35); glVertex2f(fx + 5, fy + 35);
        glEnd();
    }
}



bool isDirectionSafe(int gx, int gy, int dx, int dy) {
    // Проверка на возможность поворота на перекрестке (чтобы на встречку не повернуть)
    int cx = gx + dx;
    int cy = gy + dy;

    // Проход во все стороны от плитки перекрестка
    while (cx >= 0 && cx < MAP_WIDTH && cy >= 0 && cy < MAP_HEIGHT) {
        int tile = gameMap[cy][cx];

        if (tile == TILE_INTERSECT || tile == TILE_TRAFFIC_LIGHT_GREEN) {
            // Если перекресток или зеленый
            cx += dx;
            cy += dy;
            continue;
        }

        // Если дорога, смотрим, чтобы не встречка
        if (dx == 1 && tile == TILE_ROAD_RIGHT) return true;
        if (dx == -1 && tile == TILE_ROAD_LEFT)  return true;
        if (dy == -1 && tile == TILE_ROAD_UP)    return true;
        if (dy == 1 && tile == TILE_ROAD_DOWN)  return true;

        if (tile == TILE_SPAWN) return true; // Спавнер

        if (tile == TILE_TRAFFIC_LIGHT_GREEN) return true; // Светофор

        // Если встречка или красный
        return false;
    }
    return false;
}

void updateVehicles(float dt) {
    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (!vehicles[i].active) continue;
        Vehicle* v = &vehicles[i];

        // Текущая клетка и следующая по курсу
        int currGx = (int)(v->x / 40.0f);
        int currGy = (int)(v->y / 40.0f);
        int nextGx = currGx + v->dirX;
        int nextGy = currGy + v->dirY;

        // Проверка, нужно ли остановиться перед красным светофором
        int mustStopForRed = 0;
        if (nextGx >= 0 && nextGx < MAP_WIDTH && nextGy >= 0 && nextGy < MAP_HEIGHT) {
            if (gameMap[nextGy][nextGx] == TILE_TRAFFIC_LIGHT_RED) {
                mustStopForRed = 1;
            }
        }

        // Расстояние до точки остановки (центр текущей клетки)
        float stopX = currGx * 40.0f + 20.0f;
        float stopY = currGy * 40.0f + 20.0f;
        float distToStop = 0.0f;
        if (v->dirX > 0)      distToStop = stopX - v->x;
        else if (v->dirX < 0) distToStop = v->x - stopX;
        else if (v->dirY > 0) distToStop = stopY - v->y;
        else if (v->dirY < 0) distToStop = v->y - stopY;

        // Проверка дистанции до других машин
        float distToCar = 10000.0f;
        for (int j = 0; j < MAX_VEHICLES; j++) {
            if (i == j || !vehicles[j].active) continue;
            Vehicle* other = &vehicles[j];
            float dx = other->x - v->x;
            float dy = other->y - v->y;
            if ((v->dirX > 0 && dx > 0 && fabsf(dy) < 15.0f) ||
                (v->dirX < 0 && dx < 0 && fabsf(dy) < 15.0f) ||
                (v->dirY > 0 && dy > 0 && fabsf(dx) < 15.0f) ||
                (v->dirY < 0 && dy < 0 && fabsf(dx) < 15.0f))
            {
                float dist = sqrtf(dx * dx + dy * dy) - 16.0f;
                if (dist < distToCar) distToCar = dist;
            }
        }

        // Определяем желаемую скорость
        float desiredSpeed = VEHICLE_MAX_SPEED;
        float obstacleDist = distToCar;

        if (mustStopForRed) {
            // Если впереди красный, точка остановки - центр текущей клетки
            if (distToStop < obstacleDist) obstacleDist = distToStop;
        }

        // Ограничение скорости по тормозному пути
        if (obstacleDist < 1000.0f) {
            float maxSafe = sqrtf(2.0f * VEHICLE_DECELERATION * obstacleDist);
            if (maxSafe < desiredSpeed) desiredSpeed = maxSafe;
        }
        if (obstacleDist <= 2.0f) desiredSpeed = 0.0f;

        // Плавное изменение скорости
        if (v->speed < desiredSpeed) {
            v->speed += VEHICLE_ACCELERATION * dt;
            if (v->speed > desiredSpeed) v->speed = desiredSpeed;
        }
        else if (v->speed > desiredSpeed) {
            v->speed -= VEHICLE_DECELERATION * dt;
            if (v->speed < desiredSpeed) v->speed = desiredSpeed;
        }

        // Авария при столкновении с другой машиной
        if (distToCar < 2.0f && v->speed > 10.0f) {
            for (int j = 0; j < MAX_VEHICLES; j++) {
                if (i == j || !vehicles[j].active) continue;
                Vehicle* other = &vehicles[j];
                float dx = other->x - v->x;
                float dy = other->y - v->y;
                if ((v->dirX > 0 && dx > 0 && fabsf(dy) < 15.0f) ||
                    (v->dirX < 0 && dx < 0 && fabsf(dy) < 15.0f) ||
                    (v->dirY > 0 && dy > 0 && fabsf(dx) < 15.0f) ||
                    (v->dirY < 0 && dy < 0 && fabsf(dx) < 15.0f))
                {
                    other->active = 0;
                    break;
                }
            }
            v->active = 0;
            continue;
        }

        // Движение (если не упираемся в красный светофор на нулевой скорости)
        float move = v->speed * dt;
        float prevX = v->x, prevY = v->y;
        if (!(mustStopForRed && fabs(distToStop) < 1.0f && v->speed < 1.0f)) {
            v->x += v->dirX * move;
            v->y += v->dirY * move;
        }

        // Проверка выхода за границы / на траву
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

        // Пересечение центра клетки
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

            // Выбор направления в зависимости от типа клетки
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
                        int choice = validIndices[rand() % validCount]; // равновероятный выбор
                        float newDirX = (float)possibleDirs[choice][0];
                        float newDirY = (float)possibleDirs[choice][1];
                        if (newDirX != v->dirX || newDirY != v->dirY)
                            v->canTurn = 0;
                        v->dirX = newDirX;
                        v->dirY = newDirY;
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

            // Небольшой сдвиг, чтобы не застрять
            v->x += v->dirX * 0.1f;
            v->y += v->dirY * 0.1f;
        }
    }
}

void drawVehicles() {
    // Визуализация машин - квадраты
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (!vehicles[i].active) continue;
        glBegin(GL_QUADS);
        glVertex2f(vehicles[i].x - 8, vehicles[i].y - 8);
        glVertex2f(vehicles[i].x + 8, vehicles[i].y - 8);
        glVertex2f(vehicles[i].x + 8, vehicles[i].y + 8);
        glVertex2f(vehicles[i].x - 8, vehicles[i].y + 8);
        glEnd();
    }
}

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

void render(void) {
    // Основной метод отрисовки кадра
    glClearColor(0.01f, 0.05f, 0.03f, 1.0f); // Темно-зеленый
    glClear(GL_COLOR_BUFFER_BIT);

    if (showDebugGrid) drawDebugGrid(); // Дебаг сетка

    updateButtons(); // Обработка нажатий на кнопки

    // Отрисовка МЕНЮ
    if (currentState == STATE_MENU) {
        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(fontBaseTitle, GX(5.6f), GY(3), "TRAFFIC SIMULATOR");
        for (int i = 0; i < 5; i++) {
            int hovered = (mouseX > buttons[i].x && mouseX < buttons[i].x + buttons[i].width &&
                mouseY > buttons[i].y && mouseY < buttons[i].y + buttons[i].height);
            drawButton(&buttons[i], i, hovered);
        }
    }
    // Отрисовка ВЫБОРА УРОВНЯ
    else if (currentState == STATE_LEVEL_SELECT) {
        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(fontBaseTitle, GX(2.8f), GY(3), "SELECT DIFFICULTY LEVEL");
        for (int i = 0; i < 3; i++) {
            int hovered = (mouseX > levelButtons[i].x && mouseX < levelButtons[i].x + levelButtons[i].width &&
                mouseY > levelButtons[i].y && mouseY < levelButtons[i].y + levelButtons[i].height);
            drawButton(&levelButtons[i], i, hovered);
        }
    }
    // Отрисовка СИМУЛЯЦИИ
    else if (currentState == STATE_SIMULATION) {

        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                drawTile(x, y, gameMap[y][x]);
            }
        }
        drawVehicles();

        // Отладка: показать скорость первой активной машины
        for (int i = 0; i < MAX_VEHICLES; i++) {
            if (vehicles[i].active) {
                char speedText[32];
                sprintf(speedText, "Speed: %.1f px/s", vehicles[i].speed);
                drawText(fontBase, 30, 190, speedText);
                break;
            }
        }

        // HUD
        if (isEditMode) {
            glColor3f(1.0f, 0.5f, 0.0f);
            drawText(fontBase, 30, 30, "MODE: MAP EDITOR");
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(fontBase, 30, 70, "PRESS [SPACE] TO START SIMULATION");
        }
        else {
            glColor3f(0.0f, 1.0f, 0.0f);
            drawText(fontBase, 30, 30, "MODE: SIMULATION");
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(fontBase, 30, 70, "CLICK TRAFFIC LIGHTS TO TOGGLE");
        }

        char levelText[50];
        sprintf(levelText, "CURRENT LEVEL: %d", currentLevel);
        drawText(fontBase, 30, 110, levelText);

        // Текущая выбранная плитка
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
}