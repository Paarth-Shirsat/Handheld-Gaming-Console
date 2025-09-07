/*
 * snake.c
 *
 * Author: mustafa
 */
#include "snake.h"
#include "main.h"
#include "ili9341.h"
#include "testimg.h"
#include <stdlib.h>
#include <stdbool.h>

Snake snake[MAX_LEN];
int snakelen, current_dir, apple_x, apple_y;
bool isover;
int num_colors = 4;
int num_apple = 1;
int num_body = 1;

void genNewApple(void) {
    bool onSnake = false;
    do {
        // Aligned to 20x20 grid, with padding
        apple_x = ((rand() % (160 / 20)) * 20) + 40;
        apple_y = ((rand() % (120 / 20)) * 20) + 40;
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
    for (int i = 0; i < snakelen; i++) {
        snake[i].x = 180 - (i * 20);
        snake[i].y = 100;
    }
    genNewApple();
}

void SNAKE_updateGame(void) {
    // Save last tail segment position before updating
    int last_x = snake[snakelen - 1].x;
    int last_y = snake[snakelen - 1].y;

    // Shift body segments forward
    for (int i = snakelen - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }

    // Move head
    if (current_dir == 1) snake[0].x += 20; // right
    else if (current_dir == 2) snake[0].y += 20; // down
    else if (current_dir == 3) snake[0].x -= 20; // left
    else if (current_dir == 4) snake[0].y -= 20; // up

    // Wrap around logic
    if (snake[0].x < 40) snake[0].x = 280;
    else if (snake[0].x >= 280) snake[0].x = 40;

    if (snake[0].y < 40) snake[0].y = 200;
    else if (snake[0].y >= 200) snake[0].y = 40;


    // Check for apple collision
    if (snake[0].x == apple_x && snake[0].y == apple_y) {
        if (snakelen < MAX_LEN) {
            // Extend the snake
            snake[snakelen].x = last_x;
            snake[snakelen].y = last_y;
            snakelen++;
        }
        num_body = num_apple;
        genNewApple();
    } else {
        // Clear the last tail segment if no apple was eaten
        ILI9341_FillRectangle(last_x, last_y, 20, 20, ILI9341_BLACK);
    }

    // Check for self-collision
    for (int i = 1; i < snakelen; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            isover = true;
            break;
        }
    }
}

void SNAKE_drawGame(void) {
    // Draw the new apple
    if (num_apple == 1) ILI9341_DrawImage(apple_x, apple_y, 20, 20, body1);
    else if (num_apple == 2) ILI9341_DrawImage(apple_x, apple_y, 20, 20, body2);
    else if (num_apple == 3) ILI9341_DrawImage(apple_x, apple_y, 20, 20, body3);
    else if (num_apple == 4) ILI9341_DrawImage(apple_x, apple_y, 20, 20, body4);

    // Draw the head
    if (current_dir == 1) ILI9341_DrawImage(snake[0].x, snake[0].y, 20, 20, head1);
    else if (current_dir == 2) ILI9341_DrawImage(snake[0].x, snake[0].y, 20, 20, head2);
    else if (current_dir == 3) ILI9341_DrawImage(snake[0].x, snake[0].y, 20, 20, head3);
    else if (current_dir == 4) ILI9341_DrawImage(snake[0].x, snake[0].y, 20, 20, head4);

    // Draw the body
    for (int i = 1; i < snakelen; i++) {
        int body_img_index = (num_body - 1);
        if (body_img_index == 0) ILI9341_DrawImage(snake[i].x, snake[i].y, 20, 20, body1);
        else if (body_img_index == 1) ILI9341_DrawImage(snake[i].x, snake[i].y, 20, 20, body2);
        else if (body_img_index == 2) ILI9341_DrawImage(snake[i].x, snake[i].y, 20, 20, body3);
        else if (body_img_index == 3) ILI9341_DrawImage(snake[i].x, snake[i].y, 20, 20, body4);
    }
}

void SNAKE_ChangeDirection(int new_dir) {
    if (new_dir == 1 && current_dir == 3) return; // Right vs Left
    if (new_dir == 3 && current_dir == 1) return; // Left vs Right
    if (new_dir == 4 && current_dir == 2) return; // Up vs Down
    if (new_dir == 2 && current_dir == 4) return; // Down vs Up

    current_dir = new_dir;
}

bool SNAKE_getOver(void) {
    return isover;
}
