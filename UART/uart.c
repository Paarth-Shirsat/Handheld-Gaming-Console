/*
 * uart.c
 *
 *  Created on: Aug 6, 2025
 *      Author: taher
 */

#include "uart.h"

void uart_init(UART_HandleTypeDef *huart) {
    HAL_UART_Init(huart);
}

void uart_sendByte(UART_HandleTypeDef *huart, uint8_t data) {
    HAL_UART_Transmit(huart, &data, 1, HAL_MAX_DELAY);
}

void uart_sendData(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size) {
    HAL_UART_Transmit(huart, data, size, HAL_MAX_DELAY);
}

HAL_StatusTypeDef uart_getByte(UART_HandleTypeDef *huart, uint8_t *data) {
    return HAL_UART_Receive(huart, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef uart_getData(UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size) {
    return HAL_UART_Receive(huart, buffer, size, HAL_MAX_DELAY);
}

