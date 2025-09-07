/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>
#include <string.h>
#include "snake.h"
#include "pingpong.h"
#include "ili9341.h"
#include "fonts.h"
#include "testimg.h"
#include "mpu6050.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STATE_INTRO 0
#define STATE_MENU 1
#define STATE_GAME_SNAKE 2
#define STATE_GAME_PONG 3
#define STATE_GAME_OVER 4
#define MPU_ACCEL_X_UP_THRESHOLD 2500
#define MPU_ACCEL_X_DOWN_THRESHOLD 1500
#define MPU_ACCEL_Y_LEFT_THRESHOLD 1500
#define MPU_ACCEL_Y_RIGHT_THRESHOLD 2500
#define MPU_LED_PORT GPIOE
#define MPU_LED_PIN GPIO_PIN_2
#define JOYSTICK_X_ADC_CHANNEL ADC_CHANNEL_4
#define JOYSTICK_Y_ADC_CHANNEL ADC_CHANNEL_6
#define JOYSTICK_BUTTON_PORT GPIOC
#define JOYSTICK_BUTTON_PIN GPIO_PIN_4
#define RESET_BUTTON_PORT GPIOB
#define RESET_BUTTON_PIN GPIO_PIN_0
#define JOYSTICK_THRESHOLD 3500


/* USER CODE END PD */
volatile int currentState = STATE_INTRO;
volatile int menuSelection = 0; // 0 for Snake, 1 for Ping Pong
volatile bool isMpuControl = false;
uint32_t lastButtonPressTime = 0;
const uint32_t buttonDebounceDelay = 200;
volatile int mpu_x, mpu_y, mpu_z;
volatile bool first_snake_dir = true;

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI2_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */
uint16_t getADC(uint32_t channel);
bool isButtonHeld(GPIO_TypeDef *port, uint16_t pin);
void handleInput(void);
void drawIntroScreen(void);
void drawMenuScreen(void);
void drawGameOverScreen(void);
void runSnakeGame(void);
void runPingPongGame(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
  ILI9341_Init();
  MPU6050_Init();
  drawIntroScreen();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  handleInput();
    /* USER CODE END WHILE */
	  switch (currentState) {
	              case STATE_MENU:
	                  drawMenuScreen();
	                  HAL_Delay(500);
	                  break;
	              case STATE_GAME_SNAKE:
	                  runSnakeGame();
	                  break;
	              case STATE_GAME_PONG:
	                  runPingPongGame();
	                  break;
	              case STATE_GAME_OVER:
	                  drawGameOverScreen();
	                  break;
	              default:
	                  break;
	          }

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
void drawIntroScreen(void) {
    ILI9341_FillScreen(ILI9341_BLACK);
    ILI9341_DrawImage(128, 40, 64, 64, intro);
    char temp_msg[] = "HANDHELD-CONSOLE";
    ILI9341_WriteString(32, 204,temp_msg, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);
    HAL_Delay(2000);
    currentState = STATE_MENU;
}
void drawMenuScreen(void) {
    ILI9341_FillRectangle(30,40,260,200,ILI9341_BLACK);
    char temp_msg[] = "SELECT GAME";
    ILI9341_WriteString(72, 10, temp_msg, Font_16x26, ILI9341_WHITE, ILI9341_BLACK);

    if (menuSelection == 0) {
        ILI9341_DrawImage(80, 56, 160, 40, menu1_2); // Highlighted Snake
        ILI9341_DrawImage(80, 116, 160, 40, menu2_1); // Unhighlighted Pong
    } else {
        ILI9341_DrawImage(80, 56, 160, 40, menu1_1); // Unhighlighted Snake
        ILI9341_DrawImage(80, 116, 160, 40, menu2_2); // Highlighted Pong
    }
}
void drawGameOverScreen(void) {
    ILI9341_FillScreen(ILI9341_BLACK);
    char temp_msg1[] = "Game Over";
    char temp_msg2[] = "Press Retry";
    ILI9341_WriteString(ILI9341_WIDTH / 2 - 50, ILI9341_HEIGHT / 2, temp_msg1, Font_16x26, ILI9341_RED, ILI9341_BLACK);
    ILI9341_WriteString(ILI9341_WIDTH / 2 - 80, ILI9341_HEIGHT / 2 + 30, temp_msg2, Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
    if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET){
    	currentState = STATE_MENU;
    }
}
void handleInput(void) {
    //bool selectButtonPressed = (HAL_GPIO_ReadPin(JOYSTICK_BUTTON_PORT, JOYSTICK_BUTTON_PIN) == GPIO_PIN_SET);

    if ((HAL_GPIO_ReadPin(GPIOC , GPIO_PIN_4)==GPIO_PIN_RESET) ) {
        if (currentState == STATE_MENU) {
            if (menuSelection == 0) {
            	 ILI9341_FillScreen(ILI9341_BLACK);
                 ILI9341_DrawImage(40,0,240,240,snake_intro);
                 ILI9341_FillScreen(ILI9341_BLACK);
                 ILI9341_DrawImage(0,0,320,35,bg_final_top);
                 ILI9341_DrawImage(0,205,320,35,bg_final_bottom);
                 ILI9341_DrawImage(0,35,47,170,bg_final_left);
                 ILI9341_DrawImage(274,35,46,170,bg_final_right);
                currentState = STATE_GAME_SNAKE;
                SNAKE_Init();
            } else if (menuSelection == 1) {
                currentState = STATE_GAME_PONG;
                ILI9341_FillScreen(ILI9341_BLACK);
                PingPong_Init();
            }
        } else if (currentState == STATE_GAME_OVER) {
            currentState = STATE_MENU;
            drawMenuScreen();
        } else {
            isMpuControl = !isMpuControl;
            if (isMpuControl) {
                HAL_GPIO_WritePin(MPU_LED_PORT, MPU_LED_PIN, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(MPU_LED_PORT, MPU_LED_PIN, GPIO_PIN_RESET);
            }
        }
    }

    if (currentState == STATE_MENU) {
        uint16_t xVal = getADC(JOYSTICK_X_ADC_CHANNEL);
        if (xVal < 4095 - JOYSTICK_THRESHOLD) { // Joystick UP
            menuSelection = 0;
        } else if (xVal > JOYSTICK_THRESHOLD) { // Joystick DOWN
            menuSelection = 1;
        }
    }
}
bool isButtonHeld(GPIO_TypeDef *port, uint16_t pin) {
    return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET);
}
void runSnakeGame(void) {
    if (SNAKE_getOver()) {
    	ILI9341_FillScreen(ILI9341_BLACK);
    	ILI9341_DrawImage(40, 0, 240, 240, snake_outro);
    	HAL_Delay(500);
        if(isButtonHeld(JOYSTICK_BUTTON_PORT,JOYSTICK_BUTTON_PIN)) currentState = STATE_MENU;
        return;
    }

    int direction ;
    if (first_snake_dir){
    	direction = 1;
    	first_snake_dir = false;
    }
    if (isMpuControl) {
        MPU6050_Read_Accel(&mpu_x, &mpu_y, &mpu_z);
        if (mpu_x > MPU_ACCEL_X_UP_THRESHOLD) direction = 2; // down
        if (mpu_x < MPU_ACCEL_X_DOWN_THRESHOLD) direction = 4; // up
        if (mpu_y > MPU_ACCEL_Y_RIGHT_THRESHOLD) direction = 3; // left
        if (mpu_y < MPU_ACCEL_Y_LEFT_THRESHOLD) direction = 1; // right
    } else {
        uint16_t xVal = getADC(JOYSTICK_X_ADC_CHANNEL);
        uint16_t yVal = getADC(JOYSTICK_Y_ADC_CHANNEL);

        if (xVal > JOYSTICK_THRESHOLD) direction = 2;
        if (xVal < 4095 - JOYSTICK_THRESHOLD) direction = 4;
        if (yVal > JOYSTICK_THRESHOLD) direction = 3;
        if (yVal < 4095 - JOYSTICK_THRESHOLD) direction = 1;
    }

    if (direction != 0) {
        SNAKE_ChangeDirection(direction);
    }

    SNAKE_updateGame();
    SNAKE_drawGame();
    HAL_Delay(100);
}
void runPingPongGame(void) {
    if (PingPong_IsGameOver()) {
        currentState = STATE_GAME_OVER;
        return;
    }

    bool upPressed = false;
    bool downPressed = false;

    if (isMpuControl) {
        MPU6050_Read_Accel(&mpu_x, &mpu_y, &mpu_z);
        if (mpu_x > MPU_ACCEL_X_UP_THRESHOLD) downPressed = true;
        if (mpu_x < MPU_ACCEL_X_DOWN_THRESHOLD) upPressed = true;
    } else {
        uint16_t xVal = getADC(JOYSTICK_X_ADC_CHANNEL);
        if (xVal > JOYSTICK_THRESHOLD) downPressed = true;
        if (xVal < 4095 - JOYSTICK_THRESHOLD) upPressed = true;
    }

    PingPong_Update(upPressed, downPressed);
    PingPong_Draw();
    HAL_Delay(20);
}

uint16_t getADC(uint32_t channel){
    ADC_ChannelConfTypeDef adc_confg = {0};
    adc_confg.Channel = channel;
    adc_confg.Rank = 1;
    adc_confg.SamplingTime = ADC_SAMPLETIME_56CYCLES;
    HAL_ADC_ConfigChannel(&hadc1,&adc_confg);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1 , HAL_MAX_DELAY);
    uint16_t val = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return val;
}




/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE4 PE6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE8 PE10 PE12 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
