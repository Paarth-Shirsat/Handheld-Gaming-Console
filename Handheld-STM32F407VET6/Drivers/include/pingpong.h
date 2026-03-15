#ifndef GAME_PINGPONG_H_
#define GAME_PINGPONG_H_

#include <stdbool.h>
#include "input.h"

#define PONG_PADDLE_WIDTH 20
#define PONG_PADDLE_HEIGHT 30
#define PONG_BALL_SIZE 10
#define PONG_PLAYER_SPEED 3
#define PONG_AI_SPEED 2
#define PONG_MAX_LOSSES 5
#define PONG_PADDLE_OFFSET 10

void PingPong_Init(void);
void PingPong_Update(InputDirection direction);
void PingPong_Draw(void);
bool PingPong_IsGameOver(void);
void PingPong_Reset(void);

#endif
