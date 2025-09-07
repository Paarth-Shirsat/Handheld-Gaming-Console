/*
 * pingpong.c
 *
 *  Created on: Sep 6, 2025
 *      Author: taher
 */
#include "pingpong.h"
#include "ili9341.h"
#include "testimg.h"
#include "fonts.h"
#include "stm32f4xx_hal.h"

static int16_t playerPaddleY, aiPaddleY, ballX, ballY;
static int16_t prevPlayerPaddleY, prevAiPaddleY, prevBallX, prevBallY;
static int8_t ballDX, ballDY;
static uint32_t playerLosses;
static bool gameOver = false;

static void resetBall() {
    ballX = ILI9341_WIDTH / 2 - BALL_SIZE / 2;
    ballY = ILI9341_HEIGHT / 2 - BALL_SIZE / 2;
    ballDX = (HAL_GetTick() % 2 == 0) ? 2 : -2;
    ballDY = (HAL_GetTick() % 2 == 0) ? 2 : -2;
}

void PingPong_Init(void) {
    playerPaddleY = ILI9341_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    aiPaddleY = ILI9341_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    playerLosses = 0;
    gameOver = false;
    resetBall();

    prevPlayerPaddleY = playerPaddleY;
    prevAiPaddleY = aiPaddleY;
    prevBallX = ballX;
    prevBallY = ballY;
}

void PingPong_Update(bool up_button, bool down_button) {
    if (gameOver) {
        return;
    }

    prevPlayerPaddleY = playerPaddleY;
    prevAiPaddleY = aiPaddleY;
    prevBallX = ballX;
    prevBallY = ballY;

    int16_t playerSpeed = 3;
    if (up_button && playerPaddleY > 0) {
        playerPaddleY -= playerSpeed;
    }
    if (down_button && playerPaddleY < ILI9341_HEIGHT - PADDLE_HEIGHT) {
        playerPaddleY += playerSpeed;
    }

    int16_t aiSpeed = 2;
    if (ballY > aiPaddleY + PADDLE_HEIGHT / 2) {
        aiPaddleY += aiSpeed;
    } else if (ballY < aiPaddleY + PADDLE_HEIGHT / 2) {
        aiPaddleY -= aiSpeed;
    }

    if (aiPaddleY < 0) aiPaddleY = 0;
    if (aiPaddleY > ILI9341_HEIGHT - PADDLE_HEIGHT) aiPaddleY = ILI9341_HEIGHT - PADDLE_HEIGHT;

    ballX += ballDX;
    ballY += ballDY;

    if (ballY <= 0 || ballY >= ILI9341_HEIGHT - BALL_SIZE) {
        ballDY = -ballDY;
    }

    if (ballX <= 10 + PADDLE_WIDTH && ballX >= 10 &&
        ballY + BALL_SIZE >= playerPaddleY && ballY <= playerPaddleY + PADDLE_HEIGHT) {
        ballDX = -ballDX;
    }

    if (ballX + BALL_SIZE >= ILI9341_WIDTH - 15 && ballX + BALL_SIZE <= ILI9341_WIDTH - 15 + PADDLE_WIDTH &&
        ballY + BALL_SIZE >= aiPaddleY && ballY <= aiPaddleY + PADDLE_HEIGHT) {
        ballDX = -ballDX;
    }

    if (ballX < 0) {
        playerLosses++;
        if (playerLosses >= 5) {
            gameOver = true;
        }
        resetBall();
    } else if (ballX > ILI9341_WIDTH) {
        resetBall();
    }
}

void PingPong_Draw(void) {
    if (gameOver) {
        ILI9341_FillScreen(ILI9341_BLACK);
        char temp_msg[] = "Game Over";
        ILI9341_WriteString(ILI9341_WIDTH / 2 - 50, ILI9341_HEIGHT / 2, temp_msg, Font_16x26, ILI9341_RED, ILI9341_BLACK);
    } else {
        ILI9341_FillRectangle(10, prevPlayerPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, ILI9341_BLACK);
        ILI9341_FillRectangle(ILI9341_WIDTH - 15, prevAiPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, ILI9341_BLACK);
        ILI9341_FillRectangle(prevBallX, prevBallY, BALL_SIZE, BALL_SIZE, ILI9341_BLACK);

        ILI9341_DrawImage(10, playerPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, paddle);
        ILI9341_DrawImage(ILI9341_WIDTH - 10, aiPaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, paddle);
        ILI9341_DrawImage(ballX, ballY, BALL_SIZE, BALL_SIZE, ball);
    }
}

bool PingPong_IsGameOver(void) {
    return gameOver;
}



