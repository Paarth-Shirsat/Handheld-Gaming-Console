#include <mpu6050.h>
#include <main.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef MPU6050_Init(void){
	uint8_t check, data;

	HAL_StatusTypeDef ret;
	ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);
	if(ret != HAL_OK) return ret;

	if(check == 104){
		data = 0;
		if((ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 1000)) != HAL_OK) return ret;	//Wake up

		data = 0x07;
		if((ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 1000)) != HAL_OK) return ret;	//Sample Rate Divider to 7

		data = 0x00;
		if((ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 1000)) != HAL_OK) return ret;	//FS_SEL = 0

		data = 0x00;
		if((ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 1000)) != HAL_OK) return ret;	//AFS_SEL = 0

		data = 0x03;
		if((ret = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, 1000)) != HAL_OK) return ret;	//DLPF active

		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef MPU6050_Read_Accel(float *Ax, float *Ay, float *Az){
	uint8_t Rec_Data[6];
	HAL_StatusTypeDef ret;
	if((ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000)) != HAL_OK) return ret;

	int16_t Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
	int16_t Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
	int16_t Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

	*Ax = Accel_X_RAW / 16384.0;
	*Ay = Accel_Y_RAW / 16384.0;
	*Az = Accel_Z_RAW / 16384.0;

	return HAL_OK;
}

HAL_StatusTypeDef MPU6050_Read_Gyro(float *Gx, float *Gy, float *Gz){
	 uint8_t Rec_Data[6];
	 HAL_StatusTypeDef ret;
	 if((ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000)) != HAL_OK) return ret;

	 int16_t Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
	 int16_t Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
	 int16_t Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

	 *Gx = Gyro_X_RAW / 131.0;
	 *Gy = Gyro_Y_RAW / 131.0;
	 *Gz = Gyro_Z_RAW / 131.0;

	 return HAL_OK;
}

HAL_StatusTypeDef MPU6050_Read_Temp(float *Temp) {
    uint8_t Rec_Data[2];
    HAL_StatusTypeDef ret;
    if((ret = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, TEMP_OUT_H_REG, 1, Rec_Data, 2, 1000)) != HAL_OK) return ret;

    int16_t temp_raw = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);

    *Temp = (temp_raw / 340.0) + 36.53;

    return HAL_OK;
}
