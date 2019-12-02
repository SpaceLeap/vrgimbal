/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
/*
    This file is part of VRGimbal by VirtualRobotix Italia s.c.a.r.l..

    VRGimbal is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VRGimbal is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VRGimbal.  If not, see <http://www.gnu.org/licenses/>.

    Please refer to http://vrgimbal.wordpress.com for more information
*/

/*
 * utilities.cpp
 *
 *  Created on: 17/ott/2013
 *      Author: Murtas Matteo
 */

#include "main.h"

void flash_leds(bool bOn)
{
	if (bOn) {
		LEDPIN_ON
	} else {
		LEDPIN_OFF
	}
}


void
print_blanks(int16_t num)
{
    while(num > 0) {
        num--;
        cliSerial->println("");
    }
}

void
print_divider(void)
{
    for (int i = 0; i < 40; i++) {
        cliSerial->print_P(PSTR("-"));
    }
    cliSerial->println();
}

void print_enabled(bool b)
{
    if(b)
        cliSerial->print_P(PSTR("en"));
    else
        cliSerial->print_P(PSTR("dis"));
    cliSerial->print_P(PSTR("abled\n"));
}


void setup_printf_P(const prog_char_t *fmt, ...)
{
    va_list arg_list;
    va_start(arg_list, fmt);
    cliSerial->vprintf_P(fmt, arg_list);
    va_end(arg_list);
}

void setup_wait_key(void)
{
    // wait for user input
    while (!cliSerial->available()) {
        delay(20);
    }
    // clear input buffer
    while( cliSerial->available() ) {
        cliSerial->read();
    }
}



