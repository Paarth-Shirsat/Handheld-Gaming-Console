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
#include "ili9341.h"
#include "fonts.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
	const char *name;
	int id , x, y , height , width , textx , texty;
} items;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STATE_MENU 1
#define STATE_GAME1 2
#define STATE_GAME2 3
#define STATE_OVER 4
#define GPIO_UP_PO GPIOC
#define GPIO_UP_PIN GPIO_PIN_10
#define GPIO_DOWN_PO GPIOC
#define GPIO_DOWN_PIN GPIO_PIN_11
#define GPIO_LEFT_PO GPIOC
#define GPIO_LEFT_PIN GPIO_PIN_6
#define GPIO_RIGHT_PO GPIOC
#define GPIO_RIGHT_PIN GPIO_PIN_7
#define GPIO_BUTTON1_PO GPIOC
#define GPIO_BUTTON2_PO GPIOC
#define GPIO_BUTTON1_PIN GPIO_PIN_13
#define GPIO_BUTTON2_PIN GPIO_PIN_8
#define CHANNEL_X ADC_CHANNEL_8
#define CHANNEL_Y ADC_CHANNEL_9
#define JOYSTICK_BUTTON_PO GPIOB
#define JOYSTICK_BUTTON_PIN GPIO_PIN_2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int current_state = STATE_MENU;
int current_menu_index = 0;
items item[] = {{"Snake Game" , 1 , 50 , 50 , 220 , 50 ,105 ,66} , {"Game2 (in progress)" , 2 , 50 , 120 , 220 , 50 ,50 ,136}} ;
int menu_length = 2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
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
bool getButton(GPIO_TypeDef *GPIOP , uint16_t GPIOPIN){
	if (HAL_GPIO_ReadPin(GPIOP,GPIOPIN)==GPIO_PIN_SET){
		return true;
	}
	return false;
}

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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  ILI9341_Init();
  ILI9341_FillScreen(ILI9341_BLACK);
  current_menu_index = 0;
  bool firstdraw = true;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if(current_state == STATE_MENU){
		  if(getButton(GPIO_UP_PO , GPIO_UP_PIN)){
			  current_menu_index -= 1;
			  if(current_menu_index < 0){
				  current_menu_index=menu_length-1;
			  }
			  for(int i = 0 ; i < menu_length ; i++){
				  ILI9341_FillRectangle(item[i].x , item[i].y,item[i].width,item[i].height,ILI9341_BLACK);
			  }
			  for(int i = 0 ; i < menu_length ; i++){
				  uint16_t color = ILI9341_BLUE;
				  uint16_t color_text = ILI9341_WHITE;
				  if(current_menu_index == i){
					  color = ILI9341_CYAN;
					  color_text = ILI9341_BLACK;
				  }
				  ILI9341_FillRectangle(item[i].x , item[i].y,item[i].width,item[i].height,color);
				  ILI9341_WriteString(item[i].textx,item[i].texty,item[i].name,Font_11x18,color_text,color);

			  }
			  char title[]="SELECT A GAME";
			  ILI9341_WriteString(56,10,title,Font_16x26,ILI9341_WHITE,ILI9341_BLACK);
		  }
		  if(getButton(GPIO_DOWN_PO , GPIO_DOWN_PIN)){
		  			  current_menu_index += 1;
		  			  if(current_menu_index > menu_length-1){
		  				  current_menu_index= 0;
		  			  }
		  			  for(int i = 0 ; i < menu_length ; i++){
		  				  ILI9341_FillRectangle(item[i].x , item[i].y,item[i].width,item[i].height,ILI9341_BLACK);
		  			  }
		  			  for(int i = 0 ; i < menu_length ; i++){
		  				  uint16_t color = ILI9341_BLUE;
		  				  uint16_t color_text = ILI9341_WHITE;
		  				  if(current_menu_index == i){
		  					  color = ILI9341_CYAN;
		  					  color_text = ILI9341_BLACK;
		  				  }
		  				  ILI9341_FillRectangle(item[i].x , item[i].y,item[i].width,item[i].height,color);
		  				  ILI9341_WriteString(item[i].textx,item[i].texty,item[i].name,Font_11x18,color_text,color);

		  			  }
		  			  char title[]="SELECT A GAME";
		  			  ILI9341_WriteString(56,10,title,Font_16x26,ILI9341_WHITE,ILI9341_BLACK);
		  		  }
		  if(getButton(GPIO_BUTTON1_PO , GPIO_BUTTON1_PIN)){
		  			  if(item[current_menu_index].id == 1){
		  				  ILI9341_FillScreen(ILI9341_BLACK);
		  				  SNAKE_Init();
		  				  current_state = STATE_GAME1;
		  			  }

	  }
		  if(firstdraw){
			  for(int i = 0 ; i < menu_length ; i++){
			  				  ILI9341_FillRectangle(item[i].x , item[i].y,item[i].width,item[i].height,ILI9341_BLACK);
			  			  }
			  			  for(int i = 0 ; i < menu_length ; i++){
			  				  uint16_t color = ILI9341_BLUE;
			  				  uint16_t color_text = ILI9341_WHITE;
			  				  if(current_menu_index == i){
			  					  color = ILI9341_CYAN;
			  					  color_text = ILI9341_BLACK;
			  				  }
			  				  ILI9341_FillRectangle(item[i].x , item[i].y,item[i].width,item[i].height,color);
			  				  ILI9341_WriteString(item[i].textx,item[i].texty,item[i].name,Font_11x18,color_text,color);

			  			  }
			  			  char title[]="SELECT A GAME";
			  			  ILI9341_WriteString(56,10,title,Font_16x26,ILI9341_WHITE,ILI9341_BLACK);

			  			  firstdraw = false;

		  }

    /* USER CODE BEGIN 3 */
  }

	  else if (current_state == STATE_OVER){
		  ILI9341_FillScreen(ILI9341_RED);
		  if(getButton(GPIO_BUTTON1_PO , GPIO_BUTTON1_PIN)){
		  		  			  firstdraw = true;
		  		  			  current_state = 	STATE_MENU;
		  		  			  }

	  }
	  else if(current_state == STATE_GAME1){  //snake game part
		  if(getButton(GPIO_UP_PO , GPIO_UP_PIN)){
			  SNAKE_ChangeDirection(4);
		  }
		  if(getButton(GPIO_DOWN_PO,GPIO_UP_PIN)){
			  SNAKE_ChangeDirection(2);
		  }
		  if(getButton(GPIO_LEFT_PO,GPIO_LEFT_PIN)){
		  	  SNAKE_ChangeDirection(3);
		  }
		  if(getButton(GPIO_RIGHT_PO,GPIO_RIGHT_PIN)){
		  	  SNAKE_ChangeDirection(1);
		  }
		  bool gameover = SNAKE_getOver();
		  if(!gameover){
			  SNAKE_updateGame();
			  SNAKE_drawGame();
			  HAL_Delay(100);
		  }
		  else{
			  current_state = STATE_OVER;
		  }
	  }

  /* USER CODE END 3 */
}
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
  sConfig.Channel = ADC_CHANNEL_9;
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC6 PC7 PC8
                           PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
