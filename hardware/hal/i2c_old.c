#include "i2c.h"

DMA_InitTypeDef   DMA_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
__IO uint32_t sTimeout = I2C_LONG_TIMEOUT;

static i2c_dev i2c_dev1 = {
    .I2Cx         = I2C1,
    .gpio_port    = &gpiob,
    .sda_pin      = 9,
    .scl_pin      = 8,
    .clk       	  = RCC_APB1Periph_I2C1,
    .clkcmd       = RCC_APB1PeriphClockCmd,
    .clkrst       = RCC_APB1PeriphResetCmd,
    .dr_address   = (uint32_t)0x40005410,

    .dma          = DMA1,
    .dma_clk      = RCC_AHBPeriph_DMA1,
    .dma_clkcmd   = RCC_AHBPeriphClockCmd,

    .dma_tx_chan  = DMA1_Channel6,
    .dma_tx_irq   = DMA1_Channel6_IRQn,
    .dma_tx_fgl   = DMA1_FLAG_GL6,
    .dma_tx_ftc   = DMA1_FLAG_TC6,
    .dma_tx_fht   = DMA1_FLAG_HT6,
    .dma_tx_fte   = DMA1_FLAG_TE6,

    .dma_rx_chan  = DMA1_Channel7,
    .dma_rx_irq   = DMA1_Channel7_IRQn,
    .dma_rx_fgl   = DMA1_FLAG_GL7,
    .dma_rx_ftc   = DMA1_FLAG_TC7,
    .dma_rx_fht   = DMA1_FLAG_HT7,
    .dma_rx_fte   = DMA1_FLAG_TE7,

    .ev_nvic_line = I2C1_EV_IRQn,
    .er_nvic_line = I2C1_ER_IRQn,
};
/** I2C1 device */
i2c_dev* const _I2C1 = &i2c_dev1;

static i2c_dev i2c_dev2 = {
    .I2Cx         = I2C2,
    .gpio_port    = &gpiob,
    .sda_pin      = 11,
    .scl_pin      = 10,
    .clk       	  = RCC_APB1Periph_I2C2,
    .clkcmd       = RCC_APB1PeriphClockCmd,
    .clkrst       = RCC_APB1PeriphResetCmd,
    .dr_address   = (uint32_t)0x40005410,

    .dma          = DMA1,
    .dma_clk      = RCC_AHBPeriph_DMA1,
    .dma_clkcmd   = RCC_AHBPeriphClockCmd,

    .dma_tx_chan  = DMA1_Channel4,
    .dma_tx_irq   = DMA1_Channel4_IRQn,
    .dma_tx_fgl   = DMA1_FLAG_GL4,
    .dma_tx_ftc   = DMA1_FLAG_TC4,
    .dma_tx_fht   = DMA1_FLAG_HT4,
    .dma_tx_fte   = DMA1_FLAG_TE4,

    .dma_rx_chan  = DMA1_Channel5,
    .dma_rx_irq   = DMA1_Channel5_IRQn,
    .dma_rx_fgl   = DMA1_FLAG_GL5,
    .dma_rx_ftc   = DMA1_FLAG_TC5,
    .dma_rx_fht   = DMA1_FLAG_HT5,
    .dma_rx_fte   = DMA1_FLAG_TE5,

    .ev_nvic_line = I2C2_EV_IRQn,
    .er_nvic_line = I2C2_ER_IRQn,
};
/** I2C2 device */
i2c_dev* const _I2C2 = &i2c_dev2;

__IO uint16_t  I2CAddress = 0;
__IO uint32_t  I2CTimeout = I2C_LONG_TIMEOUT;
__IO uint16_t* I2CDataReadPointer;
__IO uint8_t*  I2CDataWritePointer;

/**
 * @brief  DeInitializes peripherals used by the I2C EEPROM driver.
 * @param  None
 * @retval None
 */
void I2C_LowLevel_DeInit(i2c_dev *dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* sEE_I2C Peripheral Disable */
    I2C_Cmd(dev->I2Cx, DISABLE);

    /* sEE_I2C DeInit */
    I2C_DeInit(dev->I2Cx);

    /*!< sEE_I2C Periph clock disable */
	dev->clkcmd(dev->clk, DISABLE);

    /*!< GPIO configuration */
    /*!< Configure I2C pins: SCL */
    GPIO_InitStructure.GPIO_Pin = BIT(dev->scl_pin);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);

    /*!< Configure I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = BIT(dev->sda_pin);
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);

	/* Configure and enable I2C DMA TX Channel interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = dev->dma_tx_irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2C_DMA_PREPRIO;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2C_DMA_SUBPRIO;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure and enable I2C DMA RX Channel interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = dev->dma_rx_irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2C_DMA_PREPRIO;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2C_DMA_SUBPRIO;
	NVIC_Init(&NVIC_InitStructure);

	/* Disable and Deinitialize the DMA channels */
	DMA_Cmd(dev->dma_tx_chan, DISABLE);
	DMA_Cmd(dev->dma_rx_chan, DISABLE);
	DMA_DeInit(dev->dma_tx_chan);
	DMA_DeInit(dev->dma_rx_chan);
}

/**
 * @brief  Initializes peripherals used by the I2C EEPROM driver.
 * @param  None
 * @retval None
 */
void I2C_LowLevel_Init(i2c_dev *dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable the i2c */
	dev->clkcmd(dev->clk, ENABLE);

    /* Reset the Peripheral */
	dev->clkrst(dev->clk, ENABLE);
	dev->clkrst(dev->clk, DISABLE);

    /* Enable the GPIOs for the SCL/SDA Pins */
	dev->gpio_port->clkcmd(dev->gpio_port->clk, ENABLE);

    /* GPIO configuration */
    /* Configure SCL */
    GPIO_InitStructure.GPIO_Pin = BIT(dev->scl_pin);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);

    /* Configure SDA */
    GPIO_InitStructure.GPIO_Pin = BIT(dev->sda_pin);
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);

    /* Configure and enable I2C DMA TX Channel interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = dev->dma_tx_irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2C_DMA_PREPRIO;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2C_DMA_SUBPRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure and enable I2C DMA RX Channel interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = dev->dma_rx_irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2C_DMA_PREPRIO;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2C_DMA_SUBPRIO;
    NVIC_Init(&NVIC_InitStructure);

    /*!< I2C DMA TX and RX channels configuration */
    /* Enable the DMA clock */
	dev->dma_clkcmd(dev->dma_clk, ENABLE);

    /* Clear any pending flag on Rx Stream  */
    DMA_ClearFlag(dev->dma_tx_fgl | dev->dma_tx_fht | dev->dma_tx_ftc | dev->dma_tx_fte);
    /* Disable the EE I2C Tx DMA stream */
    DMA_Cmd(dev->dma_tx_chan, DISABLE);
    /* Configure the DMA stream for the EE I2C peripheral TX direction */
	DMA_DeInit(dev->dma_tx_chan);
	DMA_InitStructure.DMA_PeripheralBaseAddr = dev->dr_address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
	DMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(dev->dma_tx_chan, &DMA_InitStructure);

    /* Clear any pending flag on Rx Stream */
    DMA_ClearFlag(dev->dma_rx_fgl | dev->dma_rx_fht | dev->dma_rx_ftc | dev->dma_rx_fte);
    /* Disable the EE I2C DMA Rx stream */
    DMA_Cmd(dev->dma_rx_chan, DISABLE);
    /* Configure the DMA stream for the EE I2C peripheral RX direction */
	DMA_DeInit(dev->dma_rx_chan);
	DMA_Init(dev->dma_rx_chan, &DMA_InitStructure);

    /* Enable the DMA Channels Interrupts */
	DMA_ITConfig(dev->dma_tx_chan, DMA_IT_TC, ENABLE);
	DMA_ITConfig(dev->dma_rx_chan, DMA_IT_TC, ENABLE);
}

void i2c_deinit(i2c_dev *dev)
{
	I2C_LowLevel_DeInit(dev);
}

void i2c_init(i2c_dev *dev, uint16_t address, uint32_t speed)
{
    I2C_InitTypeDef I2C_InitStructure;

	I2C_LowLevel_Init(dev);

    /* I2C configuration */
    I2C_StructInit(&I2C_InitStructure);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = address;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = speed;

    I2C_ITConfig(dev->I2Cx, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);

    /* I2C Peripheral Enable */
    I2C_Cmd(dev->I2Cx, ENABLE);
    /* Apply I2C configuration after enabling it */
    I2C_Init(dev->I2Cx, &I2C_InitStructure);

//	/* Enable the sEE_I2C peripheral DMA requests */
//	I2C_DMACmd(dev->I2Cx, ENABLE);
}

/**
 * @brief  Initializes DMA channel used by the I2C EEPROM driver.
 * @param  None
 * @retval None
 */
void I2C_LowLevel_DMAConfig(i2c_dev *dev, uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction)
{
    /* Initialize the DMA with the new parameters */
	if (Direction == I2C_DIRECTION_TX)
	{
		/* Configure the DMA Tx Stream with the buffer address and the buffer size */
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
		DMA_Init(dev->dma_tx_chan, &DMA_InitStructure);
	}
    else
	{
    	/* Configure the DMA Rx Stream with the buffer address and the buffer size */
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;
		DMA_Init(dev->dma_rx_chan, &DMA_InitStructure);
	}
}

/* Send a buffer to the i2c port */
uint32_t i2c_write(i2c_dev *dev, uint8_t addr, uint8_t *tx_buff, uint8_t *len)
{

    /* Set the pointer to the Number of data to be written. This pointer will be used
     by the DMA Transfer Completer interrupt Handler in order to reset the
     variable to 0. User should check on this variable in order to know if the
     DMA transfer has been complete or not. */
    I2CDataWritePointer = len;

    uint16_t sent = 0;
    uint8_t *buffer = tx_buff;

    sent = 0;

    // While the bus is busy
    /*!< While the bus is busy */
    sTimeout = I2C_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(dev->I2Cx, I2C_FLAG_BUSY ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}

    // Send START condition
    I2C_GenerateSTART(dev->I2Cx, ENABLE);

    // Test on EV5 and clear it (cleared by reading SR1 then writing to DR)
    sTimeout = I2CFLAGTIMEOUT;
    while (!I2C_CheckEvent(dev->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}

    // Send address for write
    I2C_Send7bitAddress(dev->I2Cx, addr, I2C_Direction_Transmitter );

    sTimeout = I2CFLAGTIMEOUT;
    // Test on EV6 and clear it
    while (!I2C_CheckEvent(dev->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}

    I2C_SendData(dev->I2Cx, *buffer++);

    // Test on EV8 and clear it
    sTimeout = I2CFLAGTIMEOUT;
    while (!I2C_CheckEvent(dev->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}
    if ((uint16_t)(*len) < 2)
	{
		/* Send the current byte */
		I2C_SendData(dev->I2Cx, *buffer);
		/* Point to the next byte to be written */
		sent++;
		/* Test on EV8 and clear it */
		sTimeout = I2CFLAGTIMEOUT;
		while (!I2C_CheckEvent(dev->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED ))
		{
			if ((sTimeout--) == 0)
				return I2C_ERROR;
		}

		/*!< STOP condition */
		I2C_GenerateSTOP(dev->I2Cx, DISABLE);
		I2C_ClearFlag(dev->I2Cx, I2C_FLAG_STOPF );
		/* Send STOP condition */
		I2C_GenerateSTOP(dev->I2Cx, ENABLE);
	}
    else
	{
		/* Configure the DMA Tx Channel with the buffer address and the buffer size */
		I2C_LowLevel_DMAConfig(dev, (uint32_t) buffer, (uint8_t)(*len), I2C_DIRECTION_TX);

		/* Enable the DMA Tx Stream */
		DMA_Cmd(dev->dma_tx_chan, ENABLE);

		/* Enable the sEE_I2C peripheral DMA requests */
		I2C_DMACmd(dev->I2Cx, ENABLE);
	}

    return I2C_OK;
}

/* Read a buffer from the i2c port */
uint32_t i2c_read(i2c_dev *dev, uint8_t addr, uint8_t *tx_buff, uint8_t txlen, uint8_t *rx_buff, uint8_t *rxlen)
{

    /* Set the pointer to the Number of data to be read. This pointer will be used
     by the DMA Transfer Completer interrupt Handler in order to reset the
     variable to 0. User should check on this variable in order to know if the
     DMA transfer has been complete or not. */
    I2CDataReadPointer = (uint16_t*) rxlen;

    uint8_t *buffer8 = rx_buff;

    // While the bus is busy
    sTimeout = I2C_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(dev->I2Cx, I2C_FLAG_BUSY ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}

    // Send START condition
    I2C_GenerateSTART(dev->I2Cx, ENABLE);

    // Test on EV5 and clear it (cleared by reading SR1 then writing to DR)
    sTimeout = I2CFLAGTIMEOUT;
    while (!I2C_CheckEvent(dev->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}

    // Send address for write
    I2C_Send7bitAddress(dev->I2Cx, addr, I2C_Direction_Transmitter );
    sTimeout = I2CFLAGTIMEOUT;
    // Test on EV6 and clear it
    while (!I2C_CheckEvent(dev->I2Cx,
	    I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}

    I2C_SendData(dev->I2Cx, *tx_buff++);

    // Test on EV8 and clear it
    I2CTimeout = I2CFLAGTIMEOUT;
    while (I2C_GetFlagStatus(dev->I2Cx, I2C_FLAG_BTF ) == RESET)
	{
		if ((I2CTimeout--) == 0)
			return I2C_ERROR;
	}

    // Send STRAT condition a second time
    I2C_GenerateSTART(dev->I2Cx, ENABLE);

    // Test on EV5 and clear it (cleared by reading SR1 then writing to DR)
    sTimeout = I2CFLAGTIMEOUT;
    while (!I2C_CheckEvent(dev->I2Cx, I2C_EVENT_MASTER_MODE_SELECT ))
	{
		if ((sTimeout--) == 0)
			return I2C_ERROR;
	}

    // Send address for read
    I2C_Send7bitAddress(dev->I2Cx, addr, I2C_Direction_Receiver );

    if ((uint16_t)(*rxlen) < 2)
	{
		I2CTimeout = I2CFLAGTIMEOUT;
		while (I2C_GetFlagStatus(dev->I2Cx, I2C_FLAG_ADDR ) == RESET)
		{
			if ((I2CTimeout--) == 0)
				return I2C_ERROR;
		}
		// Disable Acknowledgement
		I2C_AcknowledgeConfig(dev->I2Cx, DISABLE);

		/* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
		(void) dev->I2Cx->SR2;
		/*!< STOP condition */
		I2C_GenerateSTOP(dev->I2Cx, DISABLE);
		I2C_ClearFlag(dev->I2Cx, I2C_FLAG_STOPF );
		/* Send STOP condition */
		I2C_GenerateSTOP(dev->I2Cx, ENABLE);

		/* Wait for the byte to be received */
		I2CTimeout = I2CFLAGTIMEOUT;
		while (I2C_GetFlagStatus(dev->I2Cx, I2C_FLAG_RXNE ) == RESET)
		{
			if ((I2CTimeout--) == 0)
				return I2C_ERROR;
		}

		/*!< Read the byte received from the EEPROM */
		*buffer8 = I2C_ReceiveData(dev->I2Cx);

		(uint16_t)(*rxlen)--;
		/* Wait to make sure that STOP control bit has been cleared */
		I2CTimeout = I2CFLAGTIMEOUT;
		while (dev->I2Cx->CR1 & I2C_CR1_STOP )
		{
			if ((I2CTimeout--) == 0)
				return I2C_ERROR;
		}

		// Re-Enable Acknowledgement to be ready for another reception
		I2C_AcknowledgeConfig(dev->I2Cx, ENABLE);
	}
    else/* More than one Byte Master Reception procedure (DMA) -----------------*/
	{
		/*!< Test on EV6 and clear it */
		I2CTimeout = I2CFLAGTIMEOUT;
		while (!I2C_CheckEvent(dev->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ))
		{
			if ((I2CTimeout--) == 0)
				return I2C_ERROR;
		}

		/* Configure the DMA Rx Channel with the buffer address and the buffer size */
		I2C_LowLevel_DMAConfig(dev, (uint32_t) buffer8, (uint16_t)(*rxlen), I2C_DIRECTION_RX);

		/* Inform the DMA that the next End Of Transfer Signal will be the last one */
		I2C_DMALastTransferCmd(dev->I2Cx, ENABLE);

		/* Enable the DMA Rx Stream */
		DMA_Cmd(dev->dma_rx_chan, ENABLE);

		/* Enable the sEE_I2C peripheral DMA requests */
		I2C_DMACmd(dev->I2Cx, ENABLE);
	}
    return I2C_OK;
}

/**
 * @brief  This function handles the DMA Tx Channel interrupt Handler.
 * @param  None
 * @retval None
 */
void I2C_DMA_TX_IRQHandler(i2c_dev *dev)
    {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(dev->dma_tx_ftc) != RESET)
	{
	/* Disable the DMA Tx Stream and Clear TC flag */
	DMA_Cmd(dev->dma_tx_chan, DISABLE);
	DMA_ClearFlag(dev->dma_tx_ftc);

	/*!< Wait till all data have been physically transferred on the bus */
	I2CTimeout = I2C_LONG_TIMEOUT;
	while (!I2C_GetFlagStatus(dev->I2Cx, I2C_FLAG_BTF ))
	    {
	    if ((I2CTimeout--) == 0)
		return;
	    }

	/*!< Send STOP condition */
	I2C_GenerateSTOP(dev->I2Cx, ENABLE);

	/* Reset the variable holding the number of data to be written */
	*I2CDataWritePointer = 0;
	}
    }

/**
 * @brief  This function handles the DMA Rx Channel interrupt Handler.
 * @param  None
 * @retval None
 */
void I2C_DMA_RX_IRQHandler(i2c_dev *dev)
    {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(dev->dma_rx_ftc) != RESET)
	{
	/*!< Send STOP Condition */
	I2C_GenerateSTOP(dev->I2Cx, ENABLE);

	/* Disable the DMA Rx Stream and Clear TC Flag */
	DMA_Cmd(dev->dma_rx_chan, DISABLE);
	DMA_ClearFlag(dev->dma_rx_ftc);

	/* Reset the variable holding the number of data to be read */
	*I2CDataReadPointer = 0;
	}
    }

/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
uint32_t sEE_TIMEOUT_UserCallback(void)
    {
    /* Block communication and all processes */
    while (1)
	{
	}
    }

void DMA1_Channel4_IRQHandler(void)
{
	I2C_DMA_TX_IRQHandler(_I2C2);
}
void DMA1_Channel5_IRQHandler(void)
{
	I2C_DMA_RX_IRQHandler(_I2C2);
}
void DMA1_Channel6_IRQHandler(void)
{
	I2C_DMA_TX_IRQHandler(_I2C1);
}
void DMA1_Channel7_IRQHandler(void)
{
	I2C_DMA_RX_IRQHandler(_I2C1);
}

