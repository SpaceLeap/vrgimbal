/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

/**
 * @file FastSerial.cpp
 *
 * @brief Wiring-like serial api
 */

#include "FastSerial.h"
#include "boards.h"
#include "wirish_types.h"

FastSerial::FastSerial()
{
}

FastSerial::FastSerial(usart_dev *usart_device,
                       uint8 tx_pin,
                       uint8 rx_pin) {
    this->usart_device = usart_device;
    this->tx_pin = tx_pin;
    this->rx_pin = rx_pin;
    this->begin(57600);
}

void FastSerial::init(usart_dev *usart_device,
                       uint8 tx_pin,
                       uint8 rx_pin)
{
	this->usart_device = usart_device;
	this->tx_pin = tx_pin;
	this->rx_pin = rx_pin;
}

void FastSerial::configure(uint8 port)
{
	if (port == 0)
	{
		this->init(FSUSART0, FSTXPIN0, FSRXPIN0);
	}

#if BOARD_NR_USARTS > 1
	else if (port == 1)
	{
		this->init(FSUSART1, FSTXPIN1, FSRXPIN1);
	}
#endif

#if BOARD_NR_USARTS > 2
	else if (port == 2)
	{
		this->init(FSUSART2, FSTXPIN2, FSRXPIN2);
	}
#endif

#if defined(STM32_HIGH_DENSITY)

#if BOARD_NR_USARTS > 3
	else if (port == 3)
	{
		this->init(FSUSART3, FSTXPIN3, FSRXPIN3);
	}
#endif

#if BOARD_NR_USARTS > 4
	else if (port == 4)
	{
		this->init(FSUSART4, FSTXPIN4, FSRXPIN4);
	}
#endif
#endif
}

void FastSerial::begin(long baud) {
	begin(baud, DEFAULT_TX_TIMEOUT);
}

void FastSerial::begin(long baud, uint32_t tx_timeout) {

    if ( (uint32)baud > this->usart_device->max_baud) {
        return;
    }

    const stm32_pin_info *txi = &PIN_MAP[tx_pin];
    const stm32_pin_info *rxi = &PIN_MAP[rx_pin];

    gpio_set_mode(txi->gpio_device, txi->gpio_bit, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(rxi->gpio_device, rxi->gpio_bit, GPIO_INPUT_FLOATING);

    usart_init(usart_device);
    usart_setup(usart_device, baud, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Rx | USART_Mode_Tx, USART_HardwareFlowControl_None);
    usart_enable(usart_device); 
}

void FastSerial::end(void) {
    usart_disable(this->usart_device);
}

int FastSerial::available(void) {
    return usart_data_available(this->usart_device);
}

int FastSerial::txspace(void)
{
    return txfifo_freebytes();

}

void FastSerial::use_tx_fifo(bool enable)
{
	usart_reset_tx(this->usart_device);
	usart_use_tx_fifo(this->usart_device, (enable ? 1 : 0));
}
void FastSerial::set_blocking_writes(bool enable)
{
	usart_reset_tx(this->usart_device);
	usart_use_tx_fifo(this->usart_device, (enable ? 0 : 1));
}

uint8_t FastSerial::get_blocking_writes(void)
{
	uint8_t blocking;
	blocking = (this->usart_device->usetxrb ? 0 : 1);
	return blocking;
}

void FastSerial::use_timeout(uint8_t enable)
{
	usart_use_timeout(this->usart_device, enable);
}

void FastSerial::set_timeout(uint32_t timeout)
{
	usart_set_timeout(this->usart_device, timeout);
}
int FastSerial::read(void) {
	if (available() <= 0)
		return (-1);
    return usart_getc(this->usart_device);
}

int FastSerial::peek(void)
{
	if (available() <= 0)
		return (-1);

	// pull character from tail
	return usart_getc(this->usart_device);
}

// return time elasped from last uart interrupt
// that functionality is good for sincronize the use of serial resource and buffer
long FastSerial::getPortLic(void){
long ret=0;
if (this->usart_device->USARTx == USART1) ret =  uart1_lic_millis;
if (this->usart_device->USARTx == USART2) ret =  uart2_lic_millis;
if (this->usart_device->USARTx == USART3) ret =  uart3_lic_millis;
if (this->usart_device->USARTx == UART4)  ret =  uart4_lic_millis;
if (this->usart_device->USARTx == UART5)  ret =  uart5_lic_millis;

return(ret);
}

void FastSerial::flush(void) {
    usart_reset_rx(this->usart_device);
    usart_reset_tx(this->usart_device);
}
uint32_t FastSerial::txfifo_nbytes(void)
{
	return usart_txfifo_nbytes(this->usart_device);
}
uint32_t FastSerial::txfifo_freebytes(void)
{
	return usart_txfifo_freebytes(this->usart_device);
}

void FastSerial::write(uint8_t ch) {
    usart_putc(this->usart_device, ch);
}

void FastSerial::begin(long baud, unsigned int rxSpace, unsigned int txSpace)
{
	begin(baud);
}
