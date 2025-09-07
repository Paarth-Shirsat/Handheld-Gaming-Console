/*
 * snake.h
 *
 *      Author: mustafa
 */

#ifndef GAME_SNAKE_H_
#define GAME_SNAKE_H_

#include <stdbool.h>

typedef struct {
	int x ,y ;
}Snake;
#define WIDTH 320
#define HEIGHT 240
#define BLOCK_SIZE 10
#define MAX_LEN 100

void SNAKE_Init();
void SNAKE_updateGame(void);
void SNAKE_drawGame(void);
void SNAKE_ChangeDirection(int new_dir);
bool SNAKE_getOver(void);





#endif /* GAME_SNAKE_H_ */
