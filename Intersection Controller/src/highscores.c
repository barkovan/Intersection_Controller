#define _CRT_SECURE_NO_WARNINGS
#include "../header/common.h"
#include <stdio.h>
#include <stdlib.h>

ScoreNode* highScoresList[3] = { NULL, NULL, NULL };

// Вставка в список с сохранением порядка (чем меньше время, тем выше в списке)
void addHighScore(int level, float time) {
    if (level < 1 || level > 3) return;
    int idx = level - 1;

    ScoreNode* newNode = (ScoreNode*)malloc(sizeof(ScoreNode));
    newNode->time.timeSeconds = time;
    newNode->next = NULL;

    // Логика приоритетной очереди: ищем место для вставки
    if (highScoresList[idx] == NULL || time < highScoresList[idx]->time.timeSeconds) {
        newNode->next = highScoresList[idx];
        highScoresList[idx] = newNode;
    }
    else {
        ScoreNode* curr = highScoresList[idx];
        while (curr->next != NULL && curr->next->time.timeSeconds <= time) {
            curr = curr->next;
        }
        newNode->next = curr->next;
        curr->next = newNode;
    }

    // Оставляем только топ-3, остальное удаляем из памяти
    ScoreNode* curr = highScoresList[idx];
    int count = 1;
    while (curr != NULL && count < 3) {
        curr = curr->next;
        count++;
    }

    if (curr != NULL && curr->next != NULL) {
        ScoreNode* temp = curr->next;
        curr->next = NULL;
        while (temp != NULL) {
            ScoreNode* toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
    }
}

void saveHighScores() {
    FILE* f = fopen("highscores.dat", "wb");
    if (!f) return;

    for (int i = 0; i < 3; i++) {
        ScoreNode* curr = highScoresList[i];
        int count = 0;
        float times[3] = { 0 };

        while (curr && count < 3) {
            times[count] = curr->time.timeSeconds;
            curr = curr->next;
            count++;
        }

        fwrite(&count, sizeof(int), 1, f);
        if (count > 0) fwrite(times, sizeof(float), count, f);
    }
    fclose(f);
}

void loadHighScores() {
    FILE* f = fopen("highscores.dat", "rb");
    if (!f) return;

    for (int i = 0; i < 3; i++) {
        int count = 0;
        if (fread(&count, sizeof(int), 1, f) != 1) break;
        float times[3];
        if (count > 0) {
            fread(times, sizeof(float), count, f);
            for (int j = 0; j < count; j++) addHighScore(i + 1, times[j]);
        }
    }
    fclose(f);
}
