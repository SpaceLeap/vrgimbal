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
 *  @brief Delay implementation.
 */

#include "wirish_watchdog.h"

//#include "hal_types.h"
//#include "delay.h"
#include <iwdg.h>

void watchdog_init(uint8_t v, WDT_TIME sec){

	if (v == true){
		switch (sec){
			case WDT_10_SEC:
				iwdg_init(IWDG_PRE_128, 0x0C35);
				break;
			case WDT_16_SEC:
				iwdg_init(IWDG_PRE_256, 0x9C4);
				break;
			case WDT_20_SEC:
				iwdg_init(IWDG_PRE_256, 0xFA);
				break;
			case WDT_24_SEC:
				iwdg_init(IWDG_PRE_256, 0xEA6);
				break;
		}
	}
}

void watchdog_reload(void){
	iwdg_feed();
}
void watchdog_stop(void){
	//indipendent watchdog, once started, can't be stoppped
}
