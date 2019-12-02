#include "i2c.h"
#include "util.h"
#include "delay.h"

static i2c_dev i2c_dev1 = {
    .I2Cx         = I2C1,
    .gpio_port    = &gpiob,
    .sda_pin      = 9,
    .scl_pin      = 8,
#ifndef SOFT_I2C
    .clk       	  = RCC_APB1Periph_I2C1,
    .clkcmd       = RCC_APB1PeriphClockCmd,
    .clkrst       = RCC_APB1PeriphResetCmd,
    .ev_nvic_line = I2C1_EV_IRQn,
    .er_nvic_line = I2C1_ER_IRQn,
    .speed        = I2C_400KHz_SPEED,
    .i2cErrorCount= 0,
    .error        = 0
#endif
};
/** I2C1 device */
i2c_dev* const _I2C1 = &i2c_dev1;

static i2c_dev i2c_dev2 = {
    .I2Cx         = I2C2,
    .gpio_port    = &gpiob,
    .sda_pin      = 11,
    .scl_pin      = 10,
#ifndef SOFT_I2C
    .clk       	  = RCC_APB1Periph_I2C2,
    .clkcmd       = RCC_APB1PeriphClockCmd,
    .clkrst       = RCC_APB1PeriphResetCmd,
    .ev_nvic_line = I2C2_EV_IRQn,
    .er_nvic_line = I2C2_ER_IRQn,
    .speed        = I2C_400KHz_SPEED,
    .i2cErrorCount= 0,
    .error        = 0
#endif
};
/** I2C2 device */
i2c_dev* const _I2C2 = &i2c_dev2;

#ifndef SOFT_I2C

static void i2c_er_handler(i2c_dev *dev);
static void i2c_ev_handler(i2c_dev *dev);

void I2C1_ER_IRQHandler(void)
{
    i2c_er_handler(&i2c_dev1);
}

void I2C1_EV_IRQHandler(void)
{
    i2c_ev_handler(&i2c_dev1);
}

void I2C2_ER_IRQHandler(void)
{
    i2c_er_handler(&i2c_dev2);
}

void I2C2_EV_IRQHandler(void)
{
    i2c_ev_handler(&i2c_dev2);
}

static void i2c_er_handler(i2c_dev *dev)
{
}

void i2c_ev_handler(i2c_dev *dev)
{
}

void i2cInit(i2c_dev *dev, uint32_t speed)
{
	dev->speed = speed;

	dev->clkcmd(dev->clk, ENABLE);

    if (dev == _I2C1) afio_remap(AFIO_MAPR_I2C1_REMAP);

    /* Enable the GPIOs for the SCL/SDA Pins */
	dev->gpio_port->clkcmd(dev->gpio_port->clk, ENABLE);

    // Init pins
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = BIT(dev->scl_pin) | BIT(dev->sda_pin);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);

    // Init I2C
    I2C_DeInit(dev->I2Cx);

    I2C_InitTypeDef I2C_InitStructure;
    I2C_StructInit(&I2C_InitStructure);

    I2C_ITConfig(dev->I2Cx, I2C_IT_EVT | I2C_IT_ERR, DISABLE);       //Enable EVT and ERR interrupts - they are enabled by the first request
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = speed;
    I2C_Init(dev->I2Cx, &I2C_InitStructure);

    I2C_Cmd(dev->I2Cx, ENABLE);
}

//*****************************************************************************
//
//*****************************************************************************
//
//! \internal
//! \brief Generate a start condition on I2C bus.
//!
//! \param ulBase specifies the I2C module base address.
//!
//! This function is to generate a start condition on
//! specified I2C BUS.
//!
//! The \e ulBase can be one of the following values:
//! \b I2C1_BASE, \b I2C2_BASE.
//!
//! \note This is only for master
//!
//! \return None.
//
//*****************************************************************************
static void I2CStartSend (i2c_dev *dev)
{
    //
    // Send start
    //
    dev->I2Cx->CR1 |= I2C_CR1_START;
}

//*****************************************************************************
//
//! \brief Generate a stop condition on I2C bus.
//!
//! \param ulBase specifies the I2C module base address.
//!
//! This function is to generate a stop condition on
//! specified I2C BUS.
//!
//! The \e ulBase can be one of the following values:
//! \b I2C2_BASE, \b I2C1_BASE.
//!
//! \note This is only for master
//!
//! \return None.
//
//*****************************************************************************
void
I2CStopSend (i2c_dev *dev)
{
    dev->I2Cx->CR1 |= I2C_CR1_STOP;
}

//*****************************************************************************
//
//! \internal
//! \brief Send a byte to I2C bus.
//!
//! \param ulBase specifies the I2C module base address.
//! \param ucData specifies the data which will send to I2C BUS.
//!
//! This function is to send a byte on specified I2C BUS.
//!
//! The \e ulBase can be one of the following values:
//! \b I2C2_BASE, \b I2C1_BASE.
//!
//! \note This is only for master
//!
//! \return None.
//
//*****************************************************************************
static void I2CByteSend (i2c_dev *dev, unsigned char ucData)
{
    //
    // Send i2c address and RW bit
    //
    dev->I2Cx->DR = ucData;
}

//*****************************************************************************
//
//! \internal
//! \brief Checks whether the last I2Cx Event is equal to the one passed
//! as parameter.
//!
//! \param ulBase specifies the I2C module base address.
//! \param ulEvent is the specifies the event to be checked.
//!
//! This function is to checks whether the last I2Cx Event is equal to the one
//! passed as parameter.
//!
//! The \e ulBase can be one of the following values:
//! \b I2C1_BASE, \b I2C2_BASE.
//!
//! \return An Error Status enumeration value:
//! xtrue :Last event is equal to the I2C_EVENT
//! xfalse :Last event is different from the I2C_EVENT
//
//*****************************************************************************
static unsigned char
I2CEventCheck (i2c_dev *dev, unsigned long ulEvent)
{
    unsigned long ulLastEvent;
    unsigned long ulSR1,ulSR2;

    //
    // get statues
    //
    ulSR1 = dev->I2Cx->SR1;
    ulSR2 = (dev->I2Cx->SR2 << 16);
    ulLastEvent = (ulSR1 | ulSR2) & 0x00FFFFFF;

    if((ulLastEvent & ulEvent) == ulEvent)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//*****************************************************************************
//
//! Gets the current I2C status.
//!
//! \param ulBase is the base address of the I2C Master module.
//!
//! This returns the status for the I2C module.
//!
//! \return The current status vaules.
//
//*****************************************************************************
unsigned long
I2CStatusGet(i2c_dev *dev)
{
    unsigned long ulStatus;
    unsigned long ulSR1,ulSR2;

    //
    // Return the status
    //
    ulSR1 = dev->I2Cx->SR1;
    ulSR2 = (dev->I2Cx->SR2 << 16);
    ulStatus = (ulSR1 | ulSR2) & 0x00FFFFFF;

    return ulStatus;
}

//! \brief Send a master transmit request when the bus is idle.(Write Step1)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucSlaveAddr is the 7-bit slave address.
//! \param ucData is the byte to transmit.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! This function init a new write transmition. When the master have not obtained
//! control of the bus, This function send request to transmit the START
//! condition, the slave address and the data, Then it returns immediately, no
//! waiting any bus transmition to complete.
//!
//! Users can call I2CMasterBusy() to check if all the bus transmition
//! complete, the call I2CMasterErr() to check if any error occurs.
//!
//! After the master obtained control of the bus, and haven't release it, users
//! can call I2CMasterWriteRequestS2() to continue transmit data to slave.
//! Users can also call I2CMasterStop() to terminate this transmition and
//! release the I2C bus.
//!
//! For this function returns immediately, it is always using in the interrupt
//! hander.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterWriteRequestS1(i2c_dev *dev, unsigned char ucSlaveAddr,
                        unsigned char ucData, unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    //
    // Waitting the I2C Control & I2C bus to idle
    //
    do
    {
        ulStatus = dev->I2Cx->SR2;
    }while((ulStatus & I2C_SR2_BUSY));

    //
    // Send start
    //
    I2CStartSend(dev);
    while(!I2CEventCheck(dev, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }

    //
    // Send address
    //
    I2CByteSend(dev, (ucSlaveAddr << 1) | 0);
    do
    {
        ulStatus = I2CStatusGet(dev);
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    //
    // Send data
    //
    I2CByteSend(dev, ucData);

    if(bEndTransmition)
    {
        do
        {
            ulStatus = I2CStatusGet(dev);
            if(dev->I2Cx->SR1 & 0x0F00)
                break;
        }
        while(!(ulStatus == I2C_EVENT_MASTER_BYTE_TRANSMITTED));
        I2CStopSend(dev);
    }
}

//*****************************************************************************
//
//! \brief Send a master data transmit request when the master have obtained
//! control of the bus.(Write Step2)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucData is the byte to transmit.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! After the master obtained control of the bus(have called
//! I2CMasterWriteRequestS1() without any error), and haven't release it, users
//! can call this function to continue transmit data to slave.
//!
//! This function just send request to transmit the data, and it returns
//! immediately, no waiting any bus transmition to complete.
//!
//! Users can call I2CMasterBusy() to check if all the bus transmition
//! complete, the call I2CMasterErr() to check if any error occurs. Users call
//! also can I2CMasterStop() to terminate this transmition and release the
//! I2C bus.
//!
//! For this function returns immediately, it is always using in the interrupt
//! hander.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterWriteRequestS2(i2c_dev *dev, unsigned char ucData,
                        unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    //
    // Write the byte.
    //
    dev->I2Cx->DR = ucData;

    //
    // Send the stop if End Transmition.
    //
    if(bEndTransmition)
    {
        do
        {
            ulStatus = I2CStatusGet(dev);
            if(dev->I2Cx->SR1 & 0x0F00)
                break;
        }
        while(!(ulStatus == I2C_EVENT_MASTER_BYTE_TRANSMITTED));
        I2CStopSend(dev);
    }
}

//*****************************************************************************
//
//! \brief Write a data to the slave when the bus is idle, and waiting for all
//! bus transmiton complete.(Write Step1)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucSlaveAddr is the 7-bit slave address.
//! \param ucData is the byte to transmit.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! This function init a new write transmition. When the master have not obtained
//! control of the bus, This function transmit the START condition, the slave
//! address and the data, then waiting for all bus transmition complete.
//!
//! Users can then check the return value to see if any error occurs:
//! - \ref I2C_MASTER_ERR_NONE     - \b 0, no error
//! - \ref I2C_MASTER_ERR_ADDR_ACK - The transmitted address was not acknowledged
//! - \ref I2C_MASTER_ERR_DATA_ACK - The transmitted data was not acknowledged
//! - \ref I2C_MASTER_ERR_ARB_LOST - The I2C controller lost arbitration.
//!
//! After the master obtained control of the bus, and haven't release it, users
//! can call I2CMasterWriteS2() to continue transmit data to slave.
//! Users call also can I2CMasterStop() to terminate this transmition and
//! release the I2C bus.
//!
//! This function is always used in thread mode.
//!
//! \return Returns the master error status.
//
//*****************************************************************************
unsigned long
I2CMasterWriteS1(i2c_dev *dev, unsigned char ucSlaveAddr,
                 unsigned char ucData, unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    //
    // Send write request
    //
    I2CMasterWriteRequestS1(dev, ucSlaveAddr, ucData, 0);

    //
    // Waiting the I2C controller to be transmited
    //
    do
    {
        ulStatus = I2CStatusGet(dev);
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    if(bEndTransmition)
    {
        I2CStopSend(dev);
    }

    //
    // return the error status
    //
    return (ulStatus & (I2C_SR1_ARLO | I2C_SR1_AF));

}

//*****************************************************************************
//
//! \brief Write a data to the slave, when the master have obtained control of
//! the bus, and waiting for all bus transmiton complete.(Write Step2)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucData is the byte to transmit.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! After the master obtained control of the bus(have called
//! I2CMasterWriteS1() without any error), and haven't release it, users
//! can call this function to continue transmit data to slave.
//!
//! This function transmit the data to the slave, and waiting for all bus
//! transmition complete.
//!
//! Users can then check the return value to see if any error occurs:
//! - \ref I2C_MASTER_ERR_NONE     - \b 0, no error
//! - \ref I2C_MASTER_ERR_ADDR_ACK - The transmitted address was not acknowledged
//! - \ref I2C_MASTER_ERR_DATA_ACK - The transmitted data was not acknowledged
//! - \ref I2C_MASTER_ERR_ARB_LOST - The I2C controller lost arbitration.
//!
//! Then users can call this function to continue transmit data to slave.
//! Users call also call I2CMasterStop() to terminate this transmition and
//! release the I2C bus.
//!
//! This function is always used in thread mode.
//!
//! \return Returns the master error status.
//
//*****************************************************************************
unsigned long
I2CMasterWriteS2(i2c_dev *dev, unsigned char ucData,
                 unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    //
    // Send write request
    //
    I2CMasterWriteRequestS2(dev, ucData, 0);

    //
    // Waiting the I2C controller to br transmited
    //
    do
    {
        ulStatus = I2CStatusGet(dev);
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    if(bEndTransmition)
    {
        I2CStopSend(dev);
    }

    //
    // return the error status
    //
    return (ulStatus & (I2C_SR1_ARLO | I2C_SR1_AF));

}

//*****************************************************************************
//
//! \brief Write a data buffer to the slave when the bus is idle, and waiting
//! for all bus transmiton complete.(Write Buffer Step1)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucSlaveAddr is the 7-bit slave address.
//! \param pucDataBuf is the data buffer to transmit.
//! \param ulLen is the data buffer byte size.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! This function init a new data buffer write transmition. When the master have
//! not obtained control of the bus, This function transmit the START condition,
//! the slave address and the data, then waiting for the data transmition
//! complete, and continue next data transmition, until all complete. If there
//! is any error occurs, the remain data will be canceled.
//!
//! Users can then check the return value to see how many datas have been
//! successfully transmited. if the number != ulLen, user can call
//! I2CMasterErr() to see what error occurs.
//!
//! After the master obtained control of the bus, and haven't release it, users
//! can call I2CMasterWriteS2() / I2CMasterWriteBufS2() to continue transmit data
//! to slave. Users call also call I2CMasterStop() to terminate this transmition
//! and release the I2C bus.
//!
//! This function is always used in thread mode.
//!
//! \return Returns the data number that have been successully tranmited.
//
//*****************************************************************************
unsigned long
I2CMasterWriteBufS1(i2c_dev *dev, unsigned char ucSlaveAddr,
                    const unsigned char *pucDataBuf, unsigned long ulLen,
                    unsigned char bEndTransmition)
{

    unsigned long ulStatus;
    unsigned long ulWritten;

    if(ulLen == 1)
    {
        ulStatus = I2CMasterWriteS1(dev, ucSlaveAddr,
                                    pucDataBuf[0], bEndTransmition);

        return (ulStatus == I2C_MASTER_ERR_NONE) ? 1 : 0;
    }

    //
    // Waitting the I2C Control & I2C bus to idle
    //
    do
    {
        ulStatus = dev->I2Cx->SR2;
    }while((ulStatus & I2C_SR2_BUSY));

    //
    // Send start
    //
    I2CStartSend(dev);
    while(!I2CEventCheck(dev, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(dev->I2Cx->SR1 & 0x0F00)
            return 0;
    }

    //
    // Send address
    //
    I2CByteSend(dev, (ucSlaveAddr << 1) | 0);
    while(!I2CEventCheck(dev, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(dev->I2Cx->SR1 & 0x0F00)
            return 0;
    }

    //
    // Send data
    //
    I2CByteSend(dev, pucDataBuf[0]);

    //
    // Check if any error occurs
    //
    if(ulStatus & (I2C_SR1_ARLO | I2C_SR1_AF | I2C_SR1_BERR))
    {
        return 0;
    }

    ulWritten = 1;

    ulWritten += I2CMasterWriteBufS2(dev,
                                     &pucDataBuf[1],
                                     ulLen - 1,
                                     bEndTransmition);

    return ulWritten;

}

//*****************************************************************************
//
//! \brief Write a data buffer to the slave, when the master have obtained
//! control of the bus, and waiting for all bus transmiton complete.(Write
//! Buffer Step2)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param pucDataBuf is the data buffer to transmit.
//! \param ulLen is the data buffer byte size.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! After the master obtained control of the bus(have called
//! I2CMasterWriteS1() or I2CMasterWriteBufS1() without any error), and haven't
//! release it, users can call this function to continue transmit data to slave.
//!
//! This function transmit the data one by one to the slave, waiting for every
//! data transmition complete, and continue next data transmition, until all
//! complete. If there is any error occurs, the remain data will be canceled.
//!
//! Users can then check the return value to see how many datas have been
//! successfully transmited. if the number != ulLen, user can call
//! I2CMasterErr() to see what error occurs.
//!
//! Then users can call I2CMasterWriteS2() or this function to continue
//! transmit data to slave. Users call also call I2CMasterStop() to terminate
//! this transmition and release the I2C bus.
//!
//! This function is always used in thread mode.
//!
//! \return Returns the data number that have been successully tranmited.
//
//*****************************************************************************
unsigned long
I2CMasterWriteBufS2(i2c_dev *dev, const unsigned char *pucDataBuf,
                    unsigned long ulLen, unsigned char bEndTransmition)
{
    unsigned long i;
    unsigned long ulStatus;

    for(i = 0; i < ulLen - 1; i++)
    {
        //
        // Write the first byte.
        //
        dev->I2Cx->DR = pucDataBuf[i];

        //
        // Waiting the I2C controller to idle
        //
        do
        {
            ulStatus = I2CStatusGet(dev);
            //
            // Check if any error occurs
            //
            if(dev->I2Cx->SR1 & 0x0F00)
                return i;
        }
        while(!(ulStatus == I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    }

    dev->I2Cx->DR = pucDataBuf[i];

    //
    // Waiting the I2C controller to idle
    //
    do
    {
        ulStatus = I2CStatusGet(dev);
        //
        // Check if any error occurs
        //
        if(dev->I2Cx->SR1 & 0x0F00)
            return ulLen - 1;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    if(bEndTransmition)
    {
        I2CStopSend(dev);
    }

    return ulLen;

}

//*****************************************************************************
//
//! \brief Send a master receive request when the bus is idle.(Read Step1)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucSlaveAddr is the 7-bit slave address.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! This function init a new receive transmition. When the master have not obtained
//! control of the bus, This function send request to transmit the START
//! condition, the slave address and the data request, Then it returns
//! immediately, no waiting any bus transmition to complete.
//!
//! If bEndTransmition is xtrue, the receive operation will followed by an
//! negative ACK and STOP condition.
//!
//! Users can call I2CMasterBusy() to check if all the bus transmition
//! complete, then call I2CMasterErr() to check if any error occurs. Then user
//! can get the data by calling I2CMasterDataGet() if there is no error occurs.
//!
//! After the master obtained control of the bus, and haven't release it, users
//! can call I2CMasterReadRequestS2() to continue receive data from slave.
//! Users call also can I2CMasterStop() to terminate this transmition and
//! release the I2C bus.
//!
//! For this function returns immediately, it is always using in the interrupt
//! hander.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterReadRequestS1(i2c_dev *dev, unsigned char ucSlaveAddr,
                       unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    dev->I2Cx->CR1 |= I2C_CR1_ACK;

    //
    // Send start
    //
    I2CStartSend(dev);
    while(!I2CEventCheck(dev, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }

    //
    // Send address
    //
    I2CByteSend(dev, (ucSlaveAddr << 1) | 1);
    while(!I2CEventCheck(dev, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }

    if(bEndTransmition)
    {
        dev->I2Cx->CR1 &= ~I2C_CR1_ACK;
        //
        // Waiting the I2C controller to be transmited
        //
        do
        {
            ulStatus = I2CStatusGet(dev);
            if(dev->I2Cx->SR1 & 0x0F00)
                break;
        }
        while(!(ulStatus == I2C_EVENT_MASTER_BYTE_RECEIVED));
        I2CStopSend(dev);
    }
}

//*****************************************************************************
//
//! \brief Send a master data receive request when the master have obtained
//! control of the bus.(Write Step2)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! After the master obtained control of the bus(have called
//! I2CMasterReadRequestS1() without any error), and haven't release it, users
//! can call this function to continue receive data from slave.
//!
//! If bEndTransmition is xtrue, the receive operation will followed by an
//! negative ACK and STOP condition.
//!
//! Users can call I2CMasterBusy() to check if all the bus transmition
//! complete, then call I2CMasterErr() to check if any error occurs. Then user
//! can get the data by calling I2CMasterDataGet() if there is no error occurs.
//!
//! Then users can call this function to continue receive data from slave.
//! Users call also can I2CMasterStop() to terminate this transmition and
//! release the I2C bus.
//!
//! For this function returns immediately, it is always using in the interrupt
//! hander.
//!
//! \return None.
//
//*****************************************************************************
void
I2CMasterReadRequestS2(i2c_dev *dev, unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    //
    // Send the stop if End Transmition.
    //
    if(bEndTransmition)
    {
    	dev->I2Cx->CR1 &= ~I2C_CR1_ACK;
        //
        // Waiting the I2C controller to be transmited
        //
        do
        {
            ulStatus = I2CStatusGet(dev);
            if(dev->I2Cx->SR1 & 0x0F00)
                break;
        }
        while(!(ulStatus == I2C_EVENT_MASTER_BYTE_RECEIVED));
        I2CStopSend(dev);
    }
}

//*****************************************************************************
//
//! \brief Read a data from a slave when the bus is idle, and waiting for all
//! bus transmiton complete.(Read Step1)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucSlaveAddr is the 7-bit slave address.
//! \param pucData is the buffer where to save the data.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! This function init a new receive transmition. When the master have not obtained
//! control of the bus, This function send request to transmit the START
//! condition, the slave address and the data request, then waiting for all bus
//! transmition complete.
//!
//! If bEndTransmition is xtrue, the receive operation will followed by an
//! negative ACK and STOP condition.
//!
//! Users can then check the return value to see if any error occurs:
//! - \ref I2C_MASTER_ERR_NONE     - \b 0, no error
//! - \ref I2C_MASTER_ERR_ADDR_ACK - The transmitted address was not acknowledged
//! - \ref I2C_MASTER_ERR_DATA_ACK - The transmitted data was not acknowledged
//! - \ref I2C_MASTER_ERR_ARB_LOST - The I2C controller lost arbitration.
//!
//! After the master obtained control of the bus, and haven't release it, users
//! can call I2CMasterReadS2() to continue receive data from slave.
//! Users call also can I2CMasterStop() to terminate this transmition and
//! release the I2C bus.
//!
//! This function is usually used in thread mode.
//!
//! \return Returns the master error status.
//
//*****************************************************************************
unsigned long
I2CMasterReadS1(i2c_dev *dev,
                unsigned char ucSlaveAddr,
                unsigned char *pucData,
                unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    I2CMasterReadRequestS1(dev, ucSlaveAddr, 0);

    if(bEndTransmition)
    {
        dev->I2Cx->CR1 &= ~I2C_CR1_ACK;
    }

    //
    // Waiting the I2C controller to be transmited
    //
    do
    {
        ulStatus = I2CStatusGet(dev);
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_BYTE_RECEIVED));


    //
    // Get the error code
    //
    ulStatus &= (I2C_SR1_ARLO | I2C_SR1_AF);

    if(!ulStatus)
    {
        *pucData = dev->I2Cx->DR;
    }

    if(bEndTransmition)
    {
        I2CStopSend(dev);
    }

    //
    // return the error status
    //
    return ulStatus;

}

//*****************************************************************************
//
//! \brief Read a data from a slave when the master have obtained control of
//! the bus, and waiting for all bus transmiton complete.(Read Step2)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param pucData is the buffer where to save the data.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! After the master obtained control of the bus(have called
//! I2CMasterReadS1() without any error), and haven't release it, users can
//! call this function to continue receive data from the slave.
//!
//! If bEndTransmition is xtrue, the receive operation will followed by an
//! negative ACK and STOP condition.
//!
//! It will be waiting for all bus transmition complete before return.
//! Users can then check the return value to see if any error occurs:
//! - \ref I2C_MASTER_ERR_NONE     - \b 0, no error
//! - \ref I2C_MASTER_ERR_ADDR_ACK - The transmitted address was not acknowledged
//! - \ref I2C_MASTER_ERR_DATA_ACK - The transmitted data was not acknowledged
//! - \ref I2C_MASTER_ERR_ARB_LOST - The I2C controller lost arbitration.
//!
//! Then useres can call this function to continue receive data from slave.
//! Users call also can I2CMasterStop() to terminate this transmition and
//! release the I2C bus.
//!
//! This function is usually used in thread mode.
//!
//! \return Returns the master error status.
//
//*****************************************************************************
unsigned long
I2CMasterReadS2(i2c_dev *dev,
                unsigned char *pucData,
                unsigned char bEndTransmition)
{
    unsigned long ulStatus;

    I2CMasterReadRequestS2(dev, 0);

    if(bEndTransmition)
    {
        dev->I2Cx->CR1 &= ~I2C_CR1_ACK;
    }

    //
    // Waiting the I2C controller to be transmited
    //
    do
    {
        ulStatus = I2CStatusGet(dev);
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_BYTE_RECEIVED));

    //
    // Get the error code
    //
    ulStatus &= (I2C_SR1_ARLO | I2C_SR1_AF);

    if(!ulStatus)
    {
        *pucData = dev->I2Cx->DR;
    }

    if(bEndTransmition)
    {
        I2CStopSend(dev);
    }

    //
    // return the error status
    //
    return ulStatus;
}

//*****************************************************************************
//
//! \brief Read some data from a slave when the bus is idle, and waiting for all
//! bus transmiton complete.(Read Buffer Step1)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucSlaveAddr is the 7-bit slave address.
//! \param pucDataBuf is the buffer where to save the data.
//! \param ulLen is the data number to receive.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! This function init a new data buffer receive transmition. When the master
//! have not obtained control of the bus, This function send request to transmit
//! the START condition, the slave address and the data request, then waiting for
//! the data transmition complete, and continue next data transmition, until all
//! complete. If there is any error occurs, the remain data will be canceled.
//!
//! If bEndTransmition is xtrue, the receive operation will followed by an
//! negative ACK and STOP condition.
//!
//! Users can then check the return value to see how many datas have been
//! successfully received. if the number != ulLen, user can call
//! I2CMasterErr() to see what error occurs.
//!
//! After the master obtained control of the bus, and haven't release it, users
//! can call I2CMasterReadS2() or I2CMasterReadBufS2() to continue receive data .
//! from slave .Users call also can I2CMasterStop() to terminate this transmition
//! and release the I2C bus.
//!
//! This function is usually used in thread mode.
//!
//! \return Returns the data number that have been successully received.
//
//*****************************************************************************
unsigned long
I2CMasterReadBufS1(i2c_dev *dev, unsigned char ucSlaveAddr,
                   unsigned char* pucDataBuf, unsigned long ulLen,
                   unsigned char bEndTransmition)
{
    unsigned long ulStatus;
    unsigned long ulRead;

    if(ulLen == 0)
    {
        return 0;
    }

    else if(ulLen == 1)
    {
        ulStatus = I2CMasterReadS1(dev, ucSlaveAddr,
                                   pucDataBuf, bEndTransmition);

        if (ulStatus == I2C_MASTER_ERR_NONE)
        {
            pucDataBuf[0] = dev->I2Cx->DR;

            return 1;
        }
        else
        {
            return 0;
        }
    }

    //
    // Waitting the I2C Control & I2C bus to idle
    //
    do
    {
        ulStatus = dev->I2Cx->SR2;
    }while((ulStatus & I2C_SR2_BUSY));

    dev->I2Cx->CR1 |= I2C_CR1_ACK;

    //
    // Send start
    //
    I2CStartSend(dev);
    while(!I2CEventCheck(dev, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(dev->I2Cx->SR1 & 0x0F00)
            return 0;
    }

    //
    // Send address
    //
    I2CByteSend(dev, (ucSlaveAddr << 1) | 1);
    while(!I2CEventCheck(dev, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(dev->I2Cx->SR1 & 0x0F00)
            return 0;
    }

    //
    // Waiting the I2C controller to be transmited
    //
    do
    {
        ulStatus = I2CStatusGet(dev);
        if(dev->I2Cx->SR1 & 0x0F00)
            break;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_BYTE_RECEIVED));

    //
    // Check if any error occurs
    //
    if(ulStatus & (I2C_SR1_ARLO | I2C_SR1_AF | I2C_SR1_BERR))
    {
        return 0;
    }

    pucDataBuf[0] = dev->I2Cx->DR;

    ulRead = 1;

    ulRead += I2CMasterReadBufS2(dev,
                                 &pucDataBuf[1],
                                 ulLen - 1,
                                 bEndTransmition);


    return ulRead;

}

//*****************************************************************************
//
//! \brief Read some data from a slave when the master have obtained control of
//! the bus, and waiting for all bus transmiton complete.(Write Buffer Step2)
//!
//! \param ulBase is the base address of the I2C Master module.
//! \param ucSlaveAddr is the 7-bit slave address.
//! \param pucDataBuf is the buffer where to save the data.
//! \param ulLen is the data number to receive.
//! \param bEndTransmition is flag to control if transmit the STOP condition and
//! terminate this transmition.
//!
//! After the master obtained control of the bus(have called
//! I2CMasterReadS1() or I2CMasterReadBufS1() without any error), and haven't
//! release it, users can call this function to continue receive data from slave.
//!
//! This function receive data one by one from the slave, waiting for every
//! data transmition complete, and continue next data transmition, until all
//! complete. If there is any error occurs, the remain data will be canceled.
//!
//! If bEndTransmition is xtrue, the receive operation will followed by an
//! negative ACK and STOP condition.
//!
//! Users can then check the return value to see how many datas have been
//! successfully received. if the number != ulLen, user can call
//! I2CMasterErr() to see what error occurs.
//!
//! After the master obtained control of the bus, and haven't release it, users
//! can call I2CMasterReadS2() or I2CMasterReadBufS2() to continue receive data
//! from slave. Users call also can I2CMasterStop() to terminate this transmition
//! and release the I2C bus.
//!
//! This function is usually used in thread mode.
//!
//! \return Returns the data number that have been successully received.
//
//*****************************************************************************
unsigned long
I2CMasterReadBufS2(i2c_dev *dev, unsigned char *pucDataBuf,
                   unsigned long ulLen, unsigned char bEndTransmition)
{
    unsigned long i;
    unsigned long ulStatus;

    if(ulLen == 0)
    {
        return 0;
    }


    for(i = 0; i < ulLen - 1; i++)
    {
        //
        // Waiting the I2C controller to idle
        //
        do
        {
            ulStatus = I2CStatusGet(dev);
            //
            // Check if any error occurs
            //
            if(dev->I2Cx->SR1 & 0x0F00)
                return i;
        }
        while(!(ulStatus == I2C_EVENT_MASTER_BYTE_RECEIVED));

        pucDataBuf[i] = dev->I2Cx->DR;
    }

    if(bEndTransmition)
    {
        dev->I2Cx->CR1 &= ~I2C_CR1_ACK;
    }

    //
    // Waiting the I2C controller to idle
    //
    do
    {
        ulStatus = I2CStatusGet(dev);
        //
        // Check if any error occurs
        //
        if(dev->I2Cx->SR1 & 0x0F00)
            return ulLen - 1;
    }
    while(!(ulStatus == I2C_EVENT_MASTER_BYTE_RECEIVED));

    pucDataBuf[i] = dev->I2Cx->DR;

    if(bEndTransmition)
    {
        I2CStopSend(dev);
    }

    return ulLen;

}

#else

static void I2C_delay(void)
{
	//TEO 20130822 : provo a "rallentare" la i2C per i 32MHz
    volatile int i = 28;
//    volatile int i = 7;  //pare ok a 8MHz
//	volatile int i = 1;
    while (i)
        i--;
}

static bool I2C_Start(i2c_dev *dev)
{
	dev->gpio_port->GPIOx->BSRR = BIT(dev->sda_pin);
	dev->gpio_port->GPIOx->BSRR = BIT(dev->scl_pin);
    I2C_delay();
    if (!(dev->gpio_port->GPIOx->IDR & BIT(dev->sda_pin)))
        return 0;
    dev->gpio_port->GPIOx->BRR = BIT(dev->sda_pin);
    I2C_delay();
    if (dev->gpio_port->GPIOx->IDR & BIT(dev->sda_pin))
        return 0;
    dev->gpio_port->GPIOx->BRR = BIT(dev->sda_pin);
    I2C_delay();
    return 1;
}

static void I2C_Stop(i2c_dev *dev)
{
    dev->gpio_port->GPIOx->BRR = BIT(dev->scl_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BRR = BIT(dev->sda_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BSRR = BIT(dev->scl_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BSRR = BIT(dev->sda_pin);
    I2C_delay();
}

static void I2C_Ack(i2c_dev *dev)
{
	dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BRR  = BIT(dev->sda_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BSRR = BIT(dev->scl_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
    I2C_delay();
}

static void I2C_NoAck(i2c_dev *dev)
{
	dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BSRR = BIT(dev->sda_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BSRR = BIT(dev->scl_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
    I2C_delay();
}

static bool I2C_WaitAck(i2c_dev *dev)
{
	dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BSRR = BIT(dev->sda_pin);
    I2C_delay();
    dev->gpio_port->GPIOx->BSRR = BIT(dev->scl_pin);
    I2C_delay();
    if (dev->gpio_port->GPIOx->IDR & BIT(dev->sda_pin)) {
    	dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
        return 0;
    }
    dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
    return 1;
}

static void I2C_SendByte(i2c_dev *dev, uint8_t byte)
{
    uint8_t i = 8;
    while (i--) {
    	dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
        I2C_delay();
        if (byte & 0x80)
        	dev->gpio_port->GPIOx->BSRR = BIT(dev->sda_pin);
        else
            dev->gpio_port->GPIOx->BRR  = BIT(dev->sda_pin);
        byte <<= 1;
        I2C_delay();
        dev->gpio_port->GPIOx->BSRR = BIT(dev->scl_pin);
        I2C_delay();
    }
    dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
}

static uint8_t I2C_ReceiveByte(i2c_dev *dev)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    dev->gpio_port->GPIOx->BSRR = BIT(dev->sda_pin);
    while (i--) {
        byte <<= 1;
        dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
        I2C_delay();
        dev->gpio_port->GPIOx->BSRR = BIT(dev->scl_pin);
        I2C_delay();
        if (dev->gpio_port->GPIOx->IDR & BIT(dev->sda_pin)) {
            byte |= 0x01;
        }
    }
    dev->gpio_port->GPIOx->BRR  = BIT(dev->scl_pin);
    return byte;
}

void i2cInit(i2c_dev *dev, uint32_t speed)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = BIT(dev->scl_pin) | BIT(dev->sda_pin);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);
}

uint8_t i2cWriteBuffer(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
    int i;
    if (!I2C_Start(dev))
        return I2C_ERROR;
    I2C_SendByte(dev, addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck(dev)) {
        I2C_Stop(dev);
        return I2C_ERROR;
    }
    I2C_SendByte(dev, reg);
    I2C_WaitAck(dev);
    for (i = 0; i < len; i++) {
        I2C_SendByte(dev, data[i]);
        if (!I2C_WaitAck(dev)) {
            I2C_Stop(dev);
            return I2C_ERROR;
        }
    }
    I2C_Stop(dev);
    return I2C_OK;
}

uint8_t i2cWrite(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t data)
{
    if (!I2C_Start(dev))
        return I2C_ERROR;
    I2C_SendByte(dev, addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck(dev)) {
        I2C_Stop(dev);
        return I2C_ERROR;
    }
    I2C_SendByte(dev, reg);
    I2C_WaitAck(dev);
    I2C_SendByte(dev, data);
    I2C_WaitAck(dev);
    I2C_Stop(dev);
    return I2C_OK;
}

uint8_t i2cRead(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!I2C_Start(dev))
        return I2C_ERROR;
    I2C_SendByte(dev, addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck(dev)) {
        I2C_Stop(dev);
        return I2C_ERROR;
    }
    I2C_SendByte(dev, reg);
    I2C_WaitAck(dev);
    I2C_Start(dev);
    I2C_SendByte(dev, addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck(dev);
    while (len) {
        *buf = I2C_ReceiveByte(dev);
        if (len == 1)
            I2C_NoAck(dev);
        else
            I2C_Ack(dev);
        buf++;
        len--;
    }
    I2C_Stop(dev);
    return I2C_OK;
}

uint8_t i2cWriteReg(i2c_dev *dev, uint8_t addr, uint8_t* reg, uint8_t lenreg, uint8_t* buf, uint8_t lenbuf)
{
    int i;

    if (!I2C_Start(dev))
        return I2C_ERROR;

    I2C_SendByte(dev, addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck(dev)) {
        I2C_Stop(dev);
        return I2C_ERROR;
    }

    for (i = 0; i < lenreg; i++) {
        I2C_SendByte(dev, reg[i]);
        if (!I2C_WaitAck(dev)) {
            I2C_Stop(dev);
            return I2C_ERROR;
        }
    }

    for (i = 0; i < lenbuf; i++) {
        I2C_SendByte(dev, buf[i]);
        if (!I2C_WaitAck(dev)) {
            I2C_Stop(dev);
            return I2C_ERROR;
        }
    }

    I2C_Stop(dev);

    return I2C_OK;
}

uint8_t i2cReadReg(i2c_dev *dev, uint8_t addr, uint8_t* reg, uint8_t lenreg, uint8_t* buf, uint8_t lenbuf)
{
    int i;

    if (!I2C_Start(dev))
        return I2C_ERROR;

    I2C_SendByte(dev, addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck(dev)) {
        I2C_Stop(dev);
        return I2C_ERROR;
    }

    for (i = 0; i < lenreg; i++) {
        I2C_SendByte(dev, reg[i]);
        if (!I2C_WaitAck(dev)) {
            I2C_Stop(dev);
            return I2C_ERROR;
        }
    }

    I2C_Start(dev);
    I2C_SendByte(dev, addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck(dev);
    while (lenbuf) {
        *buf = I2C_ReceiveByte(dev);
        if (lenbuf == 1)
            I2C_NoAck(dev);
        else
            I2C_Ack(dev);
        buf++;
        lenbuf--;
    }
    I2C_Stop(dev);
    return I2C_OK;
}

uint16_t i2cGetErrorCounter(i2c_dev *dev)
{
    // TODO maybe fix this, but since this is test code, doesn't matter.
    return 0;
}

#endif
