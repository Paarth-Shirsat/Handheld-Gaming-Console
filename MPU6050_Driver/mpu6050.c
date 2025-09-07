#include "mpu6050.h"
#include "i2c.h"
void mpu6050_init(void) {
    i2c_write_reg(MPU6050_ADDR, MPU6050_PWR_MGMT_1, 0x00);
}

uint8_t get_whoAmI(void) {
    return i2c_read_reg(MPU6050_ADDR, MPU6050_WHOAMI); //0x68
}
void mpu6050_getData(mpu6050_data *data) {
    uint8_t raw[14];

    i2c_start();
    i2c_write_address(MPU6050_ADDR << 1);
    i2c_write_data(MPU6050_XOUT_ACCR);           
    i2c_start();                                        
    i2c_write_address((MPU6050_ADDR << 1) | 0x01);      

    for (int i = 0; i < 13; i++) {
        raw[i] = i2c_read_data(1);
    }
    raw[13] = i2c_read_data(0);

    i2c_stop();

    int16_t ax = (raw[0] << 8) | raw[1];
    int16_t ay = (raw[2] << 8) | raw[3];
    int16_t az = (raw[4] << 8) | raw[5];
    int16_t gx = (raw[8] << 8) | raw[9];
    int16_t gy = (raw[10] << 8) | raw[11];
    int16_t gz = (raw[12] << 8) | raw[13];
    data->x_accr = ax / 16384.0f;
    data->y_accr = ay / 16384.0f;
    data->z_accr = az / 16384.0f;

    data->x_gyro = gx / 131.0f;
    data->y_gyro = gy / 131.0f;
    data->z_gyro = gz / 131.0f;
}
