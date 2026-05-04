#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include "../header/common.h"
#include "../header/render.h"
#include "../header/input.h"
#include "../header/vehicle.h"
#include "../resource.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// ========== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ==========

// Состояние и окно
GameState currentState = STATE_MENU;
GLFWwindow* window = NULL;

// Ввод
int mouseX = 0, mouseY = 0;
int currentLevel = 1;
int isPauseMode = 1;          // 1 – редактор/пауза, 0 – симуляция

// Шрифты
GLuint fontBaseTitle = 0;
GLuint fontBase = 0;
GLuint fontBaseHov = 0;

// UI
Button buttons[5];
Button levelButtons[4];
Button endgameButtons[2];
float buttonScale[10] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

// Текстуры
GLuint uiButtonTex = 0;
GLuint uiButtonHoverTex = 0;
GLuint titlePlateTex = 0;
GLuint helpBgTex = 0;
GLuint endgameBgTex = 0;

GLuint carTextures[2] = { 0 };
int carTexCount = 0;

GLuint houseTex = 0;
GLuint houseBrownTex = 0;
GLuint houseBlackTex = 0;
GLuint houseBlack2Tex = 0;

GLuint treeTex = 0;
GLuint treeOrangeTex = 0;
GLuint treeGreenTex = 0;
GLuint treeRedTex = 0;
GLuint treeBlackgreenTex = 0;

GLuint bushHorTex = 0;
GLuint bushVerTex = 0;
GLuint bushVerrightTex = 0;
GLuint bushLeftupTex = 0;
GLuint bushRightupTex = 0;
GLuint bushRightdownTex = 0;

// Карта
int gameMap[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int currentBrush = TILE_ROAD_RIGHT;

// Таймеры спавнеров
float spawnTimers[MAP_HEIGHT][MAP_WIDTH] = { 0 };
int isBonusPending[MAP_HEIGHT][MAP_WIDTH] = { 0 };

float spawnMinDelay[MAP_HEIGHT][MAP_WIDTH] = { 0 };
float spawnMaxDelay[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// Светофоры
float trafficLightTimer[MAP_HEIGHT][MAP_WIDTH] = { 0.0 };
bool yellowToGreen[MAP_HEIGHT][MAP_WIDTH] = { false };

// Время и таймер
float deltaTime = 0.0f;

float gameTimer = 0.0f;
bool isEndgame = false;

// Кол-во проехавших машин и стартовые жизни
int carsPassedCount = 0;
int lives = 3;

// Размеры окна (пока фиксированы)
int wWidth = 1280, wHeight = 720;

// Отладка
int showDebugGrid = 0;

// Для автосохранения
float autoSaveTimer = 0.0f;

// ========== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ==========

// Колбэк ошибок GLFW
void error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error: %s\n", desc);
}

// Простое ограничение FPS через Sleep
void limit_fps(int target_fps) {
    static clock_t last_time = 0;
    clock_t current_time = clock();
    double target_frame_time = CLOCKS_PER_SEC / target_fps;
    double elapsed = current_time - last_time;

    if (last_time != 0 && elapsed < target_frame_time) {
        long sleep_ms = (long)((target_frame_time - elapsed) * 1000 / CLOCKS_PER_SEC);
        if (sleep_ms > 0) Sleep(sleep_ms);
    }
    last_time = current_time;
}

// ========== ТОЧКА ВХОДА ==========
int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    // Скрытие консольного окна
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);


    loadHighScores();

    glutInit(&argc, argv);
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(wWidth, wHeight, "Traffic Simulator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        glfwTerminate();
        return -1;
    }

    // Установка иконки
    HWND hwnd = FindWindowA("GLFW30", "Traffic Simulator");

    if (hwnd) {
        // Загружаем иконку из ресурсов EXE
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));

        if (hIcon) {
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        }
    }

    // Шрифты
    buildTitleFont();
    buildBaseFont();
    buildBaseFontHov();

    // Начальная настройка проекции
    framebuffer_size_callback(window, wWidth, wHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Черный фон
    glClear(GL_COLOR_BUFFER_BIT);

    // Окно загрузки
    drawText(fontBaseTitle, 460.0f, 360.0f, "LOADING...");
    glfwSwapBuffers(window);
    glFinish();

    // Включаем поддержку прозрачности
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Загрузка текстур
    uiButtonTex = loadTexture("assets/button_normal.png");
    uiButtonHoverTex = loadTexture("assets/button_hover.png");
    titlePlateTex = loadTexture("assets/title_plate.png");
    helpBgTex = loadTexture("assets/help_bg.png");
    endgameBgTex = loadTexture("assets/endgame_bg.png");

    carTextures[0] = loadTexture("assets/car.png");
    carTextures[1] = loadTexture("assets/car2.png");
    carTexCount = 2;

    houseTex = loadTexture("assets/house.png");
    houseBrownTex = loadTexture("assets/house_brown.png");
    houseBlackTex = loadTexture("assets/house_black.png");
    houseBlack2Tex = loadTexture("assets/house_black2.png");

    treeTex = loadTexture("assets/tree.png");
    treeOrangeTex = loadTexture("assets/tree_orange.png");
    treeGreenTex = loadTexture("assets/tree_green.png");
    treeRedTex = loadTexture("assets/tree_red.png");
    treeBlackgreenTex = loadTexture("assets/tree_blackgreen.png");

    bushHorTex = loadTexture("assets/bush_hor.png");
    bushVerTex = loadTexture("assets/bush_ver.png");
    bushVerrightTex = loadTexture("assets/bush_verright.png");
    bushLeftupTex = loadTexture("assets/bush_leftup.png");
    bushRightupTex = loadTexture("assets/bush_rightup.png");
    bushRightdownTex = loadTexture("assets/bush_rightdown.png");

    // Кнопки
    initButtons();

    // Колбэки ввода
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    printf("Traffic Simulator started successfully.\n");

    double lastTime = glfwGetTime();

    // Главный цикл
    while (!glfwWindowShouldClose(window) && currentState != STATE_EXIT) {
        double currentTime = glfwGetTime();
        deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;

        if (currentState == STATE_SIMULATION && !isPauseMode && !isEndgame) {
            updateVehicles(deltaTime);
            spawnLogic(deltaTime);
            updateTrafficLights();

            autoSaveTimer += deltaTime;
            if (autoSaveTimer >= 5.0f) {
                saveGame("save.dat");
                autoSaveTimer = 0.0f;
            }

            int targetCars;
            if (currentLevel == 1) targetCars = 20;      // Easy
            else if (currentLevel == 2) targetCars = 30;
            else if (currentLevel == 3) targetCars = 40;

            if (!isEndgame) gameTimer += deltaTime;

            if (lives < 1) {
                isEndgame = 1;
                initEndgameButtons();
            }

            if (carsPassedCount >= targetCars) {
                addHighScore(currentLevel, gameTimer);
                saveHighScores();
                isEndgame = 1;
                initEndgameButtons();
            }
        }

        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
        limit_fps(60);
    }

    saveGame("save.dat");

    // Очистка шрифтов
    if (fontBaseTitle) glDeleteLists(fontBaseTitle, 256);
    if (fontBase)      glDeleteLists(fontBase, 256);
    if (fontBaseHov)   glDeleteLists(fontBaseHov, 256);

    RemoveFontResourceEx(TEXT("../font/BeatMark-Regular.ttf"), FR_PRIVATE, NULL);

    clearAllVehicles();

    glfwTerminate();
    printf("Traffic Simulator closed.\n");
    return 0;
}