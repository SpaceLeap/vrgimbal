#ifndef _I2C_H
#define _I2C_H

#include "hal.h"
#include "stm32.h"

#define I2C_100KHz_SPEED            100000
#define I2C_400KHz_SPEED            400000

#define I2C_OK		0
#define I2C_ERROR	1

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define I2CFLAGTIMEOUT           ((uint32_t)0x10000)
#define I2C_LONG_TIMEOUT         ((uint32_t)(10 * I2CFLAGTIMEOUT))

#define I2C_DMA_PREPRIO                  0
#define I2C_DMA_SUBPRIO                  0

#define I2C_DIRECTION_TX                 0
#define I2C_DIRECTION_RX                 1

/* Time constant for the delay caclulation allowing to have a millisecond
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 24MHz then sEE_TIME_CONST should be 24. */
#define I2C_TIME_CONST                  24

typedef struct i2c_dev {
    I2C_TypeDef*         I2Cx;
    gpio_dev*            gpio_port;
    uint8_t              sda_pin;
    uint8_t              scl_pin;
    uint32_t             clk;
    rcc_clockcmd         clkcmd;
    rcc_clockcmd         clkrst;
    uint32_t             dr_address;

    DMA_TypeDef*         dma;
    uint32_t             dma_clk;
    rcc_clockcmd         dma_clkcmd;
    DMA_Channel_TypeDef* dma_tx_chan;
    IRQn_Type            dma_tx_irq;
    uint32_t             dma_tx_fgl;
    uint32_t             dma_tx_ftc;
    uint32_t             dma_tx_fht;
    uint32_t             dma_tx_fte;
    DMA_Channel_TypeDef* dma_rx_chan;
    IRQn_Type            dma_rx_irq;
    uint32_t             dma_rx_fgl;
    uint32_t             dma_rx_ftc;
    uint32_t             dma_rx_fht;
    uint32_t             dma_rx_fte;

    IRQn_Type            ev_nvic_line;  /* Event IRQ number */
    IRQn_Type            er_nvic_line;  /* Error IRQ number */
} i2c_dev;

#ifdef __cplusplus
  extern "C" {
#endif

void i2c_init(i2c_dev *dev, uint16_t address, uint32_t speed);
void i2c_deinit(i2c_dev *dev);
uint32_t i2c_write(i2c_dev *dev, uint8_t addr, uint8_t *tx_buff, uint8_t *len);
uint32_t i2c_read(i2c_dev *dev, uint8_t addr, uint8_t *tx_buff, uint8_t txlen, uint8_t *rx_buff, uint8_t *rxlen);

extern i2c_dev* const _I2C1;
extern i2c_dev* const _I2C2;

#ifdef __cplusplus
  }
#endif
 

#endif
