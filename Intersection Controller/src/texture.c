#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

GLuint loadTexture(const char* filename) {

    HBITMAP hBitmap = (HBITMAP)LoadImageA(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBitmap) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    int width = bm.bmWidth;
    int height = bm.bmHeight;

    HDC hdc = CreateCompatibleDC(NULL);
    SelectObject(hdc, hBitmap);

    // Выделяем память под пиксели (32 бита RGBA)
    unsigned char* data = (unsigned char*)malloc(width * height * 4);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Отрицательная высота – сверху вниз
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    GetDIBits(hdc, hBitmap, 0, height, data, &bmi, DIB_RGB_COLORS);

    DeleteDC(hdc);
    DeleteObject(hBitmap);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);
    return textureID;
}