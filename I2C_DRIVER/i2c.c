#include "i2c.h"
void i2c_init(void) {
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;   

    // Configure gpiob pins , pin6(scl) and pin7(sda)
    GPIOB->CRL &= ~((0xF << (6 * 4)) | (0xF << (7 * 4))); //clearing
    GPIOB->CRL |= (0xB << (6 * 4)) | (0xB << (7 * 4));    // setting open-drain 

    //reset
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    I2C1->CR1 &= ~I2C_CR1_PE;         // disabling before config
    I2C1->CR2 = 36;                   // apb1= 36 MHz
    I2C1->CCR = 180;                  // 36 * 10^6 / 2 * 100*10^3 = 180
    I2C1->TRISE = 37;                 // TRISE = 37
    I2C1->CR1 |= I2C_CR1_PE;          // enabling peripheral(i2c1)
}
void i2c_start(void) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
    (void) I2C1->SR1;
}
void i2c_write_address(uint8_t addr) {
    I2C1->DR = addr;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void) I2C1->SR1;
    (void) I2C1->SR2;
}

void i2c_write_data(uint8_t data) {
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));
}

uint8_t i2c_read_data(uint8_t ack) {
    if (ack)
        I2C1->CR1 |= I2C_CR1_ACK;
    else
        I2C1->CR1 &= ~I2C_CR1_ACK;
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    return I2C1->DR;
}

void i2c_stop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
}
void i2c_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    i2c_start();
    i2c_write_address(dev_addr << 1);  
    i2c_write_data(reg_addr);
    i2c_write_data(data);
    i2c_stop();
}
uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg_addr) {
    uint8_t data;
    i2c_start();
    i2c_write_address(dev_addr << 1);  
    i2c_write_data(reg_addr);
    i2c_start();                       
    i2c_write_address((dev_addr << 1) | 1);  
    data = i2c_read_data(0);
    i2c_stop();


    return data;
}