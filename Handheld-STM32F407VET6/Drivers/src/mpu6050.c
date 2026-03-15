#include <mpu6050.h>
#include <main.h>
#include <stdio.h>

extern I2C_HandleTypeDef MPU6050_I2C;

void MPU6050_Init(void){
	uint8_t check, data;
	HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);
	if(check == 104){
		data = 0;
		HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 1000);
		data = 0x07;
		HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 1000);
		data = 0x00;
		HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 1000);
		data = 0x00;
		HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 1000);
		data = 0x03;
		HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, 1000);
	}
}

void MPU6050_Read_Accel(int *Ax, int *Ay, int *Az){
	uint8_t Rec_Data[6];
	HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);
	int16_t Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
	int16_t Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
	int16_t Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
	*Ax = (int)(((int32_t)Accel_X_RAW + 32768) >> 4);
	*Ay = (int)(((int32_t)Accel_Y_RAW + 32768) >> 4);
	*Az = (int)(((int32_t)Accel_Z_RAW + 32768) >> 4);
}

void MPU6050_Read_Gyro(int *Gx, int *Gy, int *Gz){
	uint8_t Rec_Data[6];
	HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);
	int16_t Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
	int16_t Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
	int16_t Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
	*Gx = (int)(((int32_t)Gyro_X_RAW + 32768) >> 4);
	*Gy = (int)(((int32_t)Gyro_Y_RAW + 32768) >> 4);
	*Gz = (int)(((int32_t)Gyro_Z_RAW + 32768) >> 4);
}

void MPU6050_Read_Temp(int *Temp){
	uint8_t Rec_Data[2];
	HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, TEMP_OUT_H_REG, 1, Rec_Data, 2, 1000);
	int16_t temp_raw = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
	*Temp = (int)(((int32_t)temp_raw + 32768) >> 4);
}
