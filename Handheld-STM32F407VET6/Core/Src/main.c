#include "main.h"
#include "pin_defs.h"
#include "input.h"
#include "snake.h"
#include "pingpong.h"
#include "ili9341.h"
#include "fonts.h"
#include "images.h"
#include "mpu6050.h"
#include <stdbool.h>

#define STATE_INTRO 0
#define STATE_MENU 1
#define STATE_GAME_SNAKE 2
#define STATE_GAME_PONG 3

ADC_HandleTypeDef JOYSTICK_ADC;
I2C_HandleTypeDef MPU6050_I2C;
SPI_HandleTypeDef TFT_SPI;
UART_HandleTypeDef DEBUG_UART;

static int currentState = STATE_INTRO;
static int menuSelection = 0;
static uint32_t lastUpdateTime = 0;
static uint32_t lastMenuTime = 0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI2_Init(void);
static void MX_UART4_Init(void);

static void drawIntroScreen(void);
static void drawMenuScreen(void);
static void runSnakeGame(void);
static void runPingPongGame(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_I2C2_Init();
    MX_SPI2_Init();
    MX_UART4_Init();

    ILI9341_Init();
    MPU6050_Init();
    Input_Init();

    drawIntroScreen();

    while (1) {
        Input_Update();

        switch (currentState) {
            case STATE_MENU:
                if (HAL_GetTick() - lastMenuTime > 100) {
                    InputDirection dir = Input_GetDirection();

                    if (dir == INPUT_UP) {
                        menuSelection = 0;
                        drawMenuScreen();
                    } else if (dir == INPUT_DOWN) {
                        menuSelection = 1;
                        drawMenuScreen();
                    }

                    if (Input_IsEnterPressed()) {
                        if (menuSelection == 0) {
                            ILI9341_FillScreen(ILI9341_BLACK);
                            ILI9341_DrawImage(40, 0, 240, 240, snake_intro);
                            HAL_Delay(1000);
                            ILI9341_FillScreen(ILI9341_BLACK);
                            ILI9341_DrawImage(0, 0, 320, 35, bg_final_top);
                            ILI9341_DrawImage(0, 205, 320, 35, bg_final_bottom);
                            ILI9341_DrawImage(0, 35, 47, 170, bg_final_left);
                            ILI9341_DrawImage(274, 35, 46, 170, bg_final_right);
                            currentState = STATE_GAME_SNAKE;
                            SNAKE_Init();
                            lastUpdateTime = HAL_GetTick();
                        } else if (menuSelection == 1) {
                            ILI9341_FillScreen(ILI9341_BLACK);
                            currentState = STATE_GAME_PONG;
                            PingPong_Init();
                            lastUpdateTime = HAL_GetTick();
                        }
                        HAL_Delay(300);
                    }

                    lastMenuTime = HAL_GetTick();
                }
                break;

            case STATE_GAME_SNAKE:
                runSnakeGame();
                break;

            case STATE_GAME_PONG:
                runPingPongGame();
                break;

            default:
                break;
        }
    }
}

static void drawIntroScreen(void) {
    ILI9341_FillScreen(ILI9341_BLACK);
    ILI9341_DrawImage(128, 40, 64, 64, intro);
    char temp_msg[] = "HANDHELD-CONSOLE";
    ILI9341_WriteString(32, 204, temp_msg, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);
    HAL_Delay(2000);
    currentState = STATE_MENU;
    drawMenuScreen();
}

static void drawMenuScreen(void) {
    ILI9341_FillRectangle(30, 40, 260, 200, ILI9341_BLACK);
    char temp_msg[] = "SELECT GAME";
    ILI9341_WriteString(72, 10, temp_msg, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);

    if (menuSelection == 0) {
        ILI9341_DrawImage(80, 56, 160, 40, menu1_2);
        ILI9341_DrawImage(80, 116, 160, 40, menu2_1);
    } else {
        ILI9341_DrawImage(80, 56, 160, 40, menu1_1);
        ILI9341_DrawImage(80, 116, 160, 40, menu2_2);
    }
}

static void runSnakeGame(void) {
    if (Input_IsEscPressed()) {
        ILI9341_FillScreen(ILI9341_BLACK);
        ILI9341_DrawImage(40, 0, 240, 240, snake_outro);
        HAL_Delay(1500);
        currentState = STATE_MENU;
        drawMenuScreen();
        return;
    }

    if (SNAKE_IsGameOver()) {
        ILI9341_FillScreen(ILI9341_BLACK);
        ILI9341_DrawImage(40, 0, 240, 240, snake_outro);
        HAL_Delay(1500);
        currentState = STATE_MENU;
        drawMenuScreen();
        return;
    }

    if (HAL_GetTick() - lastUpdateTime > 100) {
        InputDirection direction = Input_GetDirection();
        SNAKE_Update(direction);
        SNAKE_Draw();
        lastUpdateTime = HAL_GetTick();
    }
}

static void runPingPongGame(void) {
    if (Input_IsEscPressed()) {
        currentState = STATE_MENU;
        drawMenuScreen();
        return;
    }

    if (PingPong_IsGameOver()) {
        HAL_Delay(2000);
        currentState = STATE_MENU;
        drawMenuScreen();
        return;
    }

    if (HAL_GetTick() - lastUpdateTime > 20) {
        InputDirection direction = Input_GetDirection();
        PingPong_Update(direction);
        PingPong_Draw();
        lastUpdateTime = HAL_GetTick();
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    JOYSTICK_ADC.Instance = ADC1;
    JOYSTICK_ADC.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    JOYSTICK_ADC.Init.Resolution = ADC_RESOLUTION_12B;
    JOYSTICK_ADC.Init.ScanConvMode = DISABLE;
    JOYSTICK_ADC.Init.ContinuousConvMode = DISABLE;
    JOYSTICK_ADC.Init.DiscontinuousConvMode = DISABLE;
    JOYSTICK_ADC.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    JOYSTICK_ADC.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    JOYSTICK_ADC.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    JOYSTICK_ADC.Init.NbrOfConversion = 1;
    JOYSTICK_ADC.Init.DMAContinuousRequests = DISABLE;
    JOYSTICK_ADC.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&JOYSTICK_ADC) != HAL_OK) {
        Error_Handler();
    }

    sConfig.Channel = JOYSTICK_X_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&JOYSTICK_ADC, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_I2C2_Init(void) {
    MPU6050_I2C.Instance = I2C2;
    MPU6050_I2C.Init.ClockSpeed = 100000;
    MPU6050_I2C.Init.DutyCycle = I2C_DUTYCYCLE_2;
    MPU6050_I2C.Init.OwnAddress1 = 0;
    MPU6050_I2C.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    MPU6050_I2C.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    MPU6050_I2C.Init.OwnAddress2 = 0;
    MPU6050_I2C.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    MPU6050_I2C.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&MPU6050_I2C) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_SPI2_Init(void) {
    TFT_SPI.Instance = SPI2;
    TFT_SPI.Init.Mode = SPI_MODE_MASTER;
    TFT_SPI.Init.Direction = SPI_DIRECTION_2LINES;
    TFT_SPI.Init.DataSize = SPI_DATASIZE_8BIT;
    TFT_SPI.Init.CLKPolarity = SPI_POLARITY_LOW;
    TFT_SPI.Init.CLKPhase = SPI_PHASE_1EDGE;
    TFT_SPI.Init.NSS = SPI_NSS_SOFT;
    TFT_SPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    TFT_SPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
    TFT_SPI.Init.TIMode = SPI_TIMODE_DISABLE;
    TFT_SPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    TFT_SPI.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&TFT_SPI) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_UART4_Init(void) {
    DEBUG_UART.Instance = UART4;
    DEBUG_UART.Init.BaudRate = 115200;
    DEBUG_UART.Init.WordLength = UART_WORDLENGTH_8B;
    DEBUG_UART.Init.StopBits = UART_STOPBITS_1;
    DEBUG_UART.Init.Parity = UART_PARITY_NONE;
    DEBUG_UART.Init.Mode = UART_MODE_TX_RX;
    DEBUG_UART.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    DEBUG_UART.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&DEBUG_UART) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_RST_PORT, TFT_RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MPU_LED_PORT, MPU_LED_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = TFT_CS_PIN | TFT_RST_PIN | MPU_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = TFT_DC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TFT_DC_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = JOYSTICK_BTN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(JOYSTICK_BTN_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BTN_ENTER_PIN | BTN_ESC_PIN | BTN_RIGHT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BTN_UP_PIN | BTN_DOWN_PIN | BTN_LEFT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
}
#endif
