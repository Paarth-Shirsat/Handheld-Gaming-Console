#include <stdint.h>
#ifndef MPU6050_H
#define MPU6050_H

#define MPU6050_ADDR 0x68
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_WHOAMI 0x75
#define MPU6050_XOUT_ACCR 0x3B
typedef struct{
    int16_t x_accr;
    int16_t y_accr;
    int16_t z_accr;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
} mpu6050_data  ;

void mpu6050_init();
void mpu6050_getData(mpu6050_data *data);
uint8_t get_whoAmI();



#endif