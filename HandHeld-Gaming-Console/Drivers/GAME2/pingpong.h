/*
 * pingpong.h
 *
 *  Created on: Sep 6, 2025
 *      Author: taher
 */

#ifndef GAME2_PINGPONG_H_
#define GAME2_PINGPONG_H_

#include <stdbool.h>

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 30
#define BALL_SIZE 10

void PingPong_Init(void);
void PingPong_Update(bool up_button, bool down_button);
void PingPong_Draw(void);
bool PingPong_IsGameOver(void);



#endif /* GAME2_PINGPONG_H_ */
