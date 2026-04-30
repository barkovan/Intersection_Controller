#include "../header/common.h"
#include "../header/input.h"
#include "../header/levels.h"
#include <stdio.h>

void initButtons(void) {
    // Инициализация кнопок меню
    float btnW = GX(12); // Ширина 480 пикселей
    float btnH = GY(1.5f); // Высота 60 пикселей
    float startX = GX(10); // Отступ

    buttons[0] = (Button){ startX, GY(10), btnW, btnH, "Start Simulation", "Menu" };
    buttons[1] = (Button){ startX, GY(11.5), btnW, btnH, "Load Saved Game", "Menu" };
    buttons[2] = (Button){ startX, GY(13), btnW, btnH, "High Scores", "Menu" };
    buttons[3] = (Button){ startX, GY(14.5), btnW, btnH, "Help", "Menu" };
    buttons[4] = (Button){ startX, GY(16), btnW, btnH, "Exit", "Menu" };
}

void initLevelButtons(void) {
    // Инициализация кнопок выбора уровня
    float size = GX(6); // Размер кнопки 240x240 пикселей

    levelButtons[0] = (Button){ GX(4),  GY(5), size, size, "Easy", "Level" };
    levelButtons[1] = (Button){ GX(13), GY(5), size, size, "Medium", "Level" };
    levelButtons[2] = (Button){ GX(22), GY(5), size, size, "Hard", "Level" };

    // Песочница
    levelButtons[3] = (Button){ GX(10), GY(13), GX(12), GY(2), "Sandbox", "Level"};
}

void cursor_position_callback(GLFWwindow* w, double x, double y) {
    mouseX = (int)x;
    mouseY = (int)y;
}

void mouse_button_callback(GLFWwindow* w, int button, int action, int mods) {
    if (action != GLFW_PRESS) return;

    if (currentState == STATE_MENU) {
        for (int i = 0; i < 5; i++) {
            if (mouseX > buttons[i].x && mouseX < buttons[i].x + buttons[i].width &&
                mouseY > buttons[i].y && mouseY < buttons[i].y + buttons[i].height) {

                if (i == 0 || i == 1) {
                    currentState = STATE_LEVEL_SELECT;
                    initLevelButtons();
                }
                else if (i == 2) currentState = STATE_HIGHSCORES;
                else if (i == 3) currentState = STATE_HELP;
                else if (i == 4) currentState = STATE_EXIT;
                return;
            }
        }
    }
    else if (currentState == STATE_LEVEL_SELECT) {
        for (int i = 0; i < 4; i++) {
            if (mouseX > levelButtons[i].x && mouseX < levelButtons[i].x + levelButtons[i].width &&
                mouseY > levelButtons[i].y && mouseY < levelButtons[i].y + levelButtons[i].height) {

                if (i < 3) {
                    currentLevel = i + 1;   // 1,2,3
                }
                else {
                    currentLevel = 0;       // песочница
                }

                loadLevel(currentLevel);

                currentState = STATE_SIMULATION;
                return;
            }
        }
    }
    else if (currentState == STATE_SIMULATION) {
        if (action == GLFW_PRESS) {
            int gridX = mouseX / 40;
            int gridY = mouseY / 40;

            if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
                if (isEditMode) {
                    // РЕЖИМ РЕДАКТИРОВАНИЯ: Рисуем дороги
                    if (button == GLFW_MOUSE_BUTTON_LEFT) {
                        gameMap[gridY][gridX] = currentBrush;
                    }
                    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                        gameMap[gridY][gridX] = TILE_GRASS;
                    }
                }
                else {
                    // РЕЖИМ СИМУЛЯЦИИ: Переключаем светофоры
                    if (button == GLFW_MOUSE_BUTTON_LEFT) {
                        int tile = gameMap[gridY][gridX];
                        if (tile == TILE_TRAFFIC_LIGHT_GREEN) {
                            gameMap[gridY][gridX] = TILE_TRAFFIC_LIGHT_YELLOW;
                            yellowToGreen[gridY][gridX] = false;
                            trafficLightTimer[gridY][gridX] = (float)glfwGetTime();
                        }
                        else if (tile == TILE_TRAFFIC_LIGHT_RED) {
                            gameMap[gridY][gridX] = TILE_TRAFFIC_LIGHT_YELLOW;
                            yellowToGreen[gridY][gridX] = true;
                            trafficLightTimer[gridY][gridX] = (float)glfwGetTime();
                        }
                    }
                }
            }
        }
    }
}

void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE) currentState = STATE_MENU;
    if (key == GLFW_KEY_F8) showDebugGrid = !showDebugGrid; // Дебаг сетка

    // Переключение режима по пробелу
    if (key == GLFW_KEY_SPACE) isEditMode = !isEditMode;

    // Переключение выбора плитки
    if (key == GLFW_KEY_1) currentBrush = TILE_ROAD_RIGHT;
    if (key == GLFW_KEY_2) currentBrush = TILE_ROAD_LEFT;
    if (key == GLFW_KEY_3) currentBrush = TILE_ROAD_UP;
    if (key == GLFW_KEY_4) currentBrush = TILE_ROAD_DOWN;
    if (key == GLFW_KEY_5) currentBrush = TILE_INTERSECT;
    if (key == GLFW_KEY_6) currentBrush = TILE_TRAFFIC_LIGHT_GREEN;
    if (key == GLFW_KEY_7) currentBrush = TILE_SPAWN;
}

void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}