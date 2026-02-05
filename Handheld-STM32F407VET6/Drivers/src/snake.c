#include "snake.h"
#include "ili9341.h"
#include "images.h"
#include <stdlib.h>

static Snake snake[SNAKE_MAX_LENGTH];
static int snakelen;
static int current_dir;
static int apple_x;
static int apple_y;
static bool isover;
static int num_apple;
static int num_body;

static void genNewApple(void) {
    bool onSnake = false;
    do {
        apple_x = ((rand() % (SNAKE_PLAY_AREA_WIDTH / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE) + SNAKE_AREA_X_START;
        apple_y = ((rand() % (SNAKE_PLAY_AREA_HEIGHT / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE) + SNAKE_AREA_Y_START;
        onSnake = false;
        for (int i = 0; i < snakelen; i++) {
            if (snake[i].x == apple_x && snake[i].y == apple_y) {
                onSnake = true;
                break;
            }
        }
        num_apple = (rand() % 4) + 1;
    } while (onSnake);
}

void SNAKE_Init(void) {
    srand(HAL_GetTick());
    snakelen = 3;
    current_dir = 1;
    isover = false;
    num_body = 1;

    for (int i = 0; i < snakelen; i++) {
        snake[i].x = 180 - (i * SNAKE_BLOCK_SIZE);
        snake[i].y = 100;
    }
    genNewApple();
}

void SNAKE_Reset(void) {
    SNAKE_Init();
}

void SNAKE_Update(InputDirection direction) {
    int new_dir = current_dir;

    if (direction == INPUT_RIGHT) new_dir = 1;
    else if (direction == INPUT_DOWN) new_dir = 2;
    else if (direction == INPUT_LEFT) new_dir = 3;
    else if (direction == INPUT_UP) new_dir = 4;

    if (new_dir == 1 && current_dir == 3) return;
    if (new_dir == 3 && current_dir == 1) return;
    if (new_dir == 4 && current_dir == 2) return;
    if (new_dir == 2 && current_dir == 4) return;

    current_dir = new_dir;

    int last_x = snake[snakelen - 1].x;
    int last_y = snake[snakelen - 1].y;

    for (int i = snakelen - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }

    if (current_dir == 1) snake[0].x += SNAKE_BLOCK_SIZE;
    else if (current_dir == 2) snake[0].y += SNAKE_BLOCK_SIZE;
    else if (current_dir == 3) snake[0].x -= SNAKE_BLOCK_SIZE;
    else if (current_dir == 4) snake[0].y -= SNAKE_BLOCK_SIZE;

    if (snake[0].x < SNAKE_AREA_X_START)
        snake[0].x = SNAKE_AREA_X_START + SNAKE_PLAY_AREA_WIDTH - SNAKE_BLOCK_SIZE;
    else if (snake[0].x >= SNAKE_AREA_X_START + SNAKE_PLAY_AREA_WIDTH)
        snake[0].x = SNAKE_AREA_X_START;

    if (snake[0].y < SNAKE_AREA_Y_START)
        snake[0].y = SNAKE_AREA_Y_START + SNAKE_PLAY_AREA_HEIGHT - SNAKE_BLOCK_SIZE;
    else if (snake[0].y >= SNAKE_AREA_Y_START + SNAKE_PLAY_AREA_HEIGHT)
        snake[0].y = SNAKE_AREA_Y_START;

    if (snake[0].x == apple_x && snake[0].y == apple_y) {
        if (snakelen < SNAKE_MAX_LENGTH) {
            snake[snakelen].x = last_x;
            snake[snakelen].y = last_y;
            snakelen++;
        }
        num_body = num_apple;
        genNewApple();
    } else {
        ILI9341_FillRectangle(last_x, last_y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, ILI9341_BLACK);
    }

    for (int i = 1; i < snakelen; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            isover = true;
            break;
        }
    }
}

void SNAKE_Draw(void) {
    if (num_apple == 1) ILI9341_DrawImage(apple_x, apple_y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body1);
    else if (num_apple == 2) ILI9341_DrawImage(apple_x, apple_y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body2);
    else if (num_apple == 3) ILI9341_DrawImage(apple_x, apple_y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body3);
    else if (num_apple == 4) ILI9341_DrawImage(apple_x, apple_y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body4);

    if (current_dir == 1) ILI9341_DrawImage(snake[0].x, snake[0].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, head1);
    else if (current_dir == 2) ILI9341_DrawImage(snake[0].x, snake[0].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, head2);
    else if (current_dir == 3) ILI9341_DrawImage(snake[0].x, snake[0].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, head3);
    else if (current_dir == 4) ILI9341_DrawImage(snake[0].x, snake[0].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, head4);

    for (int i = 1; i < snakelen; i++) {
        int body_img_index = num_body - 1;
        if (body_img_index == 0) ILI9341_DrawImage(snake[i].x, snake[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body1);
        else if (body_img_index == 1) ILI9341_DrawImage(snake[i].x, snake[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body2);
        else if (body_img_index == 2) ILI9341_DrawImage(snake[i].x, snake[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body3);
        else if (body_img_index == 3) ILI9341_DrawImage(snake[i].x, snake[i].y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE, body4);
    }
}

bool SNAKE_IsGameOver(void) {
    return isover;
}
