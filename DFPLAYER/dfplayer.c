/*
 * dfplayer.c
 *
 *  Created on: Aug 6, 2025
 *      Author: taher
 */

#include <stdint.h>
#include "dfplayer.h"

void df_sendCommand(UART_HandleTypeDef *huart , uint8_t command ,uint16_t val){
	uint8_t message[10];
	message[0] = 0x7E;
	message[1] = 0xFF;
	message[2] = 0x06;
	message[3] = command;
	message[4] = 0x00;
	message[5] = (val >> 8)&0xFF;
	message[6] = val&0xFF;
	uint16_t sum = message[1] + message[2] + message[3] + message[4] + message[5] + message[6];
	uint16_t checksum = 0xFFFF - sum + 1;
	message[7] =(checksum >> 8)&0xFF;
	message[8] =(checksum)&0xFF;
	message[9] =0xEF;

	uart_sendData(huart , message , 10);

}

void df_play(UART_HandleTypeDef *huart , uint16_t track){
	df_sendCommand(huart ,0x03,track);
}
void df_playFolder(UART_HandleTypeDef *huart , uint16_t folder , uint16_t track){
	uint16_t val = (folder << 8) | (track&0xFF);
	df_sendCommand(huart , 0x0F , val);
}
void df_pause(UART_HandleTypeDef *huart){
	df_sendCommand(huart,0x0E , 0);
}
void df_resume(UART_HandleTypeDef *huart){
	df_sendCommand(huart , 0x0D , 0);
}
void df_setVolume(UART_HandleTypeDef *huart , uint8_t volume){
	if(volume > 30) volume = 30;
	df_sendCommand(huart , 0x06 , volume);
}


