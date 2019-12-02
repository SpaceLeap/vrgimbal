#include "usart.h"
#include "systick.h"

/*
 * Devices
 */

static ring_buffer usart1_txrb;
static ring_buffer usart1_rxrb;
static usart_dev usart1 = {
    .USARTx     = USART1,
    .clk	    = RCC_APB2Periph_USART1,
    .clkcmd     = RCC_APB2PeriphClockCmd,
    .txrb       = &usart1_txrb,
    .rxrb       = &usart1_rxrb,
    .max_baud   = 4500000UL,
    .irq	    = USART1_IRQn,
    .tx_timeout = 10000,
};
/** USART1 device */
usart_dev *_USART1 = &usart1;

static ring_buffer usart2_txrb;
static ring_buffer usart2_rxrb;
static usart_dev usart2 = {
    .USARTx     = USART2,
    .clk	    = RCC_APB1Periph_USART2,
    .clkcmd     = RCC_APB1PeriphClockCmd,
    .txrb       = &usart2_txrb,
    .rxrb       = &usart2_rxrb,
    .max_baud   = 2250000UL,
    .irq	    = USART2_IRQn,
    .tx_timeout = 10000,
};
/** USART2 device */
usart_dev *_USART2 = &usart2;

static ring_buffer usart3_txrb;
static ring_buffer usart3_rxrb;
static usart_dev usart3 = {
    .USARTx     = USART3,
    .clk	    = RCC_APB1Periph_USART3,
    .clkcmd     = RCC_APB1PeriphClockCmd,
    .txrb       = &usart3_txrb,
    .rxrb       = &usart3_rxrb,
    .max_baud   = 2250000UL,
    .irq	    = USART3_IRQn,
    .tx_timeout = 10000,
};
/** USART3 device */
usart_dev *_USART3 = &usart3;

static ring_buffer uart4_txrb;
static ring_buffer uart4_rxrb;
static usart_dev uart4 = {
    .USARTx     = UART4,
    .clk	    = RCC_APB1Periph_UART4,
    .clkcmd     = RCC_APB1PeriphClockCmd,
    .txrb       = &uart4_txrb,
    .rxrb       = &uart4_rxrb,
    .max_baud   = 2250000UL,
    .irq	    = UART4_IRQn,
    .tx_timeout = 10000,
};
/** UART4 device */
usart_dev *_UART4 = &uart4;

static ring_buffer uart5_txrb;
static ring_buffer uart5_rxrb;
static usart_dev uart5 = {
    .USARTx     = UART5,
    .clk	    = RCC_APB1Periph_UART5,
    .clkcmd     = RCC_APB1PeriphClockCmd,
    .txrb       = &uart5_txrb,
    .rxrb       = &uart5_rxrb,
    .max_baud   = 2250000UL,
    .irq	    = UART5_IRQn,
    .tx_timeout = 10000,
};
/** UART5 device */
usart_dev *_UART5 = &uart5;

/**
 * @brief Initialize a serial port.
 * @param dev         Serial port to be initialized
 */
void usart_init(usart_dev *dev) {
	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(dev->USARTx));

    rb_init(dev->txrb, USART_TX_BUF_SIZE, dev->tx_buf);
    rb_init(dev->rxrb, USART_RX_BUF_SIZE, dev->rx_buf);
    
    // Turn on peripheral clocks
    dev->clkcmd(dev->clk, ENABLE);
}

void usart_setup(usart_dev *dev, uint32_t  baudRate, uint16_t  wordLength, uint16_t  stopBits, uint16_t  parity, uint16_t  mode, uint16_t  hardwareFlowControl) {
	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(dev->USARTx));
	assert_param(IS_USART_BAUDRATE(baudRate));
	assert_param(IS_USART_STOPBITS(stopBits));
	assert_param(IS_USART_PARITY(parity));
	assert_param(IS_USART_WORD_LENGTH(wordLength));
	assert_param(IS_USART_MODE(mode));
	assert_param(IS_USART_HARDWARE_FLOW_CONTROL(hardwareFlowControl));

	USART_InitTypeDef USART_config;
	USART_StructInit(&USART_config);
	USART_config.USART_BaudRate = baudRate;
	USART_config.USART_WordLength = wordLength;
	USART_config.USART_StopBits = stopBits;
	USART_config.USART_Parity = parity;
	USART_config.USART_Mode = mode;
	USART_config.USART_HardwareFlowControl = hardwareFlowControl;

	USART_OverSampling8Cmd(dev->USARTx, ENABLE);
	USART_Init(dev->USARTx, &USART_config);
	
    NVIC_InitTypeDef NVIC_InitStructure;          
    /* Enable the USART Interrupt */         
    NVIC_InitStructure.NVIC_IRQChannel = dev->irq;         
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         
    NVIC_Init(&NVIC_InitStructure); 

	/* Enable USART interrupt Rx request */         
	USART_ITConfig(dev->USARTx, USART_IT_RXNE, ENABLE);     
        	
}

/**
 * @brief Enable a serial port.
 *
 * USART is enabled in single buffer transmission mode, multibuffer
 * receiver mode, 8n1.
 *
 * Serial port must have a baud rate configured to work properly.
 *
 * @param dev Serial port to enable.
 * @see usart_set_baud_rate()
 */
void usart_enable(usart_dev *dev) {
	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(dev->USARTx));

	/* Enable USART */
	USART_Cmd(dev->USARTx, ENABLE); 	
}

/**
 * @brief Turn off a serial port.
 * @param dev Serial port to be disabled
 */
void usart_disable(usart_dev *dev) {
	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(dev->USARTx));

	/* Enable USART */
	USART_Cmd(dev->USARTx, DISABLE); 	
	
    /* Clean up buffer */
    usart_reset_rx(dev);
    usart_reset_tx(dev);
}

/**
 *  @brief Call a function on each USART.
 *  @param fn Function to call.
 */
void usart_foreach(void (*fn)(usart_dev*)) {
    fn(_USART1);
    fn(_USART2);
    fn(_USART3);
#if defined(MCU_STM32F407VG) || defined(stm32f407vg)
    fn(_UART4);
    fn(_UART5);
    fn(_USART6);    
#endif
}

/**
 * @brief Nonblocking USART transmit
 * @param dev Serial port to transmit over
 * @param buf Buffer to transmit
 * @param len Maximum number of bytes to transmit
 * @return Number of bytes transmitted
 */
uint32 usart_tx(usart_dev *dev, const uint8 *buf, uint32 len) {
    uint32 tosend = len;
    uint32 sent = 0;

    if (dev->usetxrb)
    {
    	while(tosend)
    	{
    		if (rb_is_full(dev->txrb))
    			break;
    		rb_insert(dev->txrb, *buf++);
    		sent++;
    		tosend--;
    	}
    	if (dev->txbusy == 0 && sent > 0)
    	{
    		dev->txbusy = 1;
			/*Enable TX Empty interrupt*/
    		dev->USARTx->CR1 |= USART_CR1_TXEIE;
    	}
    }
    else
    {
#if 0
        uint32_t rtime;
    	if (dev->use_timeout)
    		stopwatch_reset();
#endif
    	while(tosend)
    	{
	    	while (!(dev->USARTx->SR & USART_SR_TXE))
			{
#if 0
    				if (dev->use_timeout)
    				{
						rtime = stopwatch_getus();
						if (rtime >= dev->tx_timeout)
						{
							return sent;
						}
    				}
#endif
			}
	    	dev->USARTx->DR = *buf++;
    		tosend--;
    		sent++;
    	}
    }

	return sent;
}

/**
 * @brief Transmit an unsigned integer to the specified serial port in
 *        decimal format.
 *
 * This function blocks until the integer's digits have been
 * completely transmitted.
 *
 * @param dev Serial port to send on
 * @param val Number to print
 */
void usart_putudec(usart_dev *dev, uint32 val) 
{
    char digits[12];
    int i = 0;

    do {
        digits[i++] = val % 10 + '0';
        val /= 10;
    } while (val > 0);

    while (--i >= 0) {
        usart_putc(dev, digits[i]);
    }
}

/*
 * Interrupt handlers.
 */

static inline void usart_rx_irq(usart_dev *dev) {
#ifdef USART_SAFE_INSERT
    /* If the buffer is full and the user defines USART_SAFE_INSERT,
     * ignore new bytes. */
    rb_safe_insert(dev->rxrb, (uint8)dev->USARTx->DR);
#else
    /* By default, push bytes around in the ring buffer. */
    rb_push_insert(dev->rxrb, (uint8)dev->USARTx->DR);
#endif
}

static inline void usart_tx_irq(usart_dev *dev) {

	  /* Check USART Transmit Data Register Empty Interrupt */
	  if((dev->USARTx->SR & USART_SR_TXE) != 0)
	  {
	      if (!rb_is_empty(dev->txrb))
	      {
	    	  dev->USARTx->DR = rb_remove(dev->txrb);
	          dev->txbusy = 1;
	      }
	      else
	      {
	          /* Disable the USART Transmit Data Register Empty Interrupt */
	    	  dev->USARTx->CR1 &=  ~USART_CR1_TXEIE;
			  dev->txbusy = 0;
	      }
		}
}

void USART1_IRQHandler(void) {
	uart1_lic_millis=0; // syncro last byte received
    usart_rx_irq(_USART1);
	usart_tx_irq(_USART1);
}

void USART2_IRQHandler(void) {
	uart2_lic_millis=0; // syncro last byte received
    usart_rx_irq(_USART2);
	usart_tx_irq(_USART2);
}

void USART3_IRQHandler(void) {
	uart3_lic_millis=0; // syncro last byte received
    usart_rx_irq(_USART3);
	usart_tx_irq(_USART3);
}
void UART4_IRQHandler(void) {
	uart4_lic_millis=0; // syncro last byte received
    usart_rx_irq(_UART4);
	usart_tx_irq(_UART4);
}

void UART5_IRQHandler(void) {
	uart5_lic_millis=0; // syncro last byte received
    usart_rx_irq(_UART5);
	usart_tx_irq(_UART5);
}
