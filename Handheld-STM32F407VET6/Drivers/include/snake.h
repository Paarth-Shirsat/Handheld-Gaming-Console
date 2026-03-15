#ifndef GAME_SNAKE_H_
#define GAME_SNAKE_H_

#include <stdbool.h>
#include "input.h"

#define SNAKE_PLAY_AREA_WIDTH 240
#define SNAKE_PLAY_AREA_HEIGHT 160
#define SNAKE_BLOCK_SIZE 20
#define SNAKE_MAX_LENGTH 100
#define SNAKE_AREA_X_START 40
#define SNAKE_AREA_Y_START 40

typedef struct {
    int x;
    int y;
} Snake;

void SNAKE_Init(void);
void SNAKE_Update(InputDirection direction);
void SNAKE_Draw(void);
bool SNAKE_IsGameOver(void);
void SNAKE_Reset(void);

#endif
