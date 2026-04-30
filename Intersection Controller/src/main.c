#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include "../header/common.h"
#include "../header/render.h"
#include "../header/input.h"
#include "../header/vehicle.h"

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
int isEditMode = 1;          // 1 – редактор, 0 – симуляция

// Шрифты
GLuint fontBaseTitle = 0;
GLuint fontBase = 0;
GLuint fontBaseHov = 0;

// UI
Button buttons[5];
Button levelButtons[4];
float buttonScale[10] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

// Текстуры
GLuint uiButtonTex = 0;
GLuint uiButtonHoverTex = 0;
GLuint titlePlateTex = 0;
GLuint helpBgTex = 0;
GLuint carTex = 0;
GLuint houseTex = 0;
GLuint treeTex = 0;


// Карта
int gameMap[MAP_HEIGHT][MAP_WIDTH] = { 0 };
Vehicle vehicles[MAX_VEHICLES] = { 0 };
int currentBrush = TILE_ROAD_RIGHT;

// Таймеры спавнеров
float spawnTimers[MAP_HEIGHT][MAP_WIDTH] = { 0 };

// Светофоры
float trafficLightTimer[MAP_HEIGHT][MAP_WIDTH] = { 0.0 };
bool yellowToGreen[MAP_HEIGHT][MAP_WIDTH] = { false };

// Время
float deltaTime = 0.0f;

// Размеры окна (пока фиксированы)
int wWidth = 1280, wHeight = 720;

// Отладка
int showDebugGrid = 0;

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

    // Включаем поддержку прозрачности
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Загрузка текстур
    uiButtonTex = loadTexture("assets/button_normal.png");
    uiButtonHoverTex = loadTexture("assets/button_hover.png");
    titlePlateTex = loadTexture("assets/title_plate.png");
    helpBgTex = loadTexture("assets/help_bg.png");
    carTex = loadTexture("assets/car.png");
    houseTex = loadTexture("assets/house.png");
    treeTex = loadTexture("assets/tree.png");

    if (carTex == 0) printf("Error: failed to load car texture\n");

    // Начальная настройка проекции
    framebuffer_size_callback(window, wWidth, wHeight);

    // Шрифты и кнопки
    buildTitleFont();
    buildBaseFont();
    buildBaseFontHov();
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

        if (currentState == STATE_SIMULATION && !isEditMode) {
            updateVehicles(deltaTime);
            spawnLogic(deltaTime);
            updateTrafficLights();
        }

        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
        limit_fps(60);
    }

    // Очистка шрифтов
    if (fontBaseTitle) glDeleteLists(fontBaseTitle, 256);
    if (fontBase)      glDeleteLists(fontBase, 256);
    if (fontBaseHov)   glDeleteLists(fontBaseHov, 256);

    RemoveFontResourceEx(TEXT("../font/BeatMark-Regular.ttf"), FR_PRIVATE, NULL);

    glfwTerminate();
    printf("Traffic Simulator closed.\n");
    return 0;
}