#ifndef _I2C_H
#define _I2C_H

#include "stm32.h"
#include "gpio.h"
#include "hal_types.h"

//#define SOFT_I2C

#define I2C_100KHz_SPEED            100000
#define I2C_400KHz_SPEED            400000

#define I2C_OK		0
#define I2C_ERROR	1

//*****************************************************************************
//
//! \addtogroup STM32F1xx_I2C_Slave_Error I2C Master error status
//! \brief Values that show I2C Master error status
//! Values that can be passed to xx
//! as the xx parameter.
//! @{
//
//*****************************************************************************

#define I2C_MASTER_ERR_NONE     0
#define I2C_MASTER_ERR_ADDR_ACK 0x00000004
#define I2C_MASTER_ERR_DATA_ACK 0x00000008
#define I2C_MASTER_ERR_ARB_LOST 0x00000010

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

//uint8_t i2cWriteReg(i2c_dev *dev, uint8_t addr, uint8_t* reg, uint8_t lenreg, uint8_t* buf, uint8_t lenbuf);
//uint8_t i2cReadReg(i2c_dev *dev, uint8_t addr, uint8_t* reg, uint8_t lenreg, uint8_t* buf, uint8_t lenbuf);

//uint8_t i2cWriteBuffer(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
//uint8_t i2cWrite(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t data);
//uint8_t i2cRead(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t* buf);
//uint16_t i2cGetErrorCounter(i2c_dev *dev);

unsigned long I2CMasterWriteS1(i2c_dev *dev, unsigned char ucSlaveAddr, unsigned char ucData, unsigned char bEndTransmition);
unsigned long I2CMasterWriteS2(i2c_dev *dev, unsigned char ucData, unsigned char bEndTransmition);
unsigned long I2CMasterWriteBufS1(i2c_dev *dev, unsigned char ucSlaveAddr, const unsigned char *pucDataBuf, unsigned long ulLen, unsigned char bEndTransmition);
unsigned long I2CMasterWriteBufS2(i2c_dev *dev, const unsigned char *pucDataBuf, unsigned long ulLen, unsigned char bEndTransmition);

unsigned long
I2CMasterReadS1(i2c_dev *dev,
                unsigned char ucSlaveAddr,
                unsigned char *pucData,
                unsigned char bEndTransmition);
unsigned long
I2CMasterReadS2(i2c_dev *dev,
                unsigned char *pucData,
                unsigned char bEndTransmition);
unsigned long
I2CMasterReadBufS1(i2c_dev *dev, unsigned char ucSlaveAddr,
                   unsigned char* pucDataBuf, unsigned long ulLen,
                   unsigned char bEndTransmition);
unsigned long
I2CMasterReadBufS2(i2c_dev *dev, unsigned char *pucDataBuf,
                   unsigned long ulLen, unsigned char bEndTransmition);

extern i2c_dev* const _I2C1;
extern i2c_dev* const _I2C2;

#ifdef __cplusplus
  }
#endif

#endif
