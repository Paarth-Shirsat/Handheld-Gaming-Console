/*
 * uart.h
 *
 *  Created on: Aug 6, 2025
 *      Author: taher
 */
#ifndef UART_UART_H_
#define UART_UART_H_

#include "stm32f4xx_hal.h"

void uart_init(UART_HandleTypeDef *huart);
void uart_sendByte(UART_HandleTypeDef *huart, uint8_t data);
void uart_sendData(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size);

HAL_StatusTypeDef uart_getByte(UART_HandleTypeDef *huart, uint8_t *data);
HAL_StatusTypeDef uart_getData(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size);

#endif /* UART_UART_H_ */
