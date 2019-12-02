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
static void i2cUnstick(i2c_dev *dev);

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


#define I2C_DEFAULT_TIMEOUT 30000

static void i2c_er_handler(i2c_dev *dev)
{
    volatile uint32_t SR1Register;
    volatile uint32_t SR2Register;
    /* Read the I2C1 status register */
    SR1Register = dev->I2Cx->SR1;
    if (SR1Register & 0x0F00) { //an error
        dev->error = 1;
        // I2C1error.error = ((SR1Register & 0x0F00) >> 8);        //save error
        // I2C1error.job = job;    //the task
    }
    /* If AF, BERR or ARLO, abandon the current job and commence new if there are jobs */
    if (SR1Register & 0x0700) {
        SR2Register = dev->I2Cx->SR2;        //read second status register to clear ADDR if it is set (note that BTF will not be set after a NACK)
        I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, DISABLE);        //disable the RXNE/TXE interrupt - prevent the ISR tailchaining onto the ER (hopefully)
        if (!(SR1Register & 0x0200) && !(dev->I2Cx->CR1 & 0x0200)) {  //if we dont have an ARLO error, ensure sending of a stop
            if (dev->I2Cx->CR1 & 0x0100) {   //We are currently trying to send a start, this is very bad as start,stop will hang the peripheral
                while (dev->I2Cx->CR1 & 0x0100);     //wait for any start to finish sending
                I2C_GenerateSTOP(dev->I2Cx, ENABLE); //send stop to finalise bus transaction
                while (dev->I2Cx->CR1 & 0x0200);     //wait for stop to finish sending
                i2cInit(dev, dev->speed);   //reset and configure the hardware
            } else {
                I2C_GenerateSTOP(dev->I2Cx, ENABLE); //stop to free up the bus
                I2C_ITConfig(dev->I2Cx, I2C_IT_EVT | I2C_IT_ERR, DISABLE);   //Disable EVT and ERR interrupts while bus inactive
            }
        }
    }
    dev->I2Cx->SR1 &= ~0x0F00;       //reset all the error bits to clear the interrupt
    dev->busy = 0;
}

uint8_t i2cWriteBuffer(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    uint8_t i;
    uint8_t my_data[16];
    uint32_t timeout = I2C_DEFAULT_TIMEOUT;

    dev->addr = addr << 1;
    dev->reg = reg;
    dev->writing = 1;
    dev->reading = 0;
    dev->write_p = my_data;
    dev->read_p = my_data;
    dev->bytes = len;
    dev->busy = 1;
    dev->error = 0;

    // too long
    if (len > 16)
        return I2C_ERROR;

    for (i = 0; i < len; i++)
        my_data[i] = data[i];

    if (!(dev->I2Cx->CR2 & I2C_IT_EVT)) {        //if we are restarting the driver
        if (!(dev->I2Cx->CR1 & 0x0100)) {        // ensure sending a start
            while (dev->I2Cx->CR1 & 0x0200) { ; }               //wait for any stop to finish sending
            I2C_GenerateSTART(dev->I2Cx, ENABLE);        //send the start for the new job
        }
        I2C_ITConfig(dev->I2Cx, I2C_IT_EVT | I2C_IT_ERR, ENABLE);        //allow the interrupts to fire off again
    }

    while (dev->busy && --timeout > 0);
    if (timeout == 0) {
    	dev->i2cErrorCount++;
        // reinit peripheral + clock out garbage
        i2cInit(dev, dev->speed);
        return I2C_ERROR;
    }

    return dev->error;
}

uint8_t i2cWrite(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t data)
{
    return i2cWriteBuffer(dev, addr, reg, 1, &data);
}

uint8_t i2cRead(i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t* buf)
{
    uint32_t timeout = I2C_DEFAULT_TIMEOUT;

    dev->addr = addr << 1;
    dev->reg = reg;
    dev->writing = 0;
    dev->reading = 1;
    dev->read_p = buf;
    dev->write_p = buf;
    dev->bytes = len;
    dev->busy = 1;
    dev->error = 0;

    if (!(dev->I2Cx->CR2 & I2C_IT_EVT)) {        //if we are restarting the driver
        if (!(dev->I2Cx->CR1 & 0x0100)) {        // ensure sending a start
            while (dev->I2Cx->CR1 & 0x0200) { ; }               //wait for any stop to finish sending
            I2C_GenerateSTART(dev->I2Cx, ENABLE);        //send the start for the new job
        }
        I2C_ITConfig(dev->I2Cx, I2C_IT_EVT | I2C_IT_ERR, ENABLE);        //allow the interrupts to fire off again
    }

    while (dev->busy && --timeout > 0);
    if (timeout == 0) {
    	dev->i2cErrorCount++;
        // reinit peripheral + clock out garbage
        i2cInit(dev, dev->speed);
        return I2C_ERROR;
    }

    return dev->error;
}

void i2c_ev_handler(i2c_dev *dev)
{
    static uint8_t subaddress_sent, final_stop; //flag to indicate if subaddess sent, flag to indicate final bus condition
    static int8_t index;        //index is signed -1==send the subaddress
    uint8_t SReg_1 = dev->I2Cx->SR1; //read the status register here

    if (SReg_1 & 0x0001) {      //we just sent a start - EV5 in ref manual
        dev->I2Cx->CR1 &= ~0x0800;  //reset the POS bit so ACK/NACK applied to the current byte
        I2C_AcknowledgeConfig(dev->I2Cx, ENABLE);    //make sure ACK is on
        index = 0;              //reset the index
        if (dev->reading && (subaddress_sent || 0xFF == dev->reg)) {       //we have sent the subaddr
            subaddress_sent = 1;        //make sure this is set in case of no subaddress, so following code runs correctly
            if (dev->bytes == 2)
                dev->I2Cx->CR1 |= 0x0800;    //set the POS bit so NACK applied to the final byte in the two byte read
            I2C_Send7bitAddress(dev->I2Cx, dev->addr, I2C_Direction_Receiver);   //send the address and set hardware mode
        } else {                //direction is Tx, or we havent sent the sub and rep start
            I2C_Send7bitAddress(dev->I2Cx, dev->addr, I2C_Direction_Transmitter);        //send the address and set hardware mode
            if (dev->reg != 0xFF)       //0xFF as subaddress means it will be ignored, in Tx or Rx mode
                index = -1;     //send a subaddress
        }
    } else if (SReg_1 & 0x0002) {       //we just sent the address - EV6 in ref manual
        //Read SR1,2 to clear ADDR
        volatile uint8_t a;
        __DMB(); // memory fence to control hardware
        if (dev->bytes == 1 && dev->reading && subaddress_sent) { //we are receiving 1 byte - EV6_3
            I2C_AcknowledgeConfig(dev->I2Cx, DISABLE);       //turn off ACK
            __DMB();
            a = dev->I2Cx->SR2;      //clear ADDR after ACK is turned off
            I2C_GenerateSTOP(dev->I2Cx, ENABLE);     //program the stop
            final_stop = 1;
            I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, ENABLE);     //allow us to have an EV7
        } else {                //EV6 and EV6_1
            a = dev->I2Cx->SR2;      //clear the ADDR here
            __DMB();
            if (dev->bytes == 2 && dev->reading && subaddress_sent) {     //rx 2 bytes - EV6_1
                I2C_AcknowledgeConfig(dev->I2Cx, DISABLE);   //turn off ACK
                I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, DISABLE);        //disable TXE to allow the buffer to fill
            } else if (dev->bytes == 3 && dev->reading && subaddress_sent)       //rx 3 bytes
                I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, DISABLE);        //make sure RXNE disabled so we get a BTF in two bytes time
            else                //receiving greater than three bytes, sending subaddress, or transmitting
                I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, ENABLE);
        }
    } else if (SReg_1 & 0x004) {        //Byte transfer finished - EV7_2, EV7_3 or EV8_2
        final_stop = 1;
        if (dev->reading && subaddress_sent) {     //EV7_2, EV7_3
            if (dev->bytes > 2) {      //EV7_2
                I2C_AcknowledgeConfig(dev->I2Cx, DISABLE);   //turn off ACK
                dev->read_p[index++] = I2C_ReceiveData(dev->I2Cx);    //read data N-2
                I2C_GenerateSTOP(dev->I2Cx, ENABLE); //program the Stop
                final_stop = 1; //reuired to fix hardware
                dev->read_p[index++] = I2C_ReceiveData(dev->I2Cx);    //read data N-1
                I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, ENABLE); //enable TXE to allow the final EV7
            } else {            //EV7_3
                if (final_stop)
                    I2C_GenerateSTOP(dev->I2Cx, ENABLE);     //program the Stop
                else
                    I2C_GenerateSTART(dev->I2Cx, ENABLE);    //program a rep start
                dev->read_p[index++] = I2C_ReceiveData(dev->I2Cx);    //read data N-1
                dev->read_p[index++] = I2C_ReceiveData(dev->I2Cx);    //read data N
                index++;        //to show job completed
            }
        } else {                //EV8_2, which may be due to a subaddress sent or a write completion
            if (subaddress_sent || (dev->writing)) {
                if (final_stop)
                    I2C_GenerateSTOP(dev->I2Cx, ENABLE);     //program the Stop
                else
                    I2C_GenerateSTART(dev->I2Cx, ENABLE);    //program a rep start
                index++;        //to show that the job is complete
            } else {            //We need to send a subaddress
                I2C_GenerateSTART(dev->I2Cx, ENABLE);        //program the repeated Start
                subaddress_sent = 1;    //this is set back to zero upon completion of the current task
            }
        }
        //we must wait for the start to clear, otherwise we get constant BTF
        while (dev->I2Cx->CR1 & 0x0100) { ; }
    } else if (SReg_1 & 0x0040) {       //Byte received - EV7
    	dev->read_p[index++] = I2C_ReceiveData(dev->I2Cx);
        if (dev->bytes == (index + 3))
            I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, DISABLE);    //disable TXE to allow the buffer to flush so we can get an EV7_2
        if (dev->bytes == index)       //We have completed a final EV7
            index++;            //to show job is complete
    } else if (SReg_1 & 0x0080) {       //Byte transmitted -EV8/EV8_1
        if (index != -1) {      //we dont have a subaddress to send
            I2C_SendData(dev->I2Cx, dev->write_p[index++]);
            if (dev->bytes == index)   //we have sent all the data
                I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, DISABLE);        //disable TXE to allow the buffer to flush
        } else {
            index++;
            I2C_SendData(dev->I2Cx, dev->reg);       //send the subaddress
            if (dev->reading || !dev->bytes)      //if receiving or sending 0 bytes, flush now
                I2C_ITConfig(dev->I2Cx, I2C_IT_BUF, DISABLE);        //disable TXE to allow the buffer to flush
        }
    }
    if (index == dev->bytes + 1) {   //we have completed the current job
        //Completion Tasks go here
        //End of completion tasks
        subaddress_sent = 0;    //reset this here
        // I2Cx->CR1 &= ~0x0800;   //reset the POS bit so NACK applied to the current byte
        if (final_stop)  //If there is a final stop and no more jobs, bus is inactive, disable interrupts to prevent BTF
            I2C_ITConfig(dev->I2Cx, I2C_IT_EVT | I2C_IT_ERR, DISABLE);       //Disable EVT and ERR interrupts while bus inactive
        dev->busy = 0;
    }
}

void i2cInit(i2c_dev *dev, uint32_t speed)
{
	dev->speed = speed;

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // Init pins
    GPIO_InitStructure.GPIO_Pin = BIT(dev->scl_pin) | BIT(dev->sda_pin);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);

    // clock out stuff to make sure slaves arent stuck
    i2cUnstick(dev);

    // Init I2C
    I2C_DeInit(dev->I2Cx);
    I2C_StructInit(&I2C_InitStructure);

    I2C_ITConfig(dev->I2Cx, I2C_IT_EVT | I2C_IT_ERR, DISABLE);       //Enable EVT and ERR interrupts - they are enabled by the first request
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = speed;
    I2C_Cmd(dev->I2Cx, ENABLE);
    I2C_Init(dev->I2Cx, &I2C_InitStructure);

    NVIC_PriorityGroupConfig(0x500);

    // I2C ER Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // I2C EV Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

}

uint16_t i2cGetErrorCounter(i2c_dev *dev)
{
    return dev->i2cErrorCount;
}

static void i2cUnstick(i2c_dev *dev)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint8_t i;

    GPIO_InitStructure.GPIO_Pin = BIT(dev->scl_pin) | BIT(dev->sda_pin);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);

    GPIO_SetBits(dev->gpio_port->GPIOx, BIT(dev->scl_pin) | BIT(dev->sda_pin));
    for (i = 0; i < 8; i++) {
        // Wait for any clock stretching to finish
        while (!GPIO_ReadInputDataBit(dev->gpio_port->GPIOx, BIT(dev->scl_pin)))
            delay_us(10);

        // Pull low
        GPIO_ResetBits(dev->gpio_port->GPIOx, BIT(dev->scl_pin)); //Set bus low
        delay_us(10);
        // Release high again
        GPIO_SetBits(dev->gpio_port->GPIOx, BIT(dev->scl_pin)); //Set bus high
        delay_us(10);
    }

    // Generate a start then stop condition
    GPIO_ResetBits(dev->gpio_port->GPIOx, BIT(dev->sda_pin)); // Set bus data low
    delay_us(10);
    GPIO_ResetBits(dev->gpio_port->GPIOx, BIT(dev->scl_pin)); // Set bus scl low
    delay_us(10);
    GPIO_SetBits(dev->gpio_port->GPIOx, BIT(dev->scl_pin)); // Set bus scl high
    delay_us(10);
    GPIO_SetBits(dev->gpio_port->GPIOx, BIT(dev->sda_pin)); // Set bus sda high

    // Init pins
    GPIO_InitStructure.GPIO_Pin = BIT(dev->scl_pin) | BIT(dev->sda_pin);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(dev->gpio_port->GPIOx, &GPIO_InitStructure);
}

#else

static void I2C_delay(void)
{
	//TEO 20130822 : provo a "rallentare" la i2C per i 32MHz
//    volatile int i = 28;
//    volatile int i = 7;  //pare ok a 8MHz
	volatile int i = 1;
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
