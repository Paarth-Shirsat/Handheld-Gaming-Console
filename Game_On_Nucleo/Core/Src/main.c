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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "mpu6050.h"
#include "ili9341.h"
#include "fonts.h"
#include "stm32f4xx_hal.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define CELL_SIZE       16
#define SCREEN_W        ILI9341_WIDTH   // 240
#define SCREEN_H        ILI9341_HEIGHT  // 320
#define COLS            (SCREEN_W / CELL_SIZE)   // 15
#define ROWS            (SCREEN_H / CELL_SIZE)   // 20

#define MOVE_INTERVAL_MS    180

#define ADC_THRESHOLD_LOW   1000
#define ADC_THRESHOLD_HIGH  3500

#define MAX_SNAKE (COLS * ROWS)
#define BG_COLOR    ILI9341_BLACK
#define SNAKE_COLOR ILI9341_GREEN
#define FRUIT_COLOR ILI9341_RED

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

//Struct for coords
typedef struct {
    uint8_t x;
    uint8_t y;
}Point;

static Point snake[MAX_SNAKE];
static int snake_len = 0;

static Point fruit;

typedef enum {DIR_UP=0, DIR_DOWN, DIR_LEFT, DIR_RIGHT}Dir;
static Dir current_dir = DIR_RIGHT;
static Dir desired_dir = DIR_RIGHT;

static uint32_t last_move_tick = 0;
static uint8_t game_over = 0;
static uint8_t button_pressed_flag = 0;
static int score = 0;

//Joystick ADC readings
static uint32_t joyX = 0;
static uint32_t joyY = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

static void ADC_ChannelConfig(uint32_t channel);
static uint32_t ADC_ReadChannel(uint32_t channel);
static void draw_cell_circle(uint8_t col, uint8_t row, uint16_t color);
static void erase_cell(uint8_t col, uint8_t row);
static void draw_fruit(void);
static void place_random_fruit(void);
static int point_on_snake(uint8_t x, uint8_t y);
static void init_game(void);
static void move_snake_step(void);
static void show_game_over(void);
static void restart_game_when_button(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint32_t last_time_btn6 = 0;
uint32_t last_time_btn7 = 0;
uint32_t last_time_btn8 = 0;
uint32_t last_time_btn9 = 0;

//Buttons on Interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    uint32_t now = HAL_GetTick();

    switch(GPIO_Pin){
        case GPIO_PIN_6:
            if((now - last_time_btn6) > 150){
                if(current_dir != DIR_DOWN) desired_dir = DIR_UP;
                last_time_btn6 = now;
                button_pressed_flag = 1;
            }
            break;

        case GPIO_PIN_7:
            if((now - last_time_btn7) > 150){
                if(current_dir != DIR_UP) desired_dir = DIR_DOWN;
                last_time_btn7 = now;
                button_pressed_flag = 1;
            }
            break;

        case GPIO_PIN_8:
            if((now - last_time_btn8) > 150){
                if(current_dir != DIR_RIGHT) desired_dir = DIR_LEFT;
                last_time_btn8 = now;
                button_pressed_flag = 1;
            }
            break;

        case GPIO_PIN_9:
            if((now - last_time_btn9) > 150){
                if(current_dir != DIR_LEFT) desired_dir = DIR_RIGHT;
                last_time_btn9 = now;
                button_pressed_flag = 1;
            }
            break;
    }
}

static void ADC_ChannelConfig(uint32_t channel){
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

static uint32_t ADC_ReadChannel(uint32_t channel){
    uint32_t val = 0;
    ADC_ChannelConfig(channel);
    HAL_ADC_Start(&hadc1);
    if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK){
        val = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    return val;
}

//For snake body
static void draw_cell_circle(uint8_t col, uint8_t row, uint16_t color){
    uint16_t cx = col * CELL_SIZE + (CELL_SIZE/2);
    uint16_t cy = row * CELL_SIZE + (CELL_SIZE/2);
    uint16_t r  = (CELL_SIZE/2) - 2; // radius slightly smaller than half cell
    ILI9341_FillCircle(cx, cy, r, color);
}

//For snake movement and game over
static void erase_cell(uint8_t col, uint8_t row){
    uint16_t x = col * CELL_SIZE;
    uint16_t y = row * CELL_SIZE;
    ILI9341_FillRectangle(x, y, CELL_SIZE, CELL_SIZE, BG_COLOR);
}

static void draw_fruit(void){
    uint16_t x = fruit.x * CELL_SIZE;
    uint16_t y = fruit.y * CELL_SIZE;
    ILI9341_FillRectangle(x, y, CELL_SIZE, CELL_SIZE, FRUIT_COLOR);
}

/* Return 1 if (x,y) is on current snake */
static int point_on_snake(uint8_t x, uint8_t y){
    for(int i=0;i<snake_len;i++){
        if(snake[i].x == x && snake[i].y == y) return 1;
    }
    return 0;
}

//Random fruit location
static void place_random_fruit(void){
    if(snake_len >= MAX_SNAKE){
        //grid full
        return;
    }

    uint32_t tries = 0;
    while(1){
        uint8_t rx = rand() % COLS;
        uint8_t ry = rand() % ROWS;
        if(!point_on_snake(rx, ry)){
            fruit.x = rx;
            fruit.y = ry;
            draw_fruit();
            return;
        }
        tries++;
        if(tries > 10000) break;
    }
}

//Initialise Game state and draw background
static void init_game(void){
    ILI9341_FillScreen(BG_COLOR);

    // seed PRNG (use HAL_GetTick, not time())
    srand((unsigned int)HAL_GetTick());

    // start with small snake in center
    snake_len = 3;
    int center_col = COLS/2;
    int center_row = ROWS/2;
    for(int i=0;i<snake_len;i++){
        snake[i].x = center_col - (snake_len-1) + i;
        snake[i].y = center_row;
    }
    current_dir = DIR_RIGHT;
    desired_dir = DIR_RIGHT;
    game_over = 0;
    score = 0;

    for(int i=0;i<snake_len;i++){
        draw_cell_circle(snake[i].x, snake[i].y, SNAKE_COLOR);
    }

    //First fruit
    place_random_fruit();

    last_move_tick = HAL_GetTick();
}

//Snake movement (direction based)
static void move_snake_step(void){
    if(game_over) return;

    // update direction if not opposite
    if(!((current_dir == DIR_LEFT && desired_dir == DIR_RIGHT) ||
         (current_dir == DIR_RIGHT && desired_dir == DIR_LEFT) ||
         (current_dir == DIR_UP && desired_dir == DIR_DOWN) ||
         (current_dir == DIR_DOWN && desired_dir == DIR_UP))){
        current_dir = desired_dir;
    }

    // compute new head
    Point new_head = snake[snake_len - 1]; // current head
    switch(current_dir){
        case DIR_UP:
            if(new_head.y == 0) { game_over = 1; return; }
            new_head.y -= 1;
            break;
        case DIR_DOWN:
            if(new_head.y >= ROWS-1) { game_over = 1; return; }
            new_head.y += 1;
            break;
        case DIR_LEFT:
            if(new_head.x == 0) { game_over = 1; return; }
            new_head.x -= 1;
            break;
        case DIR_RIGHT:
            if(new_head.x >= COLS-1) { game_over = 1; return; }
            new_head.x += 1;
            break;
    }

    //Self-collision
    if(point_on_snake(new_head.x, new_head.y)){
        game_over = 1;
        return;
    }

    int ate = (new_head.x == fruit.x && new_head.y == fruit.y);

    if(ate){

    	score++;

        if(snake_len < MAX_SNAKE){
            snake[snake_len++] = new_head;
            draw_cell_circle(new_head.x, new_head.y, SNAKE_COLOR);
        }
        place_random_fruit();
    }
    else{
        erase_cell(snake[0].x, snake[0].y);
        for(int i=0;i<snake_len-1;i++){
            snake[i] = snake[i+1];
        }
        snake[snake_len-1] = new_head;
        draw_cell_circle(new_head.x, new_head.y, SNAKE_COLOR);
    }
}

//Game-Over
static void show_game_over(void){
    ILI9341_FillRectangle(10, (SCREEN_H/2)-28, SCREEN_W-20, 72, ILI9341_BLACK);
    ILI9341_WriteString(20, (SCREEN_H/2)-18, "GAME OVER", Font_11x18, ILI9341_WHITE, ILI9341_BLACK);

    char buf[32];
    sprintf(buf, "Score: %d", score);
    ILI9341_WriteString(20, (SCREEN_H/2)+6, buf, Font_11x18, ILI9341_WHITE, ILI9341_BLACK);

    ILI9341_WriteString(20, (SCREEN_H/2)+30, "Press any button", Font_7x10, ILI9341_WHITE, ILI9341_BLACK);
}

//Restart when any button is pressed
static void restart_game_when_button(void){
    button_pressed_flag = 0;
    while(!button_pressed_flag){
        HAL_Delay(10);
    }
    // small debounce
    HAL_Delay(100);
    init_game();
}

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
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  ILI9341_Init();
  MPU6050_Init();
  init_game();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	    uint32_t now = HAL_GetTick();

	    //Periodic Movement
	    if((now - last_move_tick) >= MOVE_INTERVAL_MS){
	        last_move_tick = now;
	        move_snake_step();
	        if(game_over){
	            show_game_over();
	            restart_game_when_button();
	        }
	    }

	    joyX = ADC_ReadChannel(ADC_CHANNEL_3);
	    joyY = ADC_ReadChannel(ADC_CHANNEL_4);

	    if(joyX < ADC_THRESHOLD_LOW){
	        if(current_dir != DIR_RIGHT) desired_dir = DIR_LEFT;
	    }
	    else if(joyX > ADC_THRESHOLD_HIGH){
	        if(current_dir != DIR_LEFT) desired_dir = DIR_RIGHT;
	    }
	    if(joyY < ADC_THRESHOLD_LOW){
	        if(current_dir != DIR_DOWN) desired_dir = DIR_UP;
	    }
	    else if(joyY > ADC_THRESHOLD_HIGH){
	        if(current_dir != DIR_UP) desired_dir = DIR_DOWN;
	    }

	    HAL_Delay(5);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PC6 PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
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
