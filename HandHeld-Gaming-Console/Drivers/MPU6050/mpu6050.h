/*
 * mpu6050.h
 *
 *  Created on: Aug 3, 2025
 *      Author: Paarth
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#define MPU6050_ADDR 0xD0

#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75
#define CONFIG_REG 0x1A

void MPU6050_Init();
void MPU6050_Read_Accel(int *Ax, int *Ay, int *Az);
void MPU6050_Read_Gyro(int *Gx, int *Gy, int *Gz);
void MPU6050_Read_Temp(int *Temp);

#endif /* INC_MPU6050_H_ */
