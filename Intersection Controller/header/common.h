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
    const char* type;   // "Menu", "Level" или "Endgame"
} Button;


// Общие текстуры
extern GLuint uiButtonTex;        // обычная кнопка
extern GLuint uiButtonHoverTex;   // кнопка при наведении
extern GLuint titlePlateTex;      // клякса для заголовка меню
extern GLuint helpBgTex;          // фоновая клякса (окно помощи)
extern GLuint endgameBgTex;       // фоновая клякса (окно конца игры)
extern GLuint carTex;             // текстура автомобиля


extern GLuint houseTex;           // текстура дома
extern GLuint houseBrownTex;
extern GLuint houseBlackTex;
extern GLuint houseBlack2Tex;


extern GLuint treeTex;            // текстура дерева
extern GLuint treeOrangeTex;
extern GLuint treeGreenTex;
extern GLuint treeRedTex;
extern GLuint treeBlackgreenTex;

extern GLuint bushHorTex;         // изгородь
extern GLuint bushVerTex;
extern GLuint bushVerrightTex;
extern GLuint bushLeftupTex;
extern GLuint bushRightupTex;
extern GLuint bushRightdownTex;

// Глобальные переменные
extern GameState currentState;
extern GLFWwindow* window;

extern int mouseX, mouseY;

extern int currentLevel;
extern int isPauseMode;

extern float gameTimer; // таймер
extern bool isEndgame;

extern int carsPassedCount; // количество проехавших машин
extern int lives; // количество жизней

// Шрифты
extern GLuint fontBaseTitle;
extern GLuint fontBase;
extern GLuint fontBaseHov;

// UI-элементы
extern Button buttons[5];
extern Button levelButtons[4];
extern Button endgameButtons[2];
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
extern int isBonusPending[MAP_HEIGHT][MAP_WIDTH];

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
    TILE_SPAWN,
    TILE_HOUSE,
    TILE_HOUSE_BROWN,
    TILE_HOUSE_BLACK,
    TILE_HOUSE_BLACK2,
    TILE_TREE,
    TILE_TREE_ORANGE,
    TILE_TREE_GREEN,
    TILE_TREE_RED,
    TILE_TREE_BLACKGREEN,
    TILE_BUSH_HOR,
    TILE_BUSH_VER,
    TILE_BUSH_VERRIGHT,
    TILE_BUSH_LEFTUP,
    TILE_BUSH_RIGHTUP,
    TILE_BUSH_RIGHTDOWN,
    TILE_SIDEWALK
} TileType;

// Светофоры
extern float trafficLightTimer[MAP_HEIGHT][MAP_WIDTH];
extern bool  yellowToGreen[MAP_HEIGHT][MAP_WIDTH];

// Физика автомобилей
#define VEHICLE_MAX_SPEED       120.0f
#define VEHICLE_ACCELERATION    200.0f
#define VEHICLE_DECELERATION    300.0f

extern float spawnMinDelay[MAP_HEIGHT][MAP_WIDTH];
extern float spawnMaxDelay[MAP_HEIGHT][MAP_WIDTH];

typedef struct Vehicle {
    float x, y;
    float speed;
    int dirX, dirY;
    int canTurn;
    struct Vehicle* next;
    struct Vehicle* prev;
} Vehicle;

extern Vehicle* vehicleList;

// Для сохранения состояния игры
#define MAX_VEHICLES 20

// Структура для сохранения
typedef struct {
    int level;
    float gameTimer;
    int carsPassed;
    int lives;
    int map[MAP_HEIGHT][MAP_WIDTH];
    float trafficTimer[MAP_HEIGHT][MAP_WIDTH];
    bool  yellowToGreenMap[MAP_HEIGHT][MAP_WIDTH];
    float spawnTimersMap[MAP_HEIGHT][MAP_WIDTH];
    int vehicleCount;
    struct {
        float x, y;
        float speed;
        int dirX, dirY;
        int canTurn;
    } vehicles[MAX_VEHICLES];
} GameSave;

void saveGame(const char* filename);
bool loadGame(const char* filename);

#endif // COMMON_H