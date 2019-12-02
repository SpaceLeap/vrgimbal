/*
 * DmaChannel.cpp
 *
 *  Created on: 20/gen/2012
 *      Author: Murtas Matteo
 */

#include "DmaChannel.h"

DmaChannel::DmaChannel() {
	// TODO Auto-generated constructor stub

}

DmaChannel::~DmaChannel() {
	// TODO Auto-generated destructor stub
}

void DmaChannel::begin(dma_dev *dev,
    	dma_channel txChannel,
    	dma_channel rxChannel,
    	void * txRegister,
    	void * rxRegister)
{
	_dev = dev;
	_txChannel = txChannel;
	_rxChannel = rxChannel;
	_txRegister = txRegister;
	_rxRegister = rxRegister;

	dma_init(_dev);
}

void DmaChannel::end()
{}

void DmaChannel::txAsync(uint8 *buff, uint16 lenBuff)
{
	_initTransfer(false, NULL, 0, buff, lenBuff);
}

bool DmaChannel::tx(uint8 *buff, uint16 lenBuff)
{
	//qui potrei impostare un timeout....
	txAsync(buff, lenBuff);
	while (!_tx_done)
		;
	return true;
}

bool DmaChannel::txDone()
{
	return _tx_done;
}


void DmaChannel::rxAsync(uint8 *cmd, uint16 lenCmd, uint8 *buff, uint16 lenBuff)
{
	_initTransfer(true, cmd, lenCmd, buff, lenBuff);
}

bool DmaChannel::rx(uint8 *cmd, uint16 lenCmd, uint8 *buff, uint16 lenBuff)
{
	//qui potrei impostare un timeout....
	rxAsync(cmd, lenCmd, buff, lenBuff);
	while (!_rx_done)
		;
	return true;
}

bool DmaChannel::rxDone()
{
	return _rx_done;
}


void DmaChannel::_txHandler(void * param)
{
	DmaChannel * ch = (DmaChannel *) param;
	if (ch)
		ch->_txCallback(0);
}

void DmaChannel::_rxHandler(void * param)
{
	DmaChannel * ch = (DmaChannel *) param;
	if (ch)
		ch->_rxCallback(0);
}


void DmaChannel::_txCallback(int status)
{
	_tx_done = true;
	_endTransfer();
}

void DmaChannel::_rxCallback(int status)
{
	_rx_done = true;
	_endTransfer();

}

void DmaChannel::_endTransfer()
{
	dma_disable(_dev, _txChannel);
	dma_disable(_dev, _rxChannel);
}

void DmaChannel::_initTransfer(bool receive, uint8 *cmd, uint16 lenCmd, uint8 *buff, uint16 lenBuff)
{
	uint16 rw_workbyte[] = { 0xffff };

	//dma_init(_dev);

	//spi_rx_dma_enable(SPI1);
	//spi_tx_dma_enable(SPI1);

	if ( receive )
	{
		//OKKIO... lenCmd dovrebbe essere uguale a lenBuff

		dma_setup_transfer(_dev, _rxChannel,
						   _rxRegister, DMA_SIZE_8BITS,
						   buff, DMA_SIZE_8BITS,
						   (DMA_MINC_MODE | DMA_TRNS_CMPLT));
		dma_set_priority(_dev, _rxChannel, DMA_PRIORITY_VERY_HIGH);
		dma_attach_interrupt(_dev, _rxChannel, &(DmaChannel::_rxHandler), (void*) this);

		dma_setup_transfer(_dev, _txChannel,
						   _txRegister, DMA_SIZE_8BITS,
						   cmd, DMA_SIZE_8BITS,
						   (DMA_MINC_MODE | DMA_FROM_MEM));
		dma_set_priority(_dev, _txChannel, DMA_PRIORITY_VERY_HIGH);

		dma_set_num_transfers(_dev, _rxChannel, lenCmd);
		dma_set_num_transfers(_dev, _txChannel, lenBuff);

		_rx_done = false;
		dma_enable(_dev, _txChannel);
		dma_enable(_dev, _rxChannel);


	}
	else
	{
		dma_setup_transfer(_dev, _rxChannel,
						   _rxRegister, DMA_SIZE_8BITS,
						   rw_workbyte, DMA_SIZE_8BITS,
						   0);
		dma_set_priority(_dev, _rxChannel, DMA_PRIORITY_VERY_HIGH);

		dma_setup_transfer(_dev, _txChannel,
						   _txRegister, DMA_SIZE_8BITS,
						   buff, DMA_SIZE_8BITS,
						   (DMA_MINC_MODE | DMA_FROM_MEM | DMA_TRNS_CMPLT));
		dma_set_priority(_dev, _txChannel, DMA_PRIORITY_VERY_HIGH);
		dma_attach_interrupt(_dev, _txChannel, &(DmaChannel::_txHandler), (void*) this);

		dma_set_num_transfers(_dev, _rxChannel, lenBuff);
		dma_set_num_transfers(_dev, _txChannel, lenBuff);

		_tx_done = false;
		dma_enable(_dev, _txChannel);
		dma_enable(_dev, _rxChannel);


	}
}
