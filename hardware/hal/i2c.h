#ifndef _I2C_H
#define _I2C_H

#include "stm32.h"
#include "gpio.h"
#include "hal_types.h"

#define SOFT_I2C

#define I2C_100KHz_SPEED            100000
#define I2C_400KHz_SPEED            400000

#define I2C_OK		0
#define I2C_ERROR	1

typedef struct i2c_dev {
    I2C_TypeDef*         I2Cx;
    gpio_dev*            gpio_port;
    uint8_t              sda_pin;
    uint8_t              scl_pin;
#ifndef SOFT_I2C
    uint32_t             clk;
    rcc_clockcmd         clkcmd;
    rcc_clockcmd         clkrst;
    IRQn_Type            ev_nvic_line;  /* Event IRQ number */
    IRQn_Type            er_nvic_line;  /* Error IRQ number */
    uint32_t             speed;

    volatile uint16_t    i2cErrorCount;
    volatile bool        error;
    volatile bool        busy;

    volatile uint8_t     addr;
    volatile uint8_t     reg;
    volatile uint8_t     bytes;
    volatile uint8_t     writing;
    volatile uint8_t     reading;
    volatile uint8_t*    write_p;
    volatile uint8_t*    read_p;
#endif
} i2c_dev;

#ifdef __cplusplus
  extern "C" {
#endif

void i2cInit(i2c_dev *dev, uint32_t speed);

uint8_t i2cWriteReg(i2c_dev *dev, uint8_t addr, uint8_t* reg, uint8_t lenreg, uint8_t* buf, uint8_t lenbuf);
uint8_t i2cReadReg(i2c_dev *dev, uint8_t addr, uint8_t* reg, uint8_t lenreg, uint8_t* buf, uint8_t lenbuf);

uint8_t i2cWriteBuffer(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
uint8_t i2cWrite(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t data);
uint8_t i2cRead(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t* buf);
uint16_t i2cGetErrorCounter(i2c_dev *dev);

extern i2c_dev* const _I2C1;
extern i2c_dev* const _I2C2;

#ifdef __cplusplus
  }
#endif

#endif
