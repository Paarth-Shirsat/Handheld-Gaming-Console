#ifndef I2C_H
#define I2C_H

#include "stm32f1xx.h"
#include <stdint.h>

void i2c_init(void);

void i2c_start(void);
void i2c_write_address(uint8_t addr);

void i2c_write_data(uint8_t data);
uint8_t i2c_read_data(uint8_t ack);
void i2c_stop(void);

void i2c_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg_addr);

#endif