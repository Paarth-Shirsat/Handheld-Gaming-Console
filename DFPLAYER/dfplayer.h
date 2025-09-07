/*
 * dfplayer.h
 *
 *  Created on: Aug 6, 2025
 *      Author: mustafa
 */
#include "stm32f4xx_hal.h"
#include "uart.h"

#ifndef DFPLAYER_DFPLAYER_H_
#define DFPLAYER_DFPLAYER_H_

void df_play(UART_HandleTypeDef *huart , uint16_t track);
void df_playFolder(UART_HandleTypeDef *huart , uint16_t folder , uint16_t track);
void df_pause(UART_HandleTypeDef *huart );
void df_resume(UART_HandleTypeDef *huart );
void df_setVolume(UART_HandleTypeDef *huart , uint8_t volume);

#endif /* DFPLAYER_DFPLAYER_H_ */
