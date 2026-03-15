#include "pingpong.h"
#include "ili9341.h"
#include "images.h"
#include "fonts.h"
#include "stm32f4xx_hal.h"

static int16_t playerPaddleY;
static int16_t aiPaddleY;
static int16_t ballX;
static int16_t ballY;
static int16_t prevPlayerPaddleY;
static int16_t prevAiPaddleY;
static int16_t prevBallX;
static int16_t prevBallY;
static int8_t ballDX;
static int8_t ballDY;
static uint32_t playerLosses;
static bool gameOver;

static void resetBall(void) {
    ballX = ILI9341_WIDTH / 2 - PONG_BALL_SIZE / 2;
    ballY = ILI9341_HEIGHT / 2 - PONG_BALL_SIZE / 2;
    ballDX = (HAL_GetTick() % 2 == 0) ? 2 : -2;
    ballDY = (HAL_GetTick() % 2 == 0) ? 2 : -2;
}

void PingPong_Init(void) {
    playerPaddleY = ILI9341_HEIGHT / 2 - PONG_PADDLE_HEIGHT / 2;
    aiPaddleY = ILI9341_HEIGHT / 2 - PONG_PADDLE_HEIGHT / 2;
    playerLosses = 0;
    gameOver = false;
    resetBall();
    prevPlayerPaddleY = playerPaddleY;
    prevAiPaddleY = aiPaddleY;
    prevBallX = ballX;
    prevBallY = ballY;
}

void PingPong_Reset(void) {
    PingPong_Init();
}

void PingPong_Update(InputDirection direction) {
    if (gameOver) {
        return;
    }

    prevPlayerPaddleY = playerPaddleY;
    prevAiPaddleY = aiPaddleY;
    prevBallX = ballX;
    prevBallY = ballY;

    if (direction == INPUT_UP && playerPaddleY > 0) {
        playerPaddleY -= PONG_PLAYER_SPEED;
    }
    if (direction == INPUT_DOWN && playerPaddleY < ILI9341_HEIGHT - PONG_PADDLE_HEIGHT) {
        playerPaddleY += PONG_PLAYER_SPEED;
    }

    if (ballY > aiPaddleY + PONG_PADDLE_HEIGHT / 2) {
        aiPaddleY += PONG_AI_SPEED;
    } else if (ballY < aiPaddleY + PONG_PADDLE_HEIGHT / 2) {
        aiPaddleY -= PONG_AI_SPEED;
    }

    if (aiPaddleY < 0) aiPaddleY = 0;
    if (aiPaddleY > ILI9341_HEIGHT - PONG_PADDLE_HEIGHT) {
        aiPaddleY = ILI9341_HEIGHT - PONG_PADDLE_HEIGHT;
    }

    ballX += ballDX;
    ballY += ballDY;

    if (ballY <= 0 || ballY >= ILI9341_HEIGHT - PONG_BALL_SIZE) {
        ballDY = -ballDY;
    }

    if (ballX <= PONG_PADDLE_OFFSET + PONG_PADDLE_WIDTH &&
        ballX >= PONG_PADDLE_OFFSET &&
        ballY + PONG_BALL_SIZE >= playerPaddleY &&
        ballY <= playerPaddleY + PONG_PADDLE_HEIGHT) {
        ballDX = -ballDX;
    }

    if (ballX + PONG_BALL_SIZE >= ILI9341_WIDTH - PONG_PADDLE_OFFSET - PONG_PADDLE_WIDTH &&
        ballX + PONG_BALL_SIZE <= ILI9341_WIDTH - PONG_PADDLE_OFFSET &&
        ballY + PONG_BALL_SIZE >= aiPaddleY &&
        ballY <= aiPaddleY + PONG_PADDLE_HEIGHT) {
        ballDX = -ballDX;
    }

    if (ballX < 0) {
        playerLosses++;
        if (playerLosses >= PONG_MAX_LOSSES) {
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
        ILI9341_WriteString(ILI9341_WIDTH / 2 - 50, ILI9341_HEIGHT / 2, temp_msg,
                           Font_16x26, ILI9341_RED, ILI9341_BLACK);
    } else {
        ILI9341_FillRectangle(PONG_PADDLE_OFFSET, prevPlayerPaddleY,
                             PONG_PADDLE_WIDTH, PONG_PADDLE_HEIGHT, ILI9341_BLACK);
        ILI9341_FillRectangle(ILI9341_WIDTH - PONG_PADDLE_OFFSET - PONG_PADDLE_WIDTH,
                             prevAiPaddleY, PONG_PADDLE_WIDTH, PONG_PADDLE_HEIGHT, ILI9341_BLACK);
        ILI9341_FillRectangle(prevBallX, prevBallY, PONG_BALL_SIZE, PONG_BALL_SIZE, ILI9341_BLACK);

        ILI9341_DrawImage(PONG_PADDLE_OFFSET, playerPaddleY,
                         PONG_PADDLE_WIDTH, PONG_PADDLE_HEIGHT, paddle);
        ILI9341_DrawImage(ILI9341_WIDTH - PONG_PADDLE_OFFSET - PONG_PADDLE_WIDTH,
                         aiPaddleY, PONG_PADDLE_WIDTH, PONG_PADDLE_HEIGHT, paddle);
        ILI9341_DrawImage(ballX, ballY, PONG_BALL_SIZE, PONG_BALL_SIZE, ball);
    }
}

bool PingPong_IsGameOver(void) {
    return gameOver;
}
