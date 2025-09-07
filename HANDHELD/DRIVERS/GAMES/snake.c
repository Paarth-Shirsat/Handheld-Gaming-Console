/*
 * snake.c
 *
 *      Author: mustafa
 */
#include "snake.h"
#include "main.h"
#include "ili9341.h"
#include <stdlib.h>
#include <stdbool.h>

Snake snake[MAX_LEN];
int snakelen ,current_dir , apple_x , apple_y ;
bool isover;
uint16_t SNAKE_COLORS[] = {
    ILI9341_CYAN,
    ILI9341_YELLOW,
    ILI9341_GREEN,
    ILI9341_BLUE,
    ILI9341_MAGENTA,
    ILI9341_ORANGE,
    ILI9341_PURPLE,
    ILI9341_WHITE
};
int num_colors = 8;
void genNewApple(void) {
    bool onSnake = false;
    do {
        apple_x = (rand() % ((WIDTH)/10)*10);
        apple_y = (rand() % ((HEIGHT)/10)*10);
        for (int i = 0; i < snakelen; i++) {
            if (snake[i].x == apple_x && snake[i].y == apple_y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}
void SNAKE_Init(void) {
    srand(HAL_GetTick());
    snakelen = 3;
    current_dir = 1;
    isover = false;
    for (int i = 0; i < snakelen; i++) {
        snake[i].x = 60 - (i * 10);
        snake[i].y = 20;
    }
    genNewApple();
}
void SNAKE_updateGame(void) {
    int tail_x = snake[snakelen - 1].x;
    int tail_y = snake[snakelen - 1].y;

    for (int i = snakelen - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }

    if(current_dir == 1){
    	snake[0].x = snake[0].x + 10 ; //right
    }
    else if (current_dir == 2){
    	snake[0].y = snake[0].y +10; //down
    }
    else if (current_dir == 3)  snake[0].x = snake[0].x - 10 ;
    else if(current_dir == 4) snake[0].y = snake[0].y - 10;

    if (snake[0].x < 0) {
        snake[0].x = WIDTH - 10;
    }
    else if (snake[0].x >= WIDTH) {
        snake[0].x = 0;
    }
    if (snake[0].y < 0) {
        snake[0].y = HEIGHT - 10;
    }
    else if (snake[0].y >= HEIGHT) {
        snake[0].y = 0;
    }

    if (snake[0].x == apple_x && snake[0].y == apple_y) {
        if (snakelen < MAX_LEN) {
            snakelen++;
        }
        genNewApple();
        ILI9341_FillRectangle(tail_x, tail_y, 10, 10, ILI9341_BLACK);
    } else {
        ILI9341_FillRectangle(tail_x, tail_y, 10,10,ILI9341_BLACK);
    }

    for (int i = 1; i < snakelen; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            isover = true;
            break;
        }
    }
}
void SNAKE_drawGame(void) {
    ILI9341_FillRectangle(apple_x, apple_y, 10, 10, ILI9341_RED);

    for (int i = 0; i < snakelen; i++) {
        uint16_t temp_color = SNAKE_COLORS[i % num_colors];
        ILI9341_FillRectangle(snake[i].x, snake[i].y, 10,10,temp_color);
    }
}
void SNAKE_ChangeDirection(int new_dir) {
    if (new_dir == 1 && current_dir == 3) return;
    if (new_dir == 3 && current_dir == 1) return;
    if (new_dir == 4 && current_dir == 2) return;
    if (new_dir == 2 && current_dir == 4) return;

    current_dir = new_dir;
}
bool SNAKE_getOver(void){
	return isover;
}





