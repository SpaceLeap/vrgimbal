/*
 * DmaChannel.h
 *
 *  Created on: 20/gen/2012
 *      Author: Murtas Matteo
 */

#ifndef DMACHANNEL_H_
#define DMACHANNEL_H_

#include "dma.h"

class DmaChannel {
public:
	DmaChannel();
	virtual ~DmaChannel();

	void begin(dma_dev *dev,
            	dma_channel txChannel,
            	dma_channel rxChannel,
            	void * txRegister,
            	void * rxRegister);
	void end();

	void txAsync(uint8 *buff, uint16 lenBuff);
	bool tx(uint8 *buff, uint16 lenBuff);
	bool txDone();

	void rxAsync(uint8 *cmd, uint16 lenCmd, uint8 *buff, uint16 lenBuff);
	bool rx(uint8 *cmd, uint16 lenCmd, uint8 *buff, uint16 lenBuff);
	bool rxDone();

private:

	dma_dev * _dev;
	dma_channel _txChannel;
	dma_channel _rxChannel;
	void * _txRegister;
	void * _rxRegister;

	bool _tx_done;
	bool _rx_done;

	static void _txHandler(void * param);
	static void _rxHandler(void * param);

	void _txCallback(int status);
	void _rxCallback(int status);

	void _endTransfer();
	void _initTransfer(bool receive, uint8 *cmd, uint16 lenCmd, uint8 *buff, uint16 lenBuff);

};

#endif /* DMACHANNEL_H_ */
