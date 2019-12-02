/*
 * AP_LANC.h
 *
 *  Created on: 20/ago/2013
 *      Author: Murtas Matteo
 */

#ifndef AP_LANC_H_
#define AP_LANC_H_

class AP_LANC {
public:
	AP_LANC();
	virtual ~AP_LANC();

	void Init(int pin);

	void SendCode(int type,int code);


protected:
	void frameStartBitWait();
	static void lowWait(int pin);
	static void writeByte(int pin, unsigned char value, unsigned uSec /* bit width */);
	static unsigned char readByte(int pin,unsigned long uSec /* bit width*/ );

	int m_LANC_X_PIN;
};

#endif /* AP_LANC_H_ */
