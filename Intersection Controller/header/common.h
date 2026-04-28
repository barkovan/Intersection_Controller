#ifndef COMMON_H
#define COMMON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>

// Игровые состояния
typedef enum {
    STATE_MENU,
    STATE_LEVEL_SELECT,
    STATE_SIMULATION,
    STATE_HELP,
    STATE_HIGHSCORES,
    STATE_EXIT
} GameState;

// Кнопка
typedef struct {
    float x, y;
    float width, height;
    const char* text;
    const char* type;
} Button;

// Текстуры кнопок
extern GLuint uiButtonTex;
extern GLuint uiButtonHoverTex;
extern GLuint titlePlateTex;

extern GLuint carTex;

// Функция загрузки текстуры
GLuint loadTexture(const char* filename);


// Глобальные переменные
extern GameState currentState;
extern GLFWwindow* window;

extern int mouseX, mouseY;
extern int currentLevel;

// Флаг режима редактирования (1 - Edit, 0 - Play)
extern int isEditMode;

// Шрифты
extern GLuint fontBaseTitle;
extern GLuint fontBase;
extern GLuint fontBaseHov;

// UI элементы
extern Button buttons[5];
extern Button levelButtons[4];
extern float buttonScale[10];

// Сетка для UI
#define GRID_SIZE 40
#define GX(col) ((col) * GRID_SIZE)
#define GY(row) ((row) * GRID_SIZE)

extern int showDebugGrid;

// Карта
#define MAP_WIDTH 32
#define MAP_HEIGHT 18

// Желтый у светофора
#define YELLOW_DURATION 1.0

typedef enum {
    TILE_GRASS = 0,
    TILE_ROAD_RIGHT,
    TILE_ROAD_LEFT,
    TILE_ROAD_UP,
    TILE_ROAD_DOWN,
    TILE_TRAFFIC_LIGHT_GREEN, // Светофор
    TILE_TRAFFIC_LIGHT_YELLOW,
    TILE_TRAFFIC_LIGHT_RED,
    TILE_INTERSECT, // Перекресток
    TILE_SPAWN       // Точка спавна

} TileType;

extern int gameMap[MAP_HEIGHT][MAP_WIDTH];

// для светофора
extern float trafficLightTimer[MAP_HEIGHT][MAP_WIDTH];
extern bool yellowToGreen[MAP_HEIGHT][MAP_WIDTH];

// Общие настройки физики (пока без уровней)
#define VEHICLE_MAX_SPEED       120.0f  // пикселей в секунду
#define VEHICLE_ACCELERATION    200.0f
#define VEHICLE_DECELERATION    250.0f
#define VEHICLE_SPAWN_INTERVAL  3.0f    // секунд между спавнами

// Машина
typedef struct {
    float x, y;      // Координаты
    float speed;     // Текущая скорость (пикселей в секунду)
    int dirX, dirY;  // Направление (-1, 0, 1)
    int active;      // Активна ли
    int canTurn;     // Может ли повернуть
} Vehicle;

#define MAX_VEHICLES 20
extern Vehicle vehicles[MAX_VEHICLES];
extern int currentBrush; // Текущая плитка

#endif // COMMON_H
