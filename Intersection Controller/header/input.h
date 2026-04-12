#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

// Инициализация кнопок
void initButtons(void);
void initLevelButtons(void);

// Callback функци ввода
void cursor_position_callback(GLFWwindow* w, double x, double y);
void mouse_button_callback(GLFWwindow* w, int button, int action, int mods);
void key_callback(GLFWwindow* w, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* w, int width, int height);

#endif