#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>

#include "../header/common.h"
#include "../header/render.h"
#include "../header/input.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// === ИНИЦИАЛИЗАЦИЯ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ ===
GameState currentState = STATE_MENU;
GLFWwindow* window = NULL;

int mouseX = 0, mouseY = 0;
int currentLevel = 1;

GLuint fontBaseTitle = 0;
GLuint fontBase = 0;
GLuint fontBaseHov = 0;

Button buttons[5];
Button levelButtons[3];
float buttonScale[10] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

int wWidth = 1280, wHeight = 720;
int showDebugGrid = 0; // Флаг дебаг сетки
int isEditMode = 1; // Со старта режим редактирования

int gameMap[MAP_HEIGHT][MAP_WIDTH] = { 0 }; // Матрица карты

Vehicle vehicles[MAX_VEHICLES] = { 0 };
int currentBrush = TILE_ROAD_RIGHT; // Плитка по умолчанию

void error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error: %s\n", desc);
}

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    // Инициализация GLUT
    glutInit(&argc, argv);

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Статичный размер окна 1280x720
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

    // Callback функция по изменению размера окна (пока не нужно)
    framebuffer_size_callback(window, wWidth, wHeight);

    // Инициализация элементов UI
    buildTitleFont();
    buildBaseFont();
    buildBaseFontHov();
    initButtons();

    // Callback фукнции ввода
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    printf("Traffic Simulator started successfully.\n");

    while (!glfwWindowShouldClose(window) && currentState != STATE_EXIT) {
        if (currentState == STATE_SIMULATION) {
            if (!isEditMode) {
                updateVehicles(); // Обновление машин
                spawnLogic();     // Обработка спавна машин
            }
        }
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (fontBaseTitle) glDeleteLists(fontBaseTitle, 256);
    if (fontBase) glDeleteLists(fontBase, 256);
    if (fontBaseHov) glDeleteLists(fontBaseHov, 256);

    RemoveFontResourceEx(TEXT("../font/BeatMark-Regular.ttf"), FR_PRIVATE, NULL);

    glfwTerminate();
    printf("Traffic Simulator closed.\n");

    return 0;
}