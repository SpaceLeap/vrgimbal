/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @brief Wirish virtual serial port
 */

#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_

#include "BetterStream.h"

/**
 * @brief Virtual serial terminal.
 */
class USBSerial : public BetterStream {
public:
    USBSerial(void);

    virtual void begin(void);
    virtual void end(void);

    virtual int available(void);

    virtual int read(void *buf, uint32 len);
    virtual int read(void);

    virtual void write(uint8);
    virtual void write(const char *str);
    virtual void write(const void*, uint32);

    virtual uint8 getRTS();
    virtual uint8 getDTR();
    virtual uint8 isConnected();
    virtual uint8 pending();

    virtual int peek();
    virtual long getPortLic();
    virtual void flush();
};

//extern USBSerial SerialUSB;

#endif

