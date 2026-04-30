#ifndef COMMON_H
#define COMMON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

// Состояния программы
typedef enum {
    STATE_MENU,
    STATE_LEVEL_SELECT,
    STATE_SIMULATION,
    STATE_HELP,
    STATE_HIGHSCORES,
    STATE_EXIT
} GameState;

// Кнопка интерфейса
typedef struct {
    float x, y;
    float width, height;
    const char* text;
    const char* type;   // "Menu" или "Level"
} Button;

// Общие текстуры
extern GLuint uiButtonTex;        // обычная кнопка
extern GLuint uiButtonHoverTex;   // кнопка при наведении
extern GLuint titlePlateTex;      // клякса для заголовка меню
extern GLuint helpBgTex;          // фоновая клякса для экрана справки
extern GLuint carTex;             // текстура автомобиля

// Глобальные переменные
extern GameState currentState;
extern GLFWwindow* window;

extern int mouseX, mouseY;
extern int currentLevel;
extern int isEditMode;

// Шрифты
extern GLuint fontBaseTitle;
extern GLuint fontBase;
extern GLuint fontBaseHov;

// UI-элементы
extern Button buttons[5];
extern Button levelButtons[4];
extern float buttonScale[10];

// Сетка
#define GRID_SIZE 40
#define GX(col) ((col) * GRID_SIZE)
#define GY(row) ((row) * GRID_SIZE)

extern int showDebugGrid;

// Карта
#define MAP_WIDTH 32
#define MAP_HEIGHT 18
extern int gameMap[MAP_HEIGHT][MAP_WIDTH];

extern int currentBrush; // текущая плитка

#define YELLOW_DURATION 1.0   // длительность жёлтого сигнала (сек)

extern float spawnTimers[MAP_HEIGHT][MAP_WIDTH]; // таймеры спавнеров

typedef enum {
    TILE_GRASS = 0,
    TILE_ROAD_RIGHT,
    TILE_ROAD_LEFT,
    TILE_ROAD_UP,
    TILE_ROAD_DOWN,
    TILE_TRAFFIC_LIGHT_GREEN,
    TILE_TRAFFIC_LIGHT_YELLOW,
    TILE_TRAFFIC_LIGHT_RED,
    TILE_INTERSECT,
    TILE_SPAWN
} TileType;

// Светофоры
extern float trafficLightTimer[MAP_HEIGHT][MAP_WIDTH];
extern bool  yellowToGreen[MAP_HEIGHT][MAP_WIDTH];

// Физика автомобилей
#define VEHICLE_MAX_SPEED       120.0f
#define VEHICLE_ACCELERATION    200.0f
#define VEHICLE_DECELERATION    250.0f
#define VEHICLE_SPAWN_INTERVAL  3.0f

typedef struct {
    float x, y;
    float speed;
    int dirX, dirY;
    int active;
    int canTurn;
} Vehicle;

#define MAX_VEHICLES 20
extern Vehicle vehicles[MAX_VEHICLES];

#endif // COMMON_H